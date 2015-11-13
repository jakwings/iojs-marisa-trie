#include "keyset-wrap.hpp"
#include "key-wrap.hpp"


Nan::Persistent<v8::FunctionTemplate> KeysetWrap::constructor;

NAN_MODULE_INIT(KeysetWrap::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(KeysetWrap::New);
  tpl->SetClassName(Nan::New("Keyset").ToLocalChecked());
  // Field#0 must be used by ObjectWrap::Wrap to export the instance of KeysetWrap.
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetIndexedPropertyHandler(tpl->InstanceTemplate(), KeysetWrap::GetKey);

  Nan::SetPrototypeMethod(tpl, "push_back", KeysetWrap::PushBack);
  Nan::SetPrototypeMethod(tpl, "num_keys", KeysetWrap::NumKeys);
  Nan::SetPrototypeMethod(tpl, "empty", KeysetWrap::Empty);
  Nan::SetPrototypeMethod(tpl, "size", KeysetWrap::Size);
  Nan::SetPrototypeMethod(tpl, "total_length", KeysetWrap::TotalLength);
  Nan::SetPrototypeMethod(tpl, "reset", KeysetWrap::Reset);
  Nan::SetPrototypeMethod(tpl, "clear", KeysetWrap::Clear);
  Nan::SetPrototypeMethod(tpl, "swap", KeysetWrap::Swap);

  constructor.Reset(tpl);
  Nan::Set(target, Nan::New("Keyset").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(KeysetWrap::New) {
  if (info.IsConstructCall()) {
    KeysetWrap* self = new KeysetWrap();
    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {  // Turn it into a construct call.
    v8::Local<v8::Function> cons = Nan::GetFunction(Nan::New(constructor)).ToLocalChecked();
    info.GetReturnValue().Set(Nan::NewInstance(cons).ToLocalChecked());
  }
}

NAN_INDEX_GETTER(KeysetWrap::GetKey) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  if (index < self->keyset_.size()) {
    v8::Local<v8::Function> cons = Nan::GetFunction(Nan::New(KeyWrap::constructor)).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons).ToLocalChecked();
    KeyWrap* wrap = NAN_UNWRAP(KeyWrap, instance);
    const marisa::Key& key = self->keyset_[static_cast<size_t>(index)];
    wrap->SetStr(key.ptr(), key.length());
    wrap->SetWeight(key.weight());
    wrap->SetId(key.id());
    info.GetReturnValue().Set(instance);
  } else {
    info.GetReturnValue().SetUndefined();
  }
}

NAN_METHOD(KeysetWrap::PushBack) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  if (self->keyset_.size() == MARISA_SIZE_MAX ||
      self->keyset_.size() == 9007199254740990LL) {  // Number.MAX_SAFE_INTEGER - 1
    Nan::ThrowRangeError("Maximum size of keyset exceeded.");
    return;
  }
  if (info[0]->IsString()) {
    Nan::Utf8String utf8_key(info[0]);
    if (info[1]->IsNumber()) {
      int64_t length = Nan::To<int64_t>(info[1]).FromJust();
      double weight = 1.0;
      if (info[2]->IsNumber()) {
        weight = Nan::To<double>(info[2]).FromJust();
      } else if (!(info[2]->IsUndefined() || info[2]->IsNull())) {
        Nan::ThrowTypeError("Argument#2 must be a number if provided with the string.");
        return;
      }
      if (length < 0 || length > utf8_key.length()) {
        length = utf8_key.length();
      }
      self->keyset_.push_back(*utf8_key, static_cast<size_t>(length), static_cast<float>(weight));
    } else {
      if (!(info[1]->IsUndefined() || info[1]->IsNull()) ||
          !(info[2]->IsUndefined() || info[2]->IsNull())) {
        Nan::ThrowTypeError("Argument#2 and/or arguments#3 must be numbers if provided with the string.");
        return;
      }
      self->keyset_.push_back(*utf8_key);
    }
    info.GetReturnValue().SetUndefined();
  } else if (info[0]->IsObject()) {
    KeyWrap* key = NAN_UNWRAP2(KeyWrap, info[0]);
    if (NULL != key) {
      self->keyset_.push_back(key->key());
      info.GetReturnValue().SetUndefined();
    } else {
      Nan::ThrowTypeError("Argument#1 must be a string or a key.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be a string or a key.");
  }
}

NAN_METHOD(KeysetWrap::NumKeys) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->keyset_.num_keys()));
}

NAN_METHOD(KeysetWrap::Empty) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  info.GetReturnValue().Set(self->keyset_.empty());
}

NAN_METHOD(KeysetWrap::Size) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->keyset_.size()));
}

NAN_METHOD(KeysetWrap::TotalLength) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->keyset_.total_length()));
}

NAN_METHOD(KeysetWrap::Reset) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  self->keyset_.reset();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(KeysetWrap::Clear) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  self->keyset_.clear();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(KeysetWrap::Swap) {
  KeysetWrap* self = NAN_UNWRAP(KeysetWrap, info.This());
  KeysetWrap* wrap = NAN_UNWRAP2(KeysetWrap, info[0]);
  if (NULL != wrap) {
    self->keyset_.swap(wrap->keyset_);
    info.GetReturnValue().SetUndefined();
  } else {
    Nan::ThrowTypeError("Argument#1 must be a keyset.");
  }
}
