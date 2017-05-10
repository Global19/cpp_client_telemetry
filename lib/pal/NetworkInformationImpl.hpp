#pragma once
#include "include\aria\Version.hpp"
#include "include\aria\Enums.hpp"
#include "include\aria\INetworkInformation.hpp"
#include "pal\InformationProviderimpl.hpp"


namespace Microsoft { namespace Applications { namespace Telemetry {
namespace PAL {

    class NetworkInformationImpl : public INetworkInformation
    {
    public:
        static INetworkInformation* Create();

        // IInformationProvider API
        virtual int  RegisterInformationChangedCallback(IPropertyChangedCallback* pCallback) { return m_info_helper.RegisterInformationChangedCallback(pCallback); }
        virtual void UnRegisterInformationChangedCallback(int callbackToken) { m_info_helper.UnRegisterInformationChangedCallback(callbackToken); }

        // INetworkInformation API
        virtual std::string const& GetNetworkProvider() { return m_provider; };
        virtual NetworkType GetNetworkType() { return m_type; }
        virtual NetworkCost GetNetworkCost() { return m_cost; }

        virtual bool IsEthernetAvailable();
        virtual bool IsWifiAvailable();
        virtual bool IsWwanAvailable();

        NetworkInformationImpl();
        virtual ~NetworkInformationImpl() { };

    protected:
        std::string m_provider;
        NetworkType m_type;
        NetworkCost m_cost;

        InformatonProviderImpl m_info_helper;

        // Disable copy constructor and assignment operator.
        NetworkInformationImpl(NetworkInformationImpl const& other) = delete;
        NetworkInformationImpl& operator=(NetworkInformationImpl const& other) = delete;
    };

} // PlatformAbstraction
}}}