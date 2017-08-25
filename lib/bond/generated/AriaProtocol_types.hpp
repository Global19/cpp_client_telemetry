//------------------------------------------------------------------------------
// This code was generated by a tool.
//
//   Tool : bondjson2cpp 2016.12.18.1
//   File : AriaProtocol.json
//
// Changes to this file may cause incorrect behavior and will be lost when
// the code is regenerated.
// <auto-generated />
//------------------------------------------------------------------------------

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace AriaProtocol {

struct Ingest {
    // 1: required int64 time
    int64_t time = 0;
    // 2: required string clientIp
    std::string clientIp;
    // 3: optional int64 auth
    int64_t auth = 0;
    // 4: optional int64 quality
    int64_t quality = 0;
    // 5: optional int64 uploadTime
    int64_t uploadTime = 0;
    // 6: optional string userAgent
    std::string userAgent;
    // 7: optional string client
    std::string client;

    bool operator==(Ingest const& other) const
    {
        return (time == other.time)
            && (clientIp == other.clientIp)
            && (auth == other.auth)
            && (quality == other.quality)
            && (uploadTime == other.uploadTime)
            && (userAgent == other.userAgent)
            && (client == other.client);
    }

    bool operator!=(Ingest const& other) const
    {
        return !(*this == other);
    }
};

struct User {
    // 1: optional string id
    std::string id;
    // 2: optional string localId
    std::string localId;
    // 3: optional string authId
    std::string authId;

    bool operator==(User const& other) const
    {
        return (id == other.id)
            && (localId == other.localId)
            && (authId == other.authId);
    }

    bool operator!=(User const& other) const
    {
        return !(*this == other);
    }
};

struct Device {
    // 1: optional string id
    std::string id;
    // 2: optional string localId
    std::string localId;
    // 3: optional string authId
    std::string authId;
    // 4: optional string authSecId
    std::string authSecId;
    // 5: optional string deviceClass
    std::string deviceClass;
    // 6: optional string orgId
    std::string orgId;
    // 7: optional string orgAuthId
    std::string orgAuthId;

    bool operator==(Device const& other) const
    {
        return (id == other.id)
            && (localId == other.localId)
            && (authId == other.authId)
            && (authSecId == other.authSecId)
            && (deviceClass == other.deviceClass)
            && (orgId == other.orgId)
            && (orgAuthId == other.orgAuthId);
    }

    bool operator!=(Device const& other) const
    {
        return !(*this == other);
    }
};

struct Os {
    // 1: optional string locale
    std::string locale;
    // 2: optional string expId
    std::string expId;
    // 3: optional int32 bootId
    int32_t bootId = 0;

    bool operator==(Os const& other) const
    {
        return (locale == other.locale)
            && (expId == other.expId)
            && (bootId == other.bootId);
    }

    bool operator!=(Os const& other) const
    {
        return !(*this == other);
    }
};

struct App {
    // 1: optional string expId
    std::string expId;
    // 2: optional string userId
    std::string userId;
    // 3: optional string env
    std::string env;
    // 4: optional int32 asId
    int32_t asId = 0;

    bool operator==(App const& other) const
    {
        return (expId == other.expId)
            && (userId == other.userId)
            && (env == other.env)
            && (asId == other.asId);
    }

    bool operator!=(App const& other) const
    {
        return !(*this == other);
    }
};

struct Utc {
    // 1: optional string stId
    std::string stId;
    // 2: optional string aId
    std::string aId;
    // 3: optional string raId
    std::string raId;
    // 4: optional string op
    std::string op;
    // 5: optional int64 cat
    int64_t cat = 0;
    // 6: optional int64 flags
    int64_t flags = 0;
    // 7: optional string sqmId
    std::string sqmId;
    // 8: optional vector<string> tickets
    std::vector<std::string> tickets;
    // 9: optional string mon
    std::string mon;
    // 10: optional int32 cpId
    int32_t cpId = 0;
    // 11: optional string bSeq
    std::string bSeq;

    bool operator==(Utc const& other) const
    {
        return (stId == other.stId)
            && (aId == other.aId)
            && (raId == other.raId)
            && (op == other.op)
            && (cat == other.cat)
            && (flags == other.flags)
            && (sqmId == other.sqmId)
            && (tickets == other.tickets)
            && (mon == other.mon)
            && (cpId == other.cpId)
            && (bSeq == other.bSeq);
    }

    bool operator!=(Utc const& other) const
    {
        return !(*this == other);
    }
};

struct Protocol {
    // 1: optional int32 metadataCrc
    int32_t metadataCrc = 0;
    // 2: optional string ticketKey
    std::string ticketKey;

    bool operator==(Protocol const& other) const
    {
        return (metadataCrc == other.metadataCrc)
            && (ticketKey == other.ticketKey);
    }

    bool operator!=(Protocol const& other) const
    {
        return !(*this == other);
    }
};

namespace _bond_enumerators {
namespace ValueKind {
enum ValueKind {
    ValueInt64         = 0,
    ValueUInt64        = 1,
    ValueInt32         = 2,
    ValueUInt32        = 3,
    ValueDouble        = 4,
    ValueString        = 5,
    ValueBool          = 6,
    ValueDateTime      = 7,
    ValueGuid          = 8,
    ValueArrayInt64    = 9,
    ValueArrayUInt64   = 10,
    ValueArrayInt32    = 11,
    ValueArrayUInt32   = 12,
    ValueArrayDouble   = 13,
    ValueArrayString   = 14,
    ValueArrayBool     = 15,
    ValueArrayDateTime = 16,
    ValueArrayGuid     = 17
};
}
}
using namespace _bond_enumerators::ValueKind;

namespace _bond_enumerators {
namespace PIIKind {
enum PIIKind {
    NotSet            = 0,
    DistinguishedName = 1,
    GenericData       = 2,
    IPV4Address       = 3,
    IPv6Address       = 4,
    MailSubject       = 5,
    PhoneNumber       = 6,
    QueryString       = 7,
    SipAddress        = 8,
    SmtpAddress       = 9,
    Identity          = 10,
    Uri               = 11,
    Fqdn              = 12,
    IPV4AddressLegacy = 13
};
}
}
using namespace _bond_enumerators::PIIKind;

struct PII {
    // 1: optional PIIKind Kind
    ::AriaProtocol::PIIKind Kind = ::AriaProtocol::PIIKind::NotSet;

    bool operator==(PII const& other) const
    {
        return (Kind == other.Kind);
    }

    bool operator!=(PII const& other) const
    {
        return !(*this == other);
    }
};

namespace _bond_enumerators {
namespace CustomerContentKind {
enum CustomerContentKind {
    NotSet         = 0,
    GenericContent = 1
};
}
}
using namespace _bond_enumerators::CustomerContentKind;

struct CustomerContent {
    // 1: optional CustomerContentKind Kind
    ::AriaProtocol::CustomerContentKind Kind = ::AriaProtocol::CustomerContentKind::NotSet;

    bool operator==(CustomerContent const& other) const
    {
        return (Kind == other.Kind);
    }

    bool operator!=(CustomerContent const& other) const
    {
        return !(*this == other);
    }
};

struct Attributes {
    // 1: optional vector<PII> pii
    std::vector< ::AriaProtocol::PII> pii;
    // 2: optional vector<CustomerContent> customerContent
    std::vector< ::AriaProtocol::CustomerContent> customerContent;

    bool operator==(Attributes const& other) const
    {
        return (pii == other.pii)
            && (customerContent == other.customerContent);
    }

    bool operator!=(Attributes const& other) const
    {
        return !(*this == other);
    }
};

struct Value {
    // 1: optional ValueKind type
    ::AriaProtocol::ValueKind type = ::AriaProtocol::ValueKind::ValueString;
    // 2: optional vector<Attributes> attributes
    std::vector< ::AriaProtocol::Attributes> attributes;
    // 3: optional string stringValue
    std::string stringValue;
    // 4: optional int64 longValue
    int64_t longValue = 0;
    // 5: optional double doubleValue
    double doubleValue = 0.0;
    // 6: optional vector<vector<uint8>> guidValue
    std::vector<std::vector<uint8_t>> guidValue;
    // 10: optional vector<vector<string>> stringArray
    std::vector<std::vector<std::string>> stringArray;
    // 11: optional vector<vector<int64>> longArray
    std::vector<std::vector<int64_t>> longArray;
    // 12: optional vector<vector<double>> doubleArray
    std::vector<std::vector<double>> doubleArray;
    // 13: optional vector<vector<vector<int64>>> guidArray
    std::vector<std::vector<std::vector<int64_t>>> guidArray;

    bool operator==(Value const& other) const
    {
        return (type == other.type)
            && (attributes == other.attributes)
            && (stringValue == other.stringValue)
            && (longValue == other.longValue)
            && (doubleValue == other.doubleValue)
            && (guidValue == other.guidValue)
            && (stringArray == other.stringArray)
            && (longArray == other.longArray)
            && (doubleArray == other.doubleArray)
            && (guidArray == other.guidArray);
    }

    bool operator!=(Value const& other) const
    {
        return !(*this == other);
    }
};

struct Data {
    // 1: optional map<string, Value> properties
    std::map<std::string, ::AriaProtocol::Value> properties;

    bool operator==(Data const& other) const
    {
        return (properties == other.properties);
    }

    bool operator!=(Data const& other) const
    {
        return !(*this == other);
    }
};

struct CsEvent {
    // 1: required string ver
    std::string ver;
    // 2: required string name
    std::string name;
    // 3: required int64 time
    int64_t time = 0;
    // 4: optional double popSample
    double popSample = 0.0;
    // 5: optional string epoch
    std::string epoch;
    // 6: optional int64 seqNum
    int64_t seqNum = 0;
    // 7: optional string iKey
    std::string iKey;
    // 8: optional int64 flags
    int64_t flags = 0;
    // 9: optional string os
    std::string os;
    // 10: optional string osVer
    std::string osVer;
    // 11: optional string appId
    std::string appId;
    // 12: optional string appVer
    std::string appVer;
    // 13: optional string cV
    std::string cV;
    // 14: optional vector<Ingest> extIngest
    std::vector< ::AriaProtocol::Ingest> extIngest;
    // 15: optional vector<User> extUser
    std::vector< ::AriaProtocol::User> extUser;
    // 16: optional vector<Device> extDevice
    std::vector< ::AriaProtocol::Device> extDevice;
    // 17: optional vector<Os> extOs
    std::vector< ::AriaProtocol::Os> extOs;
    // 18: optional vector<App> expApp
    std::vector< ::AriaProtocol::App> expApp;
    // 19: optional vector<Utc> extUtc
    std::vector< ::AriaProtocol::Utc> extUtc;
    // 20: optional vector<Protocol> extProtocol
    std::vector< ::AriaProtocol::Protocol> extProtocol;
    // 21: optional vector<Data> ext
    std::vector< ::AriaProtocol::Data> ext;
    // 22: optional map<string, string> tags
    std::map<std::string, std::string> tags;
    // 50: optional string baseType
    std::string baseType;
    // 51: optional vector<Data> baseData
    std::vector< ::AriaProtocol::Data> baseData;
    // 60: optional vector<Data> data
    std::vector< ::AriaProtocol::Data> data;

    bool operator==(CsEvent const& other) const
    {
        return (ver == other.ver)
            && (name == other.name)
            && (time == other.time)
            && (popSample == other.popSample)
            && (epoch == other.epoch)
            && (seqNum == other.seqNum)
            && (iKey == other.iKey)
            && (flags == other.flags)
            && (os == other.os)
            && (osVer == other.osVer)
            && (appId == other.appId)
            && (appVer == other.appVer)
            && (cV == other.cV)
            && (extIngest == other.extIngest)
            && (extUser == other.extUser)
            && (extDevice == other.extDevice)
            && (extOs == other.extOs)
            && (expApp == other.expApp)
            && (extUtc == other.extUtc)
            && (extProtocol == other.extProtocol)
            && (ext == other.ext)
            && (tags == other.tags)
            && (baseType == other.baseType)
            && (baseData == other.baseData)
            && (data == other.data);
    }

    bool operator!=(CsEvent const& other) const
    {
        return !(*this == other);
    }
};

} // namespace AriaProtocol
