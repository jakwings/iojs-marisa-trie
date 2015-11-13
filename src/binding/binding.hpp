#ifndef BINDING_HPP
#define BINDING_HPP


#include <marisa/base.h>
#include <marisa/exception.h>
#include <marisa/trie.h>
#include <nan.h>

/*
 * value: v8::Local<v8::Value>
 * constructor: Nan::Persistent<v8::FunctionTemplate>
 */
#define NAN_UNWRAP(T, value) \
  Nan::ObjectWrap::Unwrap<T>(Nan::To<v8::Object>(value).ToLocalChecked())
#define NAN_UNWRAP2(T, value) \
  (Nan::New(T::constructor)->HasInstance(value) ? NAN_UNWRAP(T, value) : NULL)


#endif  // BINDING_HPP
