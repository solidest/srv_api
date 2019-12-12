
#include <string>
#include <iostream>
#include "json.hpp"
#include "srv_api.h"

using namespace Napi;
using namespace nlohmann;

SrvApi::SrvApi(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Need server ip and port").ThrowAsJavaScriptException();
        return;
    }

    this->_greeterName = info[0].As<Napi::String>().Utf8Value();

    std::string ip = info[0].As<Napi::String>().Utf8Value();

    _st_srv = redisConnect(ip.c_str(), info[1].As<Napi::Number>().Int32Value());
    if (_st_srv == NULL || _st_srv->err) {

        char buff[800];
        if (_st_srv) {
            sprintf(buff, "Connection server error: %s\n", _st_srv->errstr);
            redisFree(_st_srv);
        } else {
            sprintf(buff, "Connection server error: can't allocate server context\n");
        }
        Napi::TypeError::New(env, buff).ThrowAsJavaScriptException();
    }
}

SrvApi::~SrvApi() {
    if(_st_srv!=nullptr) {
        Finalize(nullptr);
    }
}


Napi::Value SrvApi::Close(const Napi::CallbackInfo& info) {
    redisFree(_st_srv);
    _st_srv = nullptr;
    return info.Env().Null();
}

void SrvApi::Finalize(Napi::Env env){
    if(_st_srv) {
        redisFree(_st_srv);
        _st_srv = nullptr;        
    }
}

//get state of server
Napi::Value SrvApi::State(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto reply = (redisReply*)redisCommand(_st_srv, "state");
    if(reply ==nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }
    if(reply->type ==REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }
    if(reply->elements<1) {
        Napi::TypeError::New(env, "error state()").ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    auto res = Napi::Object::New(env);
    res.Set("work_type", reply->element[0]->str);
    if(reply->elements>1) {
        res.Set("proj_id", reply->element[1]->str);
    }
    if(reply->elements>2) {
        res.Set("run_case", reply->element[2]->str);
    }
    if(reply->elements>3) {
        res.Set("dbid", reply->element[3]->str);
    }
    freeReplyObject(reply);
    return res;
}

//ping
Napi::Value SrvApi::Ping(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if(_st_srv==nullptr) {
        Napi::TypeError::New(env, "null").ThrowAsJavaScriptException();
    }

    auto reply = (redisReply*)redisCommand(_st_srv, "ping");
    if(reply ==nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }
    if(reply->type ==REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }
    if(reply->type!=REDIS_REPLY_STATUS) {
        char buff[800];
        sprintf(buff, "error%i ping()", reply->type);
        Napi::TypeError::New(env, buff).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    auto res = Napi::String::New(env, reply->str);
    freeReplyObject(reply);
    return res;
}

//prepare for run case
Napi::Value SrvApi::Prepare(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 4) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber() || !info[1].IsString() || !info[2].IsString() || !info[3].IsString()) {
        Napi::TypeError::New(env, "Need proj_id, interface_path and protocol_path").ThrowAsJavaScriptException();
        return env.Null();
    }


    char cmd[] = "prepare";
    char proj_id[16];
    sprintf(proj_id, "%i", info[0].As<Napi::Number>().Int32Value());
    std::string interface = info[1].As<Napi::String>().Utf8Value();
    std::string protocol = info[2].As<Napi::String>().Utf8Value();
    std::string xtra = info[3].As<Napi::String>().Utf8Value();

    const char* argv[] = {cmd, proj_id, interface.c_str(), protocol.c_str(), xtra.c_str() };
    size_t arglen[] = {strlen(cmd), strlen(proj_id), interface.size(), protocol.size(), xtra.size()};

    auto reply = (redisReply*)redisCommandArgv(_st_srv, 5, argv, arglen);
    if(reply == nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }

    if(reply->type==REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    if(reply->type!=REDIS_REPLY_STATUS || strcmp(reply->str, "OK")!=0) {
        printf("%s\n", reply->str);
        Napi::TypeError::New(env, "error prepare()").ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    freeReplyObject(reply);
    return env.Null();
}

//start run case
Napi::Value SrvApi::StartCase(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 3) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber() || !info[1].IsString() || !info[2].IsString()) {
        Napi::TypeError::New(env, "Need proj_id, case_file and db_id").ThrowAsJavaScriptException();
        return env.Null();
    }

    char cmd[] = "start";
    char proj_id[16];
    sprintf(proj_id, "%i", info[0].As<Napi::Number>().Int32Value());
    std::string casefile = info[1].As<Napi::String>().Utf8Value();
    std::string dbid = info[2].As<Napi::String>().Utf8Value();

    const char* argv[] = {cmd, proj_id, casefile.c_str(), dbid.c_str()};
    size_t arglen[] = {strlen(cmd), strlen(proj_id), casefile.size(), dbid.size()};

    auto reply = (redisReply*)redisCommandArgv(_st_srv, 4, argv, arglen);
    if(reply ==nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }
    if(reply->type==REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }
    if(reply->type!=REDIS_REPLY_STATUS || strcmp(reply->str, "OK")!=0) {
        Napi::TypeError::New(env, "error start()").ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    freeReplyObject(reply);
    return env.Null();
}

//stop run case
Napi::Value SrvApi::StopCase(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto reply = (redisReply*)redisCommand(_st_srv, "stop");
    if(reply ==nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }
    if(reply->type ==REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }
    if(reply->type!=REDIS_REPLY_STATUS || strcmp(reply->str, "OK")!=0) {
        Napi::TypeError::New(env, "error stop()").ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    freeReplyObject(reply);
    return env.Null();
}


Napi::Value SrvApi::Clear(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Need proj_id").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto reply = (redisReply*)redisCommand(_st_srv, "clear %i", info[0].As<Napi::Number>().Int32Value());
    if(reply ==nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }

    if(reply->type ==REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    if(reply->type!=REDIS_REPLY_STATUS || strcmp(reply->str, "OK")!=0) {
        Napi::TypeError::New(env, "error clear()").ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    freeReplyObject(reply);
    return env.Null();
}

Napi::Value SrvApi::Exit(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto reply = (redisReply*)redisCommand(_st_srv, "exit");
    if(reply ==nullptr) {
        return env.Null();
    }
    if(reply->type == REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    freeReplyObject(reply);
    return env.Null();
}

Napi::Value SrvApi::PauseInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto reply = (redisReply*)redisCommand(_st_srv, "puaseInfo");

    std::string s = "";
    if(reply && reply->type==REDIS_REPLY_STRING && reply->len>0) {
        s = reply->str;
        if(s=="nil") {
            s = "";
        }
    }
    
    if(reply) {
        freeReplyObject(reply);
    }

    return Napi::String::New(env, s.c_str());
}

Napi::Value SrvApi::Continue(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Need string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string s = info[0].As<Napi::String>().Utf8Value();

    const char* argv[] = {"continue", s.c_str()};
    size_t arglen[] = {strlen("continue"), s.size()};
    auto reply = (redisReply*)redisCommandArgv(_st_srv, 2, argv, arglen);

    if(reply) {
        freeReplyObject(reply);
    }    
    return env.Null();
}

Napi::Function SrvApi::GetClass(Napi::Env env) {
    return DefineClass(env, "SrvApi", {
        SrvApi::InstanceMethod("state", &SrvApi::State),
        SrvApi::InstanceMethod("close", &SrvApi::Close),
        SrvApi::InstanceMethod("ping", &SrvApi::Ping),
        SrvApi::InstanceMethod("prepare", &SrvApi::Prepare),
        SrvApi::InstanceMethod("start", &SrvApi::StartCase),
        SrvApi::InstanceMethod("stop", &SrvApi::StopCase),
        SrvApi::InstanceMethod("clear", &SrvApi::Clear),
        SrvApi::InstanceMethod("exit", &SrvApi::Exit),
        SrvApi::InstanceMethod("pauseInfo", &SrvApi::PauseInfo),
        SrvApi::InstanceMethod("continue", &SrvApi::Continue),
        SrvApi::InstanceMethod("outDigitial", &SrvApi::OutDigital),
        SrvApi::InstanceMethod("outAnalog", &SrvApi::OutAnalog),
        SrvApi::InstanceMethod("outData", &SrvApi::OutData),
        SrvApi::InstanceMethod("outProt", &SrvApi::OutProt),
        SrvApi::InstanceMethod("inDigital", &SrvApi::InDigital),
        SrvApi::InstanceMethod("inAnalog", &SrvApi::InAnalog),
        SrvApi::InstanceMethod("inData", &SrvApi::InData),
        SrvApi::InstanceMethod("inProt", &SrvApi::InProt),
        SrvApi::InstanceMethod("sysOut", &SrvApi::SysOut),
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "SrvApi");
    exports.Set(name, SrvApi::GetClass(env));
    return exports;
}

// ================================================== db command ================================================================
Napi::Value SrvApi::doIOCmd(const Napi::CallbackInfo& info, const char* cmd) {
    Napi::Env env = info.Env();
    if (info.Length() != 3) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber()) {
        Napi::TypeError::New(env, "Need interface, start_timestamp, limit").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string intf = info[0].As<Napi::String>().Utf8Value();
    std::string tm = std::to_string(info[0].As<Napi::Number>().Int64Value());
    std::string limit = std::to_string(info[0].As<Napi::Number>().Int32Value());

    const char* argv[] = {cmd, intf.c_str(), tm.c_str(), limit.c_str() };
    size_t arglen[] = {strlen(cmd), intf.size(), tm.size(), limit.size()};
    auto reply = (redisReply*)redisCommandArgv(_st_srv, 4, argv, arglen);
    if(reply ==nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }
    if(reply->type == REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }
    if(reply->type!= REDIS_REPLY_STRING) {
        char buff[800];
        sprintf(buff, "error%i %s()", reply->type, cmd);
        Napi::TypeError::New(env, buff).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    auto res = Napi::String::New(env, reply->str);
    freeReplyObject(reply);
    return res;
}


Napi::Value SrvApi::OutDigital(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "outDigitial");
}

Napi::Value SrvApi::OutAnalog(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "outAnalog");
}

Napi::Value SrvApi::OutData(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "outData");
}

Napi::Value SrvApi::InDigital(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "inDigital");    
}

Napi::Value SrvApi::InAnalog(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "inAnalog");    

}

Napi::Value SrvApi::InData(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "inData");    
}

Napi::Value SrvApi::SysOut(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Need start_timestamp, limit").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string tm = std::to_string(info[0].As<Napi::Number>().Int64Value());
    std::string limit = std::to_string(info[0].As<Napi::Number>().Int32Value());

    const char* argv[] = {"sysOut", tm.c_str(), limit.c_str() };
    size_t arglen[] = {strlen("sysOut"), tm.size(), limit.size()};
    auto reply = (redisReply*)redisCommandArgv(_st_srv, 3, argv, arglen);
    if(reply ==nullptr) {
        Napi::TypeError::New(env, _st_srv->errstr).ThrowAsJavaScriptException();
        return env.Null();
    }
    if(reply->type == REDIS_REPLY_ERROR) {
        Napi::TypeError::New(env, reply->str).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }
    if(reply->type!= REDIS_REPLY_STRING) {
        char buff[800];
        sprintf(buff, "error%i %s()", reply->type, "sysOut");
        Napi::TypeError::New(env, buff).ThrowAsJavaScriptException();
        freeReplyObject(reply);
        return env.Null();
    }

    auto res = Napi::String::New(env, reply->str);
    freeReplyObject(reply);
    return res;
}

Napi::Value SrvApi::OutProt(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "outProt");    
}

Napi::Value SrvApi::InProt(const Napi::CallbackInfo& info) {
    return doIOCmd(info, "inDigital");    
}


NODE_API_MODULE(addon, Init)
