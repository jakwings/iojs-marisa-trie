#ifndef TRIE_WRAP_HPP
#define TRIE_WRAP_HPP


#include "binding.hpp"


class TrieWrap : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

    marisa::Trie& trie() { return trie_; }

  private:
    explicit TrieWrap() {}
    ~TrieWrap() {}

    static NAN_METHOD(New);
    static NAN_METHOD(Build);
    static NAN_METHOD(Mmap);
    static NAN_METHOD(Map);
    static NAN_METHOD(Load);
    static NAN_METHOD(Read);
    static NAN_METHOD(Save);
    static NAN_METHOD(Write);
    static NAN_METHOD(Lookup);
    static NAN_METHOD(ReverseLookup);
    static NAN_METHOD(CommonPrefixSearch);
    static NAN_METHOD(PredictiveSearch);
    static NAN_METHOD(NumTries);
    static NAN_METHOD(NumKeys);
    static NAN_METHOD(NumNodes);
    static NAN_METHOD(TailMode);
    static NAN_METHOD(NodeOrder);
    static NAN_METHOD(Empty);
    static NAN_METHOD(Size);
    static NAN_METHOD(IoSize);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Swap);

    bool CheckState() const;

    static Nan::Persistent<v8::FunctionTemplate> constructor;
    marisa::Trie trie_;
};


#endif  // TRIE_WRAP_HPP
