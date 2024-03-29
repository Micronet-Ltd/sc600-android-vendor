/*********************************************************************
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/
#ifndef CREATABLE_H
#define CREATABLE_H
#include <android/hidl/base/1.0/IBase.h>

#define TOSTRING(token) std::string(#token)
/* The name must match with a create api from the factory */
#define INIT_CREATABLE(name) Creatable(TOSTRING(create ## name))

using ::android::hidl::base::V1_0::IBase;
using android::hardware::Return;
using android::sp;

namespace vendor {
namespace qti {
namespace data {
namespace factory {

class Params {
public:
    Params() {}
    Params& setCallback(sp<IBase>);
    Params& setSlotId(uint32_t);

    sp<IBase> getCallback() const { return callback; }
    uint32_t getSlotId() const { return slotId; }
private:
    sp<IBase> callback;
    uint32_t slotId;
};

class Creatable {
public:
    Creatable(const std::string& setApiName);
    virtual ~Creatable();
    virtual sp<IBase> createInstance(const Params& params) = 0;

private:
    std::string apiName;
    bool isRegistered;

    Creatable() = delete;
    Creatable(const Creatable&) = delete;
    Creatable& operator=(const Creatable&) = delete;
};

inline Params& Params::setCallback(sp<IBase> setCallback) {
    callback = setCallback;
    return *this;
}

inline Params& Params::setSlotId(uint32_t setSlotId) {
    slotId = setSlotId;
    return *this;
}

} //factory
} //data
} //qti
} //vendor

#endif/* CREATOR_H */
