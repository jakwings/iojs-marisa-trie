#ifndef KEYSET_WRAP_HPP
#define KEYSET_WRAP_HPP


#include "binding.hpp"


class KeysetWrap : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

    marisa::Keyset& keyset() { return keyset_; }

  private:
    explicit KeysetWrap() {}
    ~KeysetWrap() {}

    static NAN_METHOD(New);
    static NAN_INDEX_GETTER(GetKey);
    static NAN_METHOD(PushBack);
    static NAN_METHOD(NumKeys);
    static NAN_METHOD(Empty);
    static NAN_METHOD(Size);
    static NAN_METHOD(TotalLength);
    static NAN_METHOD(Reset);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Swap);

    static Nan::Persistent<v8::FunctionTemplate> constructor;
    marisa::Keyset keyset_;

    friend class TrieWrap;
};


#endif  // KEYSET_WRAP_HPP
