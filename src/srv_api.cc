
#include "srv_api.h"


using namespace Napi;

SrvApi::SrvApi(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to name yourself")
          .ThrowAsJavaScriptException();
        return;
    }

    this->_greeterName = info[0].As<Napi::String>().Utf8Value();

    _st_srv = redisConnect("127.0.0.1", 6379);
    if (_st_srv == NULL || _st_srv->err) {

        char buff[800];
        if (_st_srv) {
            sprintf(buff, "Connection error: %s\n", _st_srv->errstr);
            redisFree(_st_srv);
        } else {
            sprintf(buff, "Connection error: can't allocate redis context\n");
        }
        Napi::TypeError::New(env, buff)
          .ThrowAsJavaScriptException();
    }
}

Napi::Value SrvApi::Greet(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to introduce yourself to greet")
          .ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String name = info[0].As<Napi::String>();

    printf("Hello %s\n", name.Utf8Value().c_str());
    printf("I am %s\n", this->_greeterName.c_str());

    return Napi::String::New(env, this->_greeterName);
}

Napi::Function SrvApi::GetClass(Napi::Env env) {
    return DefineClass(env, "SrvApi", {
        SrvApi::InstanceMethod("greet", &SrvApi::Greet),
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "SrvApi");
    exports.Set(name, SrvApi::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
