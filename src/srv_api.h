
#ifndef _SRV_API_
#define _SRV_API_

#include <napi.h>
#include "hiredis/hiredis.h"

class SrvApi : public Napi::ObjectWrap<SrvApi>
{
public:
    SrvApi(const Napi::CallbackInfo&);
    virtual ~SrvApi();
    virtual void Finalize(Napi::Env env);
    Napi::Value Close(const Napi::CallbackInfo&);

    Napi::Value State(const Napi::CallbackInfo&);
    Napi::Value Ping(const Napi::CallbackInfo&);
    Napi::Value Prepare(const Napi::CallbackInfo&);
    Napi::Value StartCase(const Napi::CallbackInfo&);
    Napi::Value StopCase(const Napi::CallbackInfo&);
    Napi::Value PauseCase(const Napi::CallbackInfo&);
    Napi::Value ContinueCase(const Napi::CallbackInfo&);
    Napi::Value Clear(const Napi::CallbackInfo&);
    Napi::Value Exit(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
    std::string _greeterName;
    redisContext* _st_srv;
};


#endif