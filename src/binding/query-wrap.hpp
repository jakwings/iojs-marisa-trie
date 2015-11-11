#ifndef QUERY_WRAP_HPP
#define QUERY_WRAP_HPP


#include "binding.hpp"
#include "agent-wrap.hpp"


class QueryWrap : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

    marisa::Query& query() { return query_; }

  private:
    explicit QueryWrap() {}
    explicit QueryWrap(const marisa::Query& query) : query_(query) {}
    ~QueryWrap() {}

    static NAN_METHOD(New);
    static NAN_INDEX_GETTER(GetChar);
    static NAN_METHOD(Ptr);
    static NAN_METHOD(Length);
    static NAN_METHOD(Id);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Swap);

    void SetStr(const char*);
    void SetStr(const char*, const size_t);
    void SetId(const size_t);

    static Nan::Persistent<v8::FunctionTemplate> constructor;
    marisa::Query query_;

    friend class AgentWrap;
};


#endif  // QUERY_WRAP_HPP
