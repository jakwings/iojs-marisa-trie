#include "agent-wrap.hpp"
#include "key-wrap.hpp"
#include "query-wrap.hpp"


Nan::Persistent<v8::FunctionTemplate> AgentWrap::constructor;

NAN_MODULE_INIT(AgentWrap::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(AgentWrap::New);
  tpl->SetClassName(Nan::New("Agent").ToLocalChecked());
  // Field#0 must be used by ObjectWrap::Wrap to export the instance of AgentWrap.
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "query", AgentWrap::Query);
  Nan::SetPrototypeMethod(tpl, "key", AgentWrap::Key);
  Nan::SetPrototypeMethod(tpl, "set_query", AgentWrap::SetQuery);
  Nan::SetPrototypeMethod(tpl, "clear", AgentWrap::Clear);
  Nan::SetPrototypeMethod(tpl, "swap", AgentWrap::Swap);

  constructor.Reset(tpl);
  Nan::Set(target, Nan::New("Agent").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(AgentWrap::New) {
  if (info.IsConstructCall()) {
    AgentWrap* self = new AgentWrap();
    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {  // Turn it into a construct call.
    v8::Local<v8::Function> cons = Nan::GetFunction(Nan::New(constructor)).ToLocalChecked();
    info.GetReturnValue().Set(Nan::NewInstance(cons).ToLocalChecked());
  }
}

NAN_METHOD(AgentWrap::Query) {
  AgentWrap* self = Nan::Unwrap<AgentWrap>(Nan::New(constructor), info.This());
  if (NULL != self) {
    v8::Local<v8::Function> cons = Nan::GetFunction(Nan::New(QueryWrap::constructor)).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons).ToLocalChecked();
    QueryWrap* wrap = NAN_UNWRAP(QueryWrap, instance);
    wrap->SetStr(self->agent_.query().ptr(), self->agent_.query().length());
    wrap->SetId(self->agent_.query().id());
    info.GetReturnValue().Set(instance);
  } else {
    Nan::ThrowError("Invalid invocation.");
  }
}

NAN_METHOD(AgentWrap::Key) {
  AgentWrap* self = Nan::Unwrap<AgentWrap>(Nan::New(constructor), info.This());
  if (NULL != self) {
    v8::Local<v8::Function> cons = Nan::GetFunction(Nan::New(KeyWrap::constructor)).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons).ToLocalChecked();
    KeyWrap* wrap = NAN_UNWRAP(KeyWrap, instance);
    wrap->SetStr(self->agent_.key().ptr(), self->agent_.key().length());
    wrap->SetWeight(self->agent_.key().weight());
    wrap->SetId(self->agent_.key().id());
    info.GetReturnValue().Set(instance);
  } else {
    Nan::ThrowError("Invalid invocation.");
  }
}

NAN_METHOD(AgentWrap::SetQuery) {
  AgentWrap* self = Nan::Unwrap<AgentWrap>(Nan::New(constructor), info.This());
  if (NULL != self) {
    if (info[0]->IsString()) {
      Nan::Utf8String utf8_key(info[0]);
      if (info[1]->IsNumber()) {
        int64_t length = Nan::To<int64_t>(info[1]).FromJust();
        if (length < 0 || length > utf8_key.length()) {
          length = utf8_key.length();
        }
        self->query_str_.assign(*utf8_key, static_cast<size_t>(length));
        self->agent_.set_query(self->query_str_.c_str());
      } else {
        if (!(info[1]->IsUndefined() || info[1]->IsNull())) {
          Nan::ThrowTypeError("Argument#2 must be an integer if provided with the string.");
          return;
        }
        self->query_str_.assign(*utf8_key, utf8_key.length());
        self->agent_.set_query(self->query_str_.c_str());
      }
      info.GetReturnValue().SetUndefined();
    } else if (info[0]->IsNumber()) {
      int64_t n = Nan::To<int64_t>(info[0]).FromJust();
      self->agent_.set_query(static_cast<size_t>(n));
      info.GetReturnValue().SetUndefined();
    } else {
      Nan::ThrowTypeError("Argument#1 must be a string or an integer.");
    }
  } else {
    Nan::ThrowError("Invalid invocation.");
  }
}

NAN_METHOD(AgentWrap::Clear) {
  AgentWrap* self = Nan::Unwrap<AgentWrap>(Nan::New(constructor), info.This());
  if (NULL != self) {
    self->agent_.clear();
    info.GetReturnValue().SetUndefined();
  } else {
    Nan::ThrowError("Invalid invocation.");
  }
}

NAN_METHOD(AgentWrap::Swap) {
  AgentWrap* self = Nan::Unwrap<AgentWrap>(Nan::New(constructor), info.This());
  if (NULL != self) {
    AgentWrap* wrap = NULL;
    if (NULL != (wrap = Nan::Unwrap<AgentWrap>(Nan::New(constructor), info[0]))) {
      self->agent_.swap(wrap->agent_);
      info.GetReturnValue().SetUndefined();
    } else {
      Nan::ThrowTypeError("Argument#1 must be a agent.");
    }
  } else {
    Nan::ThrowError("Invalid invocation.");
  }
}
