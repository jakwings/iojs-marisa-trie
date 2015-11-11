#ifndef KEY_WRAP_HPP
#define KEY_WRAP_HPP


#include "binding.hpp"
#include "agent-wrap.hpp"
#include "keyset-wrap.hpp"


class KeyWrap : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

    marisa::Key& key() { return key_; }

  private:
    explicit KeyWrap() {}
    explicit KeyWrap(const marisa::Key& key) : key_(key) {}
    ~KeyWrap() {}

    static NAN_METHOD(New);
    static NAN_INDEX_GETTER(GetChar);
    static NAN_METHOD(Ptr);
    static NAN_METHOD(Length);
    static NAN_METHOD(Id);
    static NAN_METHOD(Weight);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Swap);

    void SetStr(const char*);
    void SetStr(const char*, const size_t);
    void SetId(const size_t);
    void SetWeight(const float);

    static Nan::Persistent<v8::FunctionTemplate> constructor;
    marisa::Key key_;

    friend class AgentWrap;
    friend class KeysetWrap;
};


#endif  // KEY_WRAP_HPP
