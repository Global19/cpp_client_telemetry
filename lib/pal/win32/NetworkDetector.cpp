#ifndef NO_ROAM_SUP

// This macro is required for DEFINE_GUID below to declare a local instance of IID_INetworkCostManager GUID
#define INITGUID

#include "NetworkDetector.hpp"
#include <algorithm>

#include "LogManager.hpp"
#include "DebugEvents.hpp"
#include "utils/Utils.hpp"

// Define a GUID that is only available in Windows 8.x+ SDK . We are using Windows 7.1A SDK for Win32 SDK build,
// so we cannot easily add an extra dependency on Windows 8 or later functionality project-wide. It'd be error-prone,
// because when we have all Windows 8+ features - we might fall into temptation of using that features that would
// break Windows 7.1 compatibility. We cannot afford breaking Windows 7.1 compatibility at this time.
DEFINE_GUID(IID_INetworkCostManager2, 0xdcb00008, 0x570f, 0x4a9b, 0x8d, 0x69, 0x19, 0x9f, 0xdb, 0xa5, 0x72, 0x3b);

namespace Microsoft {
    namespace Applications {
        namespace Telemetry {
            namespace Windows {

                // Malwarebytes have been detected
                static bool mbDetected = false;

                /// <summary>
                /// Convert HString to std::string
                /// </summary>
                /// <param name="name"></param>
                /// <returns></returns>
                std::string to_string(const HString *name)
                {
                    UINT32 length;
                    PCWSTR rawString = name->GetRawBuffer(&length);
                    std::wstring wide(rawString);
                    std::string str(wide.begin(), wide.end());
                    return str;
                }

                /// <summary>
                /// Convert GUID to std::string
                /// </summary>
                /// <param name="guid"></param>
                /// <returns></returns>
                std::string to_string(GUID guid)
                {
                    return GuidtoString(guid);;
                }

                NetworkCost const& NetworkDetector::GetNetworkCost() const
                {
                    return (NetworkCost const &)m_currentNetworkCost;
                }

                /// <summary>
                /// Get current realtime network cost synchronously.
                /// This function can be called on any Windows release and it provides a SEH handler.
                /// </summary>
                /// <returns></returns>
#pragma warning(push)
#pragma warning(disable:6320)
                int NetworkDetector::GetCurrentNetworkCost()
                {
#if 0
                    // We don't know the cost of something that is not there
                    if (m_connectivity == NLM_CONNECTIVITY_DISCONNECTED)
                    {
                        ARIASDK_LOG_DETAIL("Disconnected!");
                        m_currentNetworkCost = NetworkCost_Unknown;
                    }
#endif
                    m_currentNetworkCost = NetworkCost_Unknown;
                    try {
                        m_currentNetworkCost = _GetCurrentNetworkCost();
                    }
                    //******************************************************************************************************************************
                    // XXX: Bug in Visual Studio debug host:
                    //
                    // onecoreuap\net\netprofiles\winrt\networkinformation\lib\handlemanager.cpp(132)\Windows.Networking.Connectivity.dll!0FBCFB9E:
                    // (caller: 0FBCEE2C) ReturnHr(1) tid(4584) 80070426 The service has not been started.
                    //
                    // Exception thrown at XXX (KernelBase.dll) in YYY : The binding handle is invalid.
                    // If there is a handler for this exception, the program may be safely continued.
                    //*******************************************************************************************************************************
                    catch(...)
                    {
                        ARIASDK_LOG_ERROR("Unable to obtain network state!");
                        m_currentNetworkCost = NetworkCost_Unknown;
                    }

                    // Notify the app about current network cost change
                    DebugEvent evt;
                    evt.type = DebugEventType::EVT_NET_CHANGED;
                    evt.param1 = m_currentNetworkCost;
                    evt.param2 = mbDetected;
                    LogManager::DispatchEvent(evt);

                    return m_currentNetworkCost;
                }
#pragma warning(pop)

                /// <summary>
                /// Get current network connectivity state
                /// </summary>
                /// <returns>Value of enum NLM_CONNECTIVITY</returns>
                int NetworkDetector::GetConnectivity() const
                {
                    return m_connectivity;
                }

                /// <summary>
                /// Internal implementation
                /// </summary>
                /// <returns></returns>
                NetworkCost NetworkDetector::_GetCurrentNetworkCost()
                {
                    std::lock_guard<std::mutex> lock(m_lock);

                    NetworkCost result = NetworkCost_Unknown;
                    ARIASDK_LOG_DETAIL("get network cost...\n");

                    if (pNlm == NULL) {
						ARIASDK_LOG_WARNING("INetworkCostManager is unavailable!");
                        return result;
                    }

                    HRESULT hr;

                    DWORD dwCost = NLM_CONNECTION_COST_UNKNOWN;
                    INetworkCostManager* pNetworkCostManager = NULL;

                    hr = pNlm->QueryInterface(IID_INetworkCostManager2, (void**)&pNetworkCostManager);
                    if (hr != S_OK) {
                        return result;
                    }

                    hr = pNetworkCostManager->GetCost(&dwCost, NULL);
                    if (hr == S_OK) {
                        switch (dwCost) {
                        case NLM_CONNECTION_COST_UNRESTRICTED:  // The connection is unlimited and is considered to be unrestricted of usage charges and capacity constraints.
                            result = NetworkCost_Unmetered;
                            break;
                        case NLM_CONNECTION_COST_FIXED:         // The use of this connection is unrestricted up to a specific data transfer limit.
                        case NLM_CONNECTION_COST_VARIABLE:      // This connection is regulated on a per byte basis.
                            result = NetworkCost_Metered;
                            break;
                        case NLM_CONNECTION_COST_OVERDATALIMIT: // The connection is currently in an OverDataLimit state as it has exceeded the carrier specified data transfer limit.
                        case NLM_CONNECTION_COST_CONGESTED:     // The network is experiencing high traffic load and is congested.
                        case NLM_CONNECTION_COST_ROAMING:       // The connection is roaming outside the network and affiliates of the home provider.
                        case NLM_CONNECTION_COST_APPROACHINGDATALIMIT:  // The connection is approaching the data limit specified by the carrier.
                            result = NetworkCost_Roaming;
                            break;
                        case NLM_CONNECTION_COST_UNKNOWN:
                        default:
                            result = NetworkCost_Unknown;       // The cost is unknown.
                            break;
                        }
                    }
                    return result;
                }

                /// <summary>
                /// COM thread interfaces supported by this class
                /// </summary>
                /// <param name="riid"></param>
                /// <param name="ppv"></param>
                /// <returns></returns>
                HRESULT NetworkDetector::QueryInterface(REFIID riid, void ** ppv)
                {
                    static const QITAB rgqit[] =
                    {
                        QITABENT(NetworkDetector, INetworkEvents),
                        QITABENT(NetworkDetector, INetworkConnectionEvents),
                        QITABENT(NetworkDetector, INetworkListManagerEvents),
                        { 0 }
                    };
                    return QISearch(this, rgqit, riid, ppv);
                }

                ULONG NetworkDetector::AddRef(void)
                {
                    return InterlockedIncrement((LONG *)&m_lRef);
                }

                ULONG NetworkDetector::Release(void)
                {
                    ULONG ulNewRef = (ULONG)InterlockedDecrement((LONG *)&m_lRef);
                    if (ulNewRef == 0)
                    {
                        delete this;
                    }
                    return ulNewRef;
                }

                HRESULT NetworkDetector::ConnectivityChanged(NLM_CONNECTIVITY newConnectivity)
                {
                    ARIASDK_LOG_DETAIL("Connectivity changed: %d", newConnectivity);
                    m_connectivity = newConnectivity;
                    GetCurrentNetworkCost();
                    return RPC_S_OK;
                }

                HRESULT NetworkDetector::NetworkAdded(GUID networkId)
                {
                    ARIASDK_LOG_DETAIL("NetworkAdded: %s", to_string(networkId).c_str());
                    m_networks.push_back(to_string(networkId));
                    return RPC_S_OK;
                }

                HRESULT NetworkDetector::NetworkDeleted(GUID networkId)
                {
                    ARIASDK_LOG_DETAIL("NetworkDeleted: %s", to_string(networkId).c_str());
                    auto &v = m_networks;
                    const std::string &item = to_string(networkId);
                    v.erase(std::remove(v.begin(), v.end(), item), v.end());
                    return RPC_S_OK;
                }

                HRESULT NetworkDetector::NetworkConnectivityChanged(GUID networkId, NLM_CONNECTIVITY newConnectivity)
                {
                    ARIASDK_LOG_DETAIL("NetworkConnectivityChanged: %s, %d", to_string(networkId).c_str(), newConnectivity);
                    m_networks_connectivity[to_string(networkId)] = newConnectivity;
                    return RPC_S_OK;
                }

                HRESULT NetworkDetector::NetworkPropertyChanged(GUID networkId, NLM_NETWORK_PROPERTY_CHANGE flags)
                {
                    ARIASDK_LOG_DETAIL("NetworkPropertyChanged: %s, %d", to_string(networkId).c_str(), flags);
                    GetCurrentNetworkCost();
                    return RPC_S_OK;
                }

                HRESULT NetworkDetector::NetworkConnectionConnectivityChanged(GUID connectionId, NLM_CONNECTIVITY newConnectivity)
                {
                    ARIASDK_LOG_DETAIL("NetworkConnectionConnectivityChanged: %s, %d", to_string(connectionId).c_str(), newConnectivity);
                    m_connections_connectivity[to_string(connectionId)] = newConnectivity;
                    return RPC_S_OK;
                }

                HRESULT NetworkDetector::NetworkConnectionPropertyChanged(GUID connectionId, NLM_CONNECTION_PROPERTY_CHANGE flags)
                {
                    ARIASDK_LOG_DETAIL("NetworkConnectionPropertyChanged: %s, %d", to_string(connectionId).c_str(), flags);
                    return RPC_S_OK;
                }

                /// <summary>
                /// Get activation factory and look-up network info statistics
                /// </summary>
                /// <returns></returns>
                bool NetworkDetector::GetNetworkInfoStats()
                {
                    HRESULT hr = GetActivationFactory(HString::MakeReference(RuntimeClass_Windows_Networking_Connectivity_NetworkInformation).Get(), &networkInfoStats);
                    if (hr != S_OK)
                    {
                        ARIASDK_LOG_ERROR("Unable to get Windows::Networking::Connectivity::NetworkInformation");
                        return false;
                    }
                    return true;
                }

                bool NetworkDetector::RegisterAndListen()
                {
                    // ???
                    HRESULT hr = pNlm->QueryInterface(IID_IUnknown, (void**)&pSink);
                    if (FAILED(hr))
                    {
                        ARIASDK_LOG_ERROR("cannot query IID_IUnknown!!!");
                        return false;
                    }

                    pSink = (INetworkEvents*)this;

                    hr = pNlm->QueryInterface(IID_IConnectionPointContainer, (void**)&pCpc);
                    if (FAILED(hr))
                    {
                        ARIASDK_LOG_ERROR("Unable to QueryInterface IID_IConnectionPointContainer!");
                        return false;
                    }

                    hr = pCpc->FindConnectionPoint(IID_INetworkConnectionEvents, &m_pc1);
                    if (SUCCEEDED(hr))
                    {
                        hr = m_pc1->Advise(
                            pSink.Get(),
                            &m_dwCookie_INetworkConnectionEvents);
                        ARIASDK_LOG_INFO("listening to INetworkConnectionEvents... %s",
                            (SUCCEEDED(hr)) ? "OK" : "FAILED");
                    }

                    hr = pCpc->FindConnectionPoint(IID_INetworkEvents, &m_pc2);
                    if (SUCCEEDED(hr))
                    {
                        hr = m_pc2->Advise(
                            pSink.Get(),
                            &m_dwCookie_INetworkEvents);
                        ARIASDK_LOG_INFO("listening to INetworkEvents... %s",
                            (SUCCEEDED(hr)) ? "OK" : "FAILED");
                    }

                    hr = pCpc->FindConnectionPoint(IID_INetworkListManagerEvents, &m_pc3);
                    if (SUCCEEDED(hr))
                    {
                        hr = m_pc3->Advise(
                            pSink.Get(),
                            &m_dwCookie_INetworkListManagerEvents);
                        ARIASDK_LOG_INFO("listening to INetworkListManagerEvents... %s",
                            (SUCCEEDED(hr)) ? "OK" : "FAILED");
                    }

                    isRunning = true;
                    BOOL bRet;
                    MSG msg;
                    m_listener_tid = GetCurrentThreadId();
                    cv.notify_all();
                    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
                    {
                        if (bRet == -1)
                        {
                            break;
                        }
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                    isRunning = false;
                    Unregister();
                    return true;
                }

                /// <summary>
                /// 
                /// </summary>
                void NetworkDetector::Unregister()
                {
                    if (m_pc1 != NULL)
                        m_pc1->Unadvise(m_dwCookie_INetworkConnectionEvents);

                    if (m_pc2 != NULL)
                        m_pc2->Unadvise(m_dwCookie_INetworkEvents);

                    if (m_pc3 != NULL)
                        m_pc3->Unadvise(m_dwCookie_INetworkListManagerEvents);
                }

                /// <summary>
                /// Register for COM events and block-wait in RegisterAndListen
                /// </summary>
#pragma warning(push)
#pragma warning(disable:6320)
                void NetworkDetector::run()
                {
                    if (isRunning)
                    {
						ARIASDK_LOG_WARNING("Already running!");
                        return;
                    }

					static HMODULE hNtDll = ::GetModuleHandle(TEXT("ntdll.dll"));
					typedef HRESULT NTSTATUS;
					typedef NTSTATUS(__stdcall * RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
					static RtlGetVersion_t pRtlGetVersion = hNtDll ? reinterpret_cast<RtlGetVersion_t>(::GetProcAddress(hNtDll, "RtlGetVersion")) : nullptr;

					RTL_OSVERSIONINFOW rtlOsvi = { sizeof(rtlOsvi) };
					OSVERSIONINFOW osvi = { sizeof(osvi) };
					BOOL bIsWindows8orLater = false;
#pragma warning(suppress:4996) // warning C4996: 'GetVersionExW': was declared deprecated
					if (pRtlGetVersion && SUCCEEDED(pRtlGetVersion(&rtlOsvi)))
					{
						bIsWindows8orLater = ((rtlOsvi.dwMajorVersion >= 6) && (rtlOsvi.dwMinorVersion >= 2)) || (rtlOsvi.dwMajorVersion > 6);

					}
//					else if (::GetVersionExW(&osvi))
//					{
//						bIsWindows8orLater = ((osvi.dwMajorVersion >= 6) && (osvi.dwMinorVersion >= 2)) || (osvi.dwMajorVersion > 6);
//					}
					

                    // Check Windows version and if below Windows 8, then avoid running Network cost detection logic                 
                    // Applications not manifested for Windows 8.1 or Windows 10 will return the Windows 8 OS version value (6.2)
                    if (!bIsWindows8orLater)
                    {
                        isRunning = false;
                        ARIASDK_LOG_INFO("Running on Windows %d.%d without network detector...", osvi.dwMajorVersion, osvi.dwMinorVersion);
                        return;
                    }

                    try
                    {
                        HRESULT hr = CoInitialize(nullptr);
                        if (FAILED(hr))
                        {
                            ARIASDK_LOG_ERROR("CoInitialize Failed.");
                            return;
                        }
                        if (GetNetworkInfoStats())
                        {
                            ARIASDK_LOG_INFO("create network list manager...");
                            hr = CoCreateInstance(
                                CLSID_NetworkListManager,
                                NULL,
                                CLSCTX_ALL,
                                IID_INetworkListManager,
                                (void**)&pNlm);
                            if (FAILED(hr))
                            {
                                ARIASDK_LOG_ERROR("Unable to CoCreateInstance for CLSID_NetworkListManager!");
                            }
							else
							{
                            	GetCurrentNetworkCost();
                            	ARIASDK_LOG_DETAIL("start listening to events...");
                            	RegisterAndListen(); // we block here to process COM events
							}
                            networkInfoStats.Detach();
                            if (pNlm != NULL)
                            {
                                ARIASDK_LOG_DETAIL("release network list manager...");
                                pNlm->Release();
                            };
                        }
                    }
                    catch(...)
                    {
                        ARIASDK_LOG_ERROR("Handled exception in network cost detection (Windows 7?)");
                        isRunning = false;
                    }
                    CoUninitialize();
                }
#pragma warning(pop)

                /// <summary>
                /// Start network monitoring thread
                /// </summary>
                /// <returns>true - if start is successful, false - otherwise</returns>
                bool NetworkDetector::Start()
                {
                    // Start a new thread. Notify waiters on exit.
                    netDetectThread = std::thread([this]() {
                        run();
                        cv.notify_all();
                    });
                    netDetectThread.detach();

                    // Wait for up to 1 second for COM mumbo-jumbo to complete
                    std::unique_lock<std::mutex> lock(m_lock);
                    cv.wait_for(lock, std::chrono::milliseconds(1000));

                    return isRunning;
                };

                /// <summary>
                /// Stop network monitoring thread
                /// </summary>
                void NetworkDetector::Stop()
                {
                    PostThreadMessage(m_listener_tid, (UINT)-1, 0, NULL);
                    if ( isRunning && netDetectThread.joinable() )
                    {
                        std::unique_lock<std::mutex> lock(m_lock);
                        cv.wait_for(lock, std::chrono::milliseconds(1000));
                    }
                };

                /// <summary>
                /// NetworkDetector destructor, performs Stop() if running
                /// </summary>
                /// <returns></returns>
                NetworkDetector::~NetworkDetector()
                {
                    ARIASDK_LOG_DETAIL("Shutting down NetworkDetector...");
                    Stop();
                    ARIASDK_LOG_DETAIL("NetworkDetector destroyed.");
                }


                const std::map<std::string, NLM_CONNECTIVITY>& NetworkDetector::GetNetworksConnectivity() const
                {
                    return m_networks_connectivity;
                }

                const std::map<std::string, NLM_CONNECTIVITY>& NetworkDetector::GetConnectionsConnectivity() const
                {
                    return m_connections_connectivity;
                }
            }
        }
    }
}
#endif