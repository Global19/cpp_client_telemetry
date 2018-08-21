#include "MemoryStorage.hpp"

#include "utils/Utils.hpp"
#include <climits>

namespace ARIASDK_NS_BEGIN {

    ARIASDK_LOG_INST_COMPONENT_CLASS(MemoryStorage, "EventsSDK.MemoryStorage", "Events telemetry client - MemoryStorage class");

    MemoryStorage::MemoryStorage(ILogManager & logManager, IRuntimeConfig & runtimeConfig) :
        m_logManager(logManager),
        m_config(runtimeConfig),
        m_size(0),
        m_lastReadCount(0)
    {
    }

    void MemoryStorage::Initialize(IOfflineStorageObserver & observer)
    {
        m_observer = &observer;
    }

    void MemoryStorage::Shutdown()
    {
        // TODO: #1 [MG] - verify that all contents have been flushed.
        // TODO: #2 [MG] - enable 'emergency' recovery 
    }

    void MemoryStorage::Flush()
    {
        // TODO: [MG] - consider moving Flush() implementation from OfflineStorageHandler::Flush()
        // to this class in order to provide tight coupling between this instance of MemoryStorage
        // and a corresponding OfflineStorage_SQLite instance.
    }

    bool MemoryStorage::StoreRecord(StorageRecord const & record)
    {
        // XXX: [MG] - locking is not needed here because OfflineStorageHandler provides the locking
        LOCKGUARD(m_lock);

        m_size += record.blob.size() + sizeof(record); // approximate contents size

        // TODO: [MG] - verify is m_size is larger than the ram queue size.
        // If so, then perform Flush to copy all ram contents to disk

        m_records[record.latency].push_back(std::move(record));
        return true;
    }

    // XXX: [MG] - reservation is not required for ram records. What's needed here is that in case of a retry,
    // we should keep the retry counter on a vector that contains all 'reserved' records (records pending upload)
    bool MemoryStorage::GetAndReserveRecords(std::function<bool(StorageRecord&&)> const & consumer, unsigned leaseTimeMs, EventLatency minLatency, unsigned maxCount)
    {
        UNREFERENCED_PARAMETER(leaseTimeMs);

        LOG_TRACE("Retrieving max. %u%s events of latency at least %d (%s)",
            maxCount, (maxCount > 0) ? "" : " (unlimited)",
            minLatency, latencyToStr(static_cast<EventLatency>(minLatency)));

        if (maxCount == 0)
            maxCount = UINT_MAX;

        LOCKGUARD(m_lock);
        m_lastReadCount = 0;
        for (unsigned latency = minLatency; (latency < EventLatency_Max) && (maxCount); latency++)
        {
            while (maxCount && (m_records[latency]).size() )
            {
                m_lastReadCount++;
                StorageRecord & record = m_records[latency].back();
                size_t recordSize = record.blob.size() + sizeof(record);
                consumer(std::move(record));
                m_records[latency].pop_back();
                if (m_size.load() > recordSize)
                {
                    m_size -= recordSize;
                }
                else
                {
                    m_size = 0;
                }
                maxCount--;
            }
        }

        return true;
    }

    bool MemoryStorage::IsLastReadFromMemory()
    {
        return true;
    }

    unsigned MemoryStorage::LastReadRecordCount()
    {
        return m_lastReadCount.load();
    }

    /// <summary>
    /// XXX: [MG] - ram storage doesn't support deleting records
    /// </summary>
    /// <param name="ids"></param>
    /// <param name="headers"></param>
    /// <param name="fromMemory"></param>
    void MemoryStorage::DeleteRecords(std::vector<StorageRecordId> const & ids, HttpHeaders headers, bool & fromMemory)
    {
        UNREFERENCED_PARAMETER(ids);
        UNREFERENCED_PARAMETER(headers);
        UNREFERENCED_PARAMETER(fromMemory);
    }

    /// <summary>
    /// XXX: [MG] - ram storage doesn't support reserving records
    /// </summary>
    /// <param name="ids"></param>
    /// <param name="incrementRetryCount"></param>
    /// <param name="headers"></param>
    /// <param name="fromMemory"></param>
    void MemoryStorage::ReleaseRecords(std::vector<StorageRecordId> const & ids, bool incrementRetryCount, HttpHeaders headers, bool & fromMemory)
    {
        UNREFERENCED_PARAMETER(ids);
        UNREFERENCED_PARAMETER(incrementRetryCount);
        UNREFERENCED_PARAMETER(headers);
        UNREFERENCED_PARAMETER(fromMemory);
    }

    /// <summary>
    /// Storing settings in RAM storage is not supported
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    bool MemoryStorage::StoreSetting(std::string const & name, std::string const & value)
    {
        UNREFERENCED_PARAMETER(name);
        UNREFERENCED_PARAMETER(value);
        return false;
    }

    /// <summary>
    /// Retrieving settings from RAM storage is not supported
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    std::string MemoryStorage::GetSetting(std::string const & name)
    {
        UNREFERENCED_PARAMETER(name);
        return std::string();
    }

    unsigned MemoryStorage::GetSize()
    {
        return m_size.load();
    }

    std::vector<StorageRecord>* MemoryStorage::GetRecords(bool shutdown, EventLatency minLatency, unsigned maxCount)
    {
        UNREFERENCED_PARAMETER(shutdown);
        UNREFERENCED_PARAMETER(minLatency);
        UNREFERENCED_PARAMETER(maxCount);

        std::vector<StorageRecord>* records = new std::vector<StorageRecord>();
        auto consumer = [records, this](StorageRecord&& record) -> bool {
            records->push_back(std::move(record));
            bool wantMore = true;
            return wantMore;
        };
        GetAndReserveRecords(consumer, 0, minLatency, maxCount);
        return records;
    }

    bool MemoryStorage::ResizeDb()
    {
        return true;
    }

    MemoryStorage::~MemoryStorage()
    {
    }

} ARIASDK_NS_END
