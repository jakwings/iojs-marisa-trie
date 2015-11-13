#include "key-wrap.hpp"


Nan::Persistent<v8::FunctionTemplate> KeyWrap::constructor;

NAN_MODULE_INIT(KeyWrap::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(KeyWrap::New);
  tpl->SetClassName(Nan::New("Key").ToLocalChecked());
  // Field#0 must be used by ObjectWrap::Wrap to export the instance of KeyWrap.
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetIndexedPropertyHandler(tpl->InstanceTemplate(), KeyWrap::GetChar);

  Nan::SetPrototypeMethod(tpl, "ptr", KeyWrap::Ptr);
  Nan::SetPrototypeMethod(tpl, "length", KeyWrap::Length);
  Nan::SetPrototypeMethod(tpl, "id", KeyWrap::Id);
  Nan::SetPrototypeMethod(tpl, "weight", KeyWrap::Weight);
  Nan::SetPrototypeMethod(tpl, "clear", KeyWrap::Clear);
  Nan::SetPrototypeMethod(tpl, "swap", KeyWrap::Swap);

  constructor.Reset(tpl);
  Nan::Set(target, Nan::New("Key").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(KeyWrap::New) {
  if (info.IsConstructCall()) {
    KeyWrap* self = NULL;
    KeyWrap* wrap = NULL;
    if (info[0]->IsUndefined() || info[0]->IsNull()) {
      self = new KeyWrap();
    } else if (NULL != (wrap = NAN_UNWRAP2(KeyWrap, info[0]))) {
      self = new KeyWrap(wrap->key_);
    } else {
      Nan::ThrowTypeError("Argument#1 must be a key if provided.");
      return;
    }
    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {  // Turn it into a construct call.
    v8::Local<v8::Function> cons = Nan::GetFunction(Nan::New(constructor)).ToLocalChecked();
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
  }
}

NAN_INDEX_GETTER(KeyWrap::GetChar) {
  KeyWrap* self = NAN_UNWRAP(KeyWrap, info.This());
  if (index < self->key_.length()) {
    info.GetReturnValue().Set(static_cast<unsigned char>(self->key_[index]));
  } else {
    info.GetReturnValue().SetUndefined();
  }
}

NAN_METHOD(KeyWrap::Ptr) {
  KeyWrap* self = NAN_UNWRAP(KeyWrap, info.This());
  if (self->key_.length() > 0) {
    info.GetReturnValue().Set(Nan::New(self->key_.ptr(), self->key_.length()).ToLocalChecked());
  } else {
    info.GetReturnValue().SetEmptyString();
  }
}

NAN_METHOD(KeyWrap::Length) {
  KeyWrap* self = NAN_UNWRAP(KeyWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->key_.length()));
}

NAN_METHOD(KeyWrap::Id) {
  KeyWrap* self = NAN_UNWRAP(KeyWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->key_.id()));
}

NAN_METHOD(KeyWrap::Weight) {
  KeyWrap* self = NAN_UNWRAP(KeyWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->key_.weight()));
}

NAN_METHOD(KeyWrap::Clear) {
  KeyWrap* self = NAN_UNWRAP(KeyWrap, info.This());
  self->key_.clear();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(KeyWrap::Swap) {
  KeyWrap* self = NAN_UNWRAP(KeyWrap, info.This());
  KeyWrap* wrap = NAN_UNWRAP2(KeyWrap, info[0]);
  if (NULL != wrap) {
    self->key_.swap(wrap->key_);
    info.GetReturnValue().SetUndefined();
  } else {
    Nan::ThrowTypeError("Argument#1 must be a key.");
  }
}

void KeyWrap::SetStr(const char* ptr) {
  key_.set_str(ptr);
}

void KeyWrap::SetStr(const char* ptr, const size_t length) {
  key_.set_str(ptr, length);
}

void KeyWrap::SetId(const size_t id) {
  key_.set_id(id);
}

void KeyWrap::SetWeight(const float weight) {
  key_.set_weight(weight);
}
