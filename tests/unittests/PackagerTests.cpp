// Copyright (c) Microsoft. All rights reserved .

#include "common/Common.hpp"
#include "common/MockIRuntimeConfig.hpp"
#include "utils/Utils.hpp"
#include "packager/Packager.hpp"
#include <bond_lite/All.hpp>
#include "bond/generated/AriaProtocol_types.hpp"
#include "bond/generated/AriaProtocol_readers.hpp"

using namespace testing;
using namespace ARIASDK_NS;


class PackagerTests : public StrictMock<Test> {
  protected:
    LogConfiguration               logConfiguration;
    StrictMock<MockIRuntimeConfig> runtimeConfigMock;
    Packager                       packager;

    RouteSink<PackagerTests, EventsUploadContextPtr const&> emptyPackage{this, &PackagerTests::resultEmptyPackage};
    RouteSink<PackagerTests, EventsUploadContextPtr const&> packagedEvents{this, &PackagerTests::resultPackagedEvents};

  protected:
    PackagerTests()
      : packager(logConfiguration, runtimeConfigMock)
    {
        packager.emptyPackage   >> emptyPackage;
        packager.packagedEvents >> packagedEvents;
    }

    MOCK_METHOD1(resultEmptyPackage,   void(EventsUploadContextPtr const &));
    MOCK_METHOD1(resultPackagedEvents, void(EventsUploadContextPtr const &));
};


TEST_F(PackagerTests, EmptyInputResultsInEmptyPackage)
{
    auto ctx = EventsUploadContext::create();
    EXPECT_CALL(*this, resultEmptyPackage(ctx))
        .WillOnce(Return());
    packager.finalizePackage(ctx);
}

TEST_F(PackagerTests, PackagesEventsByTenant)
{
    auto ctx = EventsUploadContext::create();
    EXPECT_CALL(runtimeConfigMock, GetMaximumUploadSizeBytes())
        .WillOnce(Return(100000))
        .RetiresOnSaturation();

    StorageRecord record1("r1", "tenant1-token", EventPriority_Normal, 1234567890, std::vector<uint8_t>{1, 1, 1, 0});
    bool wantMore = true;
    packager.addEventToPackage(ctx, record1, wantMore);

    EXPECT_CALL(*this, resultPackagedEvents(ctx))
        .WillOnce(Return());
    packager.finalizePackage(ctx);

    EXPECT_THAT(ctx->body, Not(IsEmpty()));
    EXPECT_THAT(ctx->recordIds, SizeIs(1));
    EXPECT_THAT(ctx->recordIds, Contains("r1"));
    EXPECT_THAT(ctx->packageIds, SizeIs(1));
    EXPECT_THAT(ctx->packageIds, Contains(Key("tenant1-token")));


    ctx = EventsUploadContext::create();
    EXPECT_CALL(runtimeConfigMock, GetMaximumUploadSizeBytes())
        .WillOnce(Return(100000))
        .RetiresOnSaturation();

    wantMore = true;
    packager.addEventToPackage(ctx, record1, wantMore);
    StorageRecord record2("r2", "tenant2-token", EventPriority_Normal, 1234567891, std::vector<uint8_t>{2, 2, 2, 0});
    packager.addEventToPackage(ctx, record2, wantMore);

    EXPECT_CALL(*this, resultPackagedEvents(ctx))
        .WillOnce(Return());
    packager.finalizePackage(ctx);

    EXPECT_THAT(ctx->body, Not(IsEmpty()));
    EXPECT_THAT(ctx->recordIds, SizeIs(2));
    EXPECT_THAT(ctx->recordIds, Contains("r1"));
    EXPECT_THAT(ctx->recordIds, Contains("r2"));
    EXPECT_THAT(ctx->packageIds, SizeIs(2));
    EXPECT_THAT(ctx->packageIds, Contains(Key("tenant1-token")));
    EXPECT_THAT(ctx->packageIds, Contains(Key("tenant2-token")));
}

TEST_F(PackagerTests, UsesPriorityOfTheFirstEvent)
{
    auto ctx = EventsUploadContext::create();
    EXPECT_CALL(runtimeConfigMock, GetMaximumUploadSizeBytes())
        .WillOnce(Return(100000))
        .RetiresOnSaturation();
    EXPECT_THAT(ctx->priority, EventPriority_Unspecified);

    StorageRecord record("r1", "tenant1-token", EventPriority_Normal, 1234567890, std::vector<uint8_t>{1, 1, 1, 0});
    bool wantMore = false;
    packager.addEventToPackage(ctx, record, wantMore);
    EXPECT_THAT(ctx->priority, EventPriority_Normal);
}

TEST_F(PackagerTests, HonorsMaximumPackageSize)
{
    unsigned const MaxSize  = 100000;
    unsigned const PartSize = (MaxSize - 200) / 3;

    auto ctx = EventsUploadContext::create();
    EXPECT_CALL(runtimeConfigMock, GetMaximumUploadSizeBytes())
        .WillOnce(Return(MaxSize))
        .RetiresOnSaturation();

    bool wantMore = true;
    int i = 0;
    while (i < 4 && wantMore) {
        StorageRecord record("r" + toString(i), "tenant1-token", EventPriority_Normal, 1234567890 + i, std::vector<uint8_t>(PartSize, 0));
        packager.addEventToPackage(ctx, record, wantMore);
        i++;
    }
    EXPECT_THAT(i, 4);
    EXPECT_THAT(wantMore, false);

    EXPECT_CALL(*this, resultPackagedEvents(ctx))
        .WillOnce(Return());
    packager.finalizePackage(ctx);

    EXPECT_THAT(ctx->body, SizeIs(Gt(PartSize * 3)));
    EXPECT_THAT(ctx->body, SizeIs(Lt(MaxSize)));
}

TEST_F(PackagerTests, PackagesAtLeastOneEventEvenIfOverSizeLimit)
{
    unsigned const MaxSize = 10000;

    auto ctx = EventsUploadContext::create();
    EXPECT_CALL(runtimeConfigMock, GetMaximumUploadSizeBytes())
        .WillOnce(Return(MaxSize))
        .RetiresOnSaturation();

    bool wantMore = true;
    StorageRecord record("r", "tenant1-token", EventPriority_Normal, 1234567890, std::vector<uint8_t>(MaxSize, 0));
    packager.addEventToPackage(ctx, record, wantMore);
    EXPECT_THAT(wantMore, false);

    EXPECT_CALL(*this, resultPackagedEvents(ctx))
        .WillOnce(Return());
    packager.finalizePackage(ctx);

    EXPECT_THAT(ctx->body, SizeIs(Gt(MaxSize)));
}

TEST_F(PackagerTests, SetsRequestBondFieldsCorrectly)
{
    auto ctx = EventsUploadContext::create();
    EXPECT_CALL(runtimeConfigMock, GetMaximumUploadSizeBytes())
        .WillOnce(Return(100000))
        .RetiresOnSaturation();

    bool wantMore = true;
    StorageRecord record1("r1", "tenant1-token", EventPriority_Normal, 1234567890, std::vector<uint8_t>{0});
    packager.addEventToPackage(ctx, record1, wantMore);
    StorageRecord record2("r2", "tenant2-token", EventPriority_Normal, 1234567891, std::vector<uint8_t>{0});
    packager.addEventToPackage(ctx, record2, wantMore);
    StorageRecord record3("r3", "tenant1-token", EventPriority_Normal, 1234567892, std::vector<uint8_t>{0});
    packager.addEventToPackage(ctx, record1, wantMore);

    EXPECT_CALL(*this, resultPackagedEvents(ctx))
        .WillOnce(Return());
    packager.finalizePackage(ctx);

    AriaProtocol::ClientToCollectorRequest r;
    bond_lite::CompactBinaryProtocolReader reader(ctx->body);
    ASSERT_THAT(bond_lite::Deserialize(reader, r), true);

    EXPECT_THAT(r.DataPackages, IsEmpty());

    ASSERT_THAT(r.TokenToDataPackagesMap, Contains(Key("tenant1-token")));
    ASSERT_THAT(r.TokenToDataPackagesMap["tenant1-token"], SizeIs(1));
    ASSERT_THAT(r.TokenToDataPackagesMap["tenant1-token"][0].Records, SizeIs(2));

    ASSERT_THAT(r.TokenToDataPackagesMap, Contains(Key("tenant2-token")));
    ASSERT_THAT(r.TokenToDataPackagesMap["tenant2-token"], SizeIs(1));
    ASSERT_THAT(r.TokenToDataPackagesMap["tenant2-token"][0].Records, SizeIs(1));

    auto const& dp = r.TokenToDataPackagesMap["tenant1-token"][0];
    EXPECT_THAT(dp.Type,          Eq("Client"));
    EXPECT_THAT(dp.Source,        Eq("act_default_source"));
    EXPECT_THAT(dp.Version,       Eq(VersionString));
    EXPECT_THAT(dp.DataPackageId, SizeIs(36));
    EXPECT_THAT(dp.Timestamp,     Near(PAL::getUtcSystemTimeMs(), 1000));
}

TEST_F(PackagerTests, ForcedTenantIsForced)
{
    LogConfiguration logConfigurationF;
    logConfigurationF.SetProperty("forcedTenantToken", "forced-Tenant-Token");
    Packager packagerF(logConfigurationF, runtimeConfigMock);
    packagerF.packagedEvents >> packagedEvents;

    auto ctx = EventsUploadContext::create();
    EXPECT_CALL(runtimeConfigMock, GetMaximumUploadSizeBytes())
        .WillOnce(Return(100000))
        .RetiresOnSaturation();

    bool wantMore = true;
    StorageRecord record1("r1", "tenant1-token", EventPriority_Normal, 1234567890, std::vector<uint8_t>{0});
    packagerF.addEventToPackage(ctx, record1, wantMore);
    StorageRecord record2("r2", "tenant2-token", EventPriority_Normal, 1234567891, std::vector<uint8_t>{0});
    packagerF.addEventToPackage(ctx, record2, wantMore);
    StorageRecord record3("r3", "tenant1-token", EventPriority_Normal, 1234567892, std::vector<uint8_t>{0});
    packagerF.addEventToPackage(ctx, record1, wantMore);

    EXPECT_CALL(*this, resultPackagedEvents(ctx))
        .WillOnce(Return());
    packagerF.finalizePackage(ctx);

    EXPECT_THAT(ctx->packageIds, SizeIs(1));
    EXPECT_THAT(ctx->packageIds, Contains(Key("forced-tenant-token")));

    AriaProtocol::ClientToCollectorRequest r;
    bond_lite::CompactBinaryProtocolReader reader(ctx->body);
    ASSERT_THAT(bond_lite::Deserialize(reader, r), true);

    EXPECT_THAT(r.TokenToDataPackagesMap, SizeIs(1));
    ASSERT_THAT(r.TokenToDataPackagesMap, Contains(Key("forced-tenant-token")));
    ASSERT_THAT(r.TokenToDataPackagesMap["forced-tenant-token"], SizeIs(1));
    ASSERT_THAT(r.TokenToDataPackagesMap["forced-tenant-token"][0].Records, SizeIs(3));
}
