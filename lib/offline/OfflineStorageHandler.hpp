// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "pal/PAL.hpp"
#include <IOfflineStorage.hpp>
#include <IRuntimeConfig.hpp>
#include <LogConfiguration.hpp>
#include <memory>

namespace ARIASDK_NS_BEGIN {
        
class OfflineStorageHandler : public IOfflineStorage,
                              public IOfflineStorageObserver,
                              public PAL::RefCountedImpl<OfflineStorageHandler>
{
  public:
    OfflineStorageHandler(LogConfiguration const& configuration, IRuntimeConfig& runtimeConfig);
    virtual ~OfflineStorageHandler() override;
    virtual void Initialize(IOfflineStorageObserver& observer) override;
    virtual void Shutdown() override;
    virtual bool StoreRecord(StorageRecord const& record) override;
    virtual bool GetAndReserveRecords(std::function<bool(StorageRecord&&)> const& consumer, unsigned leaseTimeMs, EventPriority minPriority = EventPriority_Unspecified, unsigned maxCount = 0 ) override;
    virtual bool IsLastReadFromMemory() override;
    virtual unsigned LastReadRecordCount() override;
    virtual void DeleteRecords(std::vector<StorageRecordId> const& ids, HttpHeaders headers, bool& fromMemory) override;
    virtual void ReleaseRecords(std::vector<StorageRecordId> const& ids, bool incrementRetryCount, HttpHeaders headers, bool& fromMemory) override;
    virtual bool StoreSetting(std::string const& name, std::string const& value) override;
    virtual std::string GetSetting(std::string const& name) override;
    virtual unsigned GetSize() override;
    virtual std::vector<StorageRecord>* GetRecords(bool shutdown, EventPriority minPriority = EventPriority_Unspecified, unsigned maxCount = 0) override;

    virtual void OnStorageOpened(std::string const& type) override;
    virtual void OnStorageFailed(std::string const& reason) override;
    virtual void OnStorageTrimmed(unsigned numRecords) override;
    virtual void OnStorageRecordsDropped(unsigned numRecords) override;

  protected:
    IOfflineStorageObserver*    m_observer;
    LogConfiguration            m_logConfiguration;
    std::string                 m_databasePath;
    IRuntimeConfig&             m_runtimeConfig;
    std::unique_ptr<IOfflineStorage>       m_offlineStorageMemory;
    std::unique_ptr<IOfflineStorage>       m_offlineStorageDisk;
    bool                                   m_readFromMemory;
    unsigned                               m_lastReadCount;

  protected:
    ARIASDK_LOG_DECL_COMPONENT_CLASS();
};


} ARIASDK_NS_END
