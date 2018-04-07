// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include <api/IRuntimeConfig.hpp>

namespace testing {

    class MockIRuntimeConfig : public ARIASDK_NS::IRuntimeConfig {
    public:
        MockIRuntimeConfig();
        virtual ~MockIRuntimeConfig();

        MOCK_METHOD1(SetDefaultConfig, void(IRuntimeConfig &));
        MOCK_METHOD3(DecorateEvent, void(std::map<std::string, std::string> &, std::string const &, std::string const &));
        MOCK_METHOD0(GetCollectorUrl, std::string());
        MOCK_METHOD0(GetMetaStatsTenantToken, std::string());
        MOCK_METHOD2(GetEventLatency, ARIASDK_NS::EventLatency(std::string const &, std::string const &));
        MOCK_METHOD0(GetMetaStatsSendIntervalSec, unsigned());
        MOCK_METHOD0(GetOfflineStorageMaximumSizeBytes, unsigned());
        MOCK_METHOD0(GetOfflineStorageResizeThresholdPct, unsigned());
        MOCK_METHOD0(GetMaximumRetryCount, unsigned());
        MOCK_METHOD0(GetUploadRetryBackoffConfig, std::string());
        MOCK_METHOD0(IsHttpRequestCompressionEnabled, bool());
        MOCK_METHOD0(GetMinimumUploadBandwidthBps, unsigned());
        MOCK_METHOD0(GetMaximumUploadSizeBytes, unsigned());
        MOCK_METHOD3(SetEventLatency, void(std::string const &, std::string const &, ARIASDK_NS::EventLatency));
        MOCK_METHOD0(GetTeardownTime, uint32_t());
        MOCK_METHOD0(IsClockSkewEnabled, bool());

        // FIXME: Google Mock doesn't support operators
        virtual ARIASDK_NS::Variant & operator[](const char* key)
        {
            return (*this)[key];
        };

    };


} // namespace testing
