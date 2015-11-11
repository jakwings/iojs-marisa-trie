#ifndef AGENT_WRAP_HPP
#define AGENT_WRAP_HPP


#include "binding.hpp"
#include <string>


class AgentWrap : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

    marisa::Agent& agent() { return agent_; }

  private:
    explicit AgentWrap() {}
    ~AgentWrap() {}

    static NAN_METHOD(New);
    static NAN_METHOD(SetQuery);
    static NAN_METHOD(Query);
    static NAN_METHOD(Key);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Swap);

    static Nan::Persistent<v8::FunctionTemplate> constructor;
    marisa::Agent agent_;
    std::string query_str_;

    friend class TrieWrap;
};


#endif  // AGENT_WRAP_HPP
