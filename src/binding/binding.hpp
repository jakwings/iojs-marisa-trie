#ifndef BINDING_HPP
#define BINDING_HPP


#include <marisa/base.h>
#include <marisa/exception.h>
#include <marisa/trie.h>
#include <nan.h>

#define NAN_UNWRAP(T, val) \
  Nan::ObjectWrap::Unwrap<T>(Nan::To<v8::Object>(val).ToLocalChecked())

namespace Nan {

template<typename T>
T* Unwrap(v8::Local<v8::FunctionTemplate> tpl, v8::Local<v8::Value> val) {
  Nan::HandleScope scope;
  if (tpl->HasInstance(val)) {
    v8::Local<v8::Object> object = Nan::To<v8::Object>(val).ToLocalChecked();
    return Nan::ObjectWrap::Unwrap<T>(object);
  }
  return NULL;
}

}  // namespace Nan


#endif  // BINDING_HPP
