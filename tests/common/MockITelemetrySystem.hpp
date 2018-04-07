// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include <system/ITelemetrySystem.hpp>

using namespace MAT;

namespace testing {

    class MockITelemetrySystem : public ITelemetrySystem {
    public:
        MockITelemetrySystem() {};
        virtual ~MockITelemetrySystem() {};

        MOCK_METHOD0(start, void());
        MOCK_METHOD0(stop, void());
        MOCK_METHOD0(pause, void());
        MOCK_METHOD0(resume, void());
        MOCK_METHOD0(upload, void());
        MOCK_METHOD0(getLogManager, ILogManager&());
        MOCK_METHOD0(getConfig, IRuntimeConfig&());
        MOCK_METHOD0(getContext, ISemanticContext&());
        MOCK_METHOD1(DispatchEvent, bool(DebugEvent evt));
        MOCK_METHOD1(sendEvent, void(IncomingEventContextPtr const& event));
        MOCK_METHOD0(startAsync, void());
        MOCK_METHOD0(stopAsync, void());
        MOCK_METHOD0(handleFlushWorkerThread, void());
        MOCK_METHOD0(signalDone, void());
        MOCK_METHOD0(pauseAsync, void());
        MOCK_METHOD0(resumeAsync, void());
        MOCK_METHOD1(handleIncomingEventPrepared, void(IncomingEventContextPtr const& event));
        MOCK_METHOD1(preparedIncomingEventAsync, void(IncomingEventContextPtr const& event));
    };

}  // namespace ARIASDK_NS_BEGIN
