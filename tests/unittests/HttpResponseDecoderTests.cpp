// Copyright (c) Microsoft. All rights reserved .

#include "common/Common.hpp"
#include "common/MockIHttpClient.hpp"
#include "http/HttpResponseDecoder.hpp"

using namespace testing;
using namespace ARIASDK_NS;


class HttpResponseDecoderTests : public StrictMock<Test> {
  protected:
    HttpResponseDecoder                                                decoder;
    RouteSink<HttpResponseDecoderTests, EventsUploadContextPtr const&> eventsAccepted{this, &HttpResponseDecoderTests::resultEventsAccepted};
    RouteSink<HttpResponseDecoderTests, EventsUploadContextPtr const&> eventsRejected{this, &HttpResponseDecoderTests::resultEventsRejected};
    RouteSink<HttpResponseDecoderTests, EventsUploadContextPtr const&> temporaryNetworkFailure{this, &HttpResponseDecoderTests::resultTemporaryNetworkFailure};
    RouteSink<HttpResponseDecoderTests, EventsUploadContextPtr const&> temporaryServerFailure{this, &HttpResponseDecoderTests::resultTemporaryServerFailure};
    RouteSink<HttpResponseDecoderTests, EventsUploadContextPtr const&> requestAborted{this, &HttpResponseDecoderTests::resultRequestAborted};

  protected:
    HttpResponseDecoderTests()
    {
        decoder.eventsAccepted          >> eventsAccepted;
        decoder.eventsRejected          >> eventsRejected;
        decoder.temporaryNetworkFailure >> temporaryNetworkFailure;
        decoder.temporaryServerFailure  >> temporaryServerFailure;
        decoder.requestAborted          >> requestAborted;
    }

    MOCK_METHOD1(resultEventsAccepted,          void(EventsUploadContextPtr const &));
    MOCK_METHOD1(resultEventsRejected,          void(EventsUploadContextPtr const &));
    MOCK_METHOD1(resultTemporaryNetworkFailure, void(EventsUploadContextPtr const &));
    MOCK_METHOD1(resultTemporaryServerFailure,  void(EventsUploadContextPtr const &));
    MOCK_METHOD1(resultRequestAborted,          void(EventsUploadContextPtr const &));

    EventsUploadContextPtr createContextWith(HttpResult result, int status, std::string const& body)
    {
        std::unique_ptr<SimpleHttpResponse> rsp(new SimpleHttpResponse("HttpResponseDecoderTests"));
        rsp->m_result     = result;
        rsp->m_statusCode = status;
        rsp->m_body.assign(reinterpret_cast<uint8_t const*>(body.data()), reinterpret_cast<uint8_t const*>(body.data()) + body.size());

        EventsUploadContextPtr ctx = EventsUploadContext::create();
        ctx->httpRequestId = rsp->GetId();
        ctx->httpResponse = std::move(rsp);
        ctx->durationMs = 1234;
        return ctx;
    }
};


TEST_F(HttpResponseDecoderTests, AcceptsAccepted)
{
    auto ctx = createContextWith(HttpResult_OK, 200, "");
    EXPECT_CALL(*this, resultEventsAccepted(ctx))
        .WillOnce(Return());
    decoder.decode(ctx);
}

TEST_F(HttpResponseDecoderTests, RejectsRejected)
{
    auto ctx = createContextWith(HttpResult_OK, 404, "<h1>Service not found</h1>");
    EXPECT_CALL(*this, resultEventsRejected(ctx))
        .WillOnce(Return());
    decoder.decode(ctx);
}

TEST_F(HttpResponseDecoderTests, UnderstandsTemporaryServerFailures)
{
    auto ctx = createContextWith(HttpResult_OK, 500, "{error:500,detail:\"Bad karma\"}");
    EXPECT_CALL(*this, resultTemporaryServerFailure(ctx))
        .WillOnce(Return());
    decoder.decode(ctx);

    ctx = createContextWith(HttpResult_OK, 408, "Timeout");
    EXPECT_CALL(*this, resultTemporaryServerFailure(ctx))
        .WillOnce(Return());
    decoder.decode(ctx);
}

TEST_F(HttpResponseDecoderTests, UnderstandsTemporaryNetworkFailures)
{
    auto ctx = createContextWith(HttpResult_LocalFailure, -1, "");
    EXPECT_CALL(*this, resultTemporaryNetworkFailure(ctx))
        .WillOnce(Return());
    decoder.decode(ctx);

    ctx = createContextWith(HttpResult_NetworkFailure, -1, "");
    EXPECT_CALL(*this, resultTemporaryNetworkFailure(ctx))
        .WillOnce(Return());
    decoder.decode(ctx);
}

TEST_F(HttpResponseDecoderTests, SkipsAbortedRequests)
{
    auto ctx = createContextWith(HttpResult_Aborted, -1, "");
    EXPECT_CALL(*this, resultRequestAborted(ctx))
        .WillOnce(Return());
    decoder.decode(ctx);
}
