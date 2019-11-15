#pragma once

#include <napi.h>

class SrvApi : public Napi::ObjectWrap<SrvApi>
{
public:
    SrvApi(const Napi::CallbackInfo&);
    Napi::Value Greet(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
    std::string _greeterName;
};
