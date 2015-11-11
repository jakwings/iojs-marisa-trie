#include "trie-wrap.hpp"
#include "agent-wrap.hpp"
#include "keyset-wrap.hpp"


Nan::Persistent<v8::FunctionTemplate> TrieWrap::constructor;

NAN_MODULE_INIT(TrieWrap::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(TrieWrap::New);
  tpl->SetClassName(Nan::New("Trie").ToLocalChecked());
  // Field#0 must be used by ObjectWrap::Wrap to export the instance of TrieWrap.
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "build", TrieWrap::Build);
  Nan::SetPrototypeMethod(tpl, "mmap", TrieWrap::Mmap);
  Nan::SetPrototypeMethod(tpl, "map", TrieWrap::Map);
  Nan::SetPrototypeMethod(tpl, "load", TrieWrap::Load);
  Nan::SetPrototypeMethod(tpl, "read", TrieWrap::Read);
  Nan::SetPrototypeMethod(tpl, "save", TrieWrap::Save);
  Nan::SetPrototypeMethod(tpl, "write", TrieWrap::Write);
  Nan::SetPrototypeMethod(tpl, "lookup", TrieWrap::Lookup);
  Nan::SetPrototypeMethod(tpl, "reverse_lookup", TrieWrap::ReverseLookup);
  Nan::SetPrototypeMethod(tpl, "common_prefix_search", TrieWrap::CommonPrefixSearch);
  Nan::SetPrototypeMethod(tpl, "predictive_search", TrieWrap::PredictiveSearch);
  Nan::SetPrototypeMethod(tpl, "num_tries", TrieWrap::NumTries);
  Nan::SetPrototypeMethod(tpl, "num_keys", TrieWrap::NumKeys);
  Nan::SetPrototypeMethod(tpl, "num_nodes", TrieWrap::NumNodes);
  Nan::SetPrototypeMethod(tpl, "tail_mode", TrieWrap::TailMode);
  Nan::SetPrototypeMethod(tpl, "node_order", TrieWrap::NodeOrder);
  Nan::SetPrototypeMethod(tpl, "empty", TrieWrap::Empty);
  Nan::SetPrototypeMethod(tpl, "size", TrieWrap::Size);
  Nan::SetPrototypeMethod(tpl, "io_size", TrieWrap::IoSize);
  Nan::SetPrototypeMethod(tpl, "clear", TrieWrap::Clear);
  Nan::SetPrototypeMethod(tpl, "swap", TrieWrap::Swap);

  constructor.Reset(tpl);
  Nan::Set(target, Nan::New("Trie").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(TrieWrap::New) {
  if (info.IsConstructCall()) {
    TrieWrap* self = new TrieWrap();
    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {  // Turn it into a construct call.
    v8::Local<v8::Function> cons = Nan::GetFunction(Nan::New(constructor)).ToLocalChecked();
    info.GetReturnValue().Set(Nan::NewInstance(cons).ToLocalChecked());
  }
}

NAN_METHOD(TrieWrap::Build) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  KeysetWrap* wrap = Nan::Unwrap<KeysetWrap>(Nan::New(KeysetWrap::constructor), info[0]);
  if (NULL != wrap) {
    int64_t flag = 0;
    if (info[1]->IsNumber()) {
      flag = Nan::To<int64_t>(info[1]).FromJust();
    } else if (!(info[1]->IsUndefined() || info[1]->IsNull())) {
      Nan::ThrowTypeError("Argument#2 must be an integer if provided.");
    }
    self->trie_.build(wrap->keyset(), flag);
    info.GetReturnValue().SetUndefined();
  } else {
    Nan::ThrowTypeError("Argument#1 must be a keyset.");
  }
}

NAN_METHOD(TrieWrap::Mmap) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (info[0]->IsString()) {
    Nan::Utf8String filepath(info[0]);
    try {
      self->trie_.mmap(*filepath);
      info.GetReturnValue().SetUndefined();
    } catch (const marisa::Exception& ex) {
      Nan::ThrowError("Invalid trie data.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be a string.");
  }
}

NAN_METHOD(TrieWrap::Map) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (info[0]->IsObject() && node::Buffer::HasInstance(info[0])) {
    v8::Local<v8::Object> obj = Nan::To<v8::Object>(info[0]).ToLocalChecked();
    char* buffer = node::Buffer::Data(obj);
    size_t blen = node::Buffer::Length(obj);
    if (info[1]->IsNumber()) {
      double length = Nan::To<double>(info[1]).FromJust();
      if (length >= 0 && length < blen) {
        blen = static_cast<size_t>(length);
      }
    } else if (!(info[1]->IsUndefined() || info[1]->IsNull())) {
      Nan::ThrowTypeError("Argument#2 must be an integer if provided.");
    }
    try {
      self->trie_.map(buffer, blen);
      info.GetReturnValue().SetUndefined();
    } catch (const marisa::Exception& ex) {
      Nan::ThrowError("Invalid trie data.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be a buffer.");
  }
}

NAN_METHOD(TrieWrap::Load) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (info[0]->IsString()) {
    Nan::Utf8String filepath(info[0]);
    try {
      self->trie_.load(*filepath);
      info.GetReturnValue().SetUndefined();
    } catch (const marisa::Exception& ex) {
      Nan::ThrowError("Invalid trie data.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be a string.");
  }
}

NAN_METHOD(TrieWrap::Read) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (info[0]->IsNumber()) {
    int64_t fd = Nan::To<int64_t>(info[0]).FromJust();
    try {
      self->trie_.read(fd);
      info.GetReturnValue().SetUndefined();
    } catch (const marisa::Exception& ex) {
      Nan::ThrowError("Invalid trie data.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be an integer.");
  }
}

NAN_METHOD(TrieWrap::Save) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (info[0]->IsString()) {
    if (self->CheckState()) {
      Nan::Utf8String filepath(info[0]);
      try {
        self->trie_.save(*filepath);
        info.GetReturnValue().SetUndefined();
      } catch (const marisa::Exception& ex) {
        Nan::ThrowError("Failed to save trie data.");
      }
    } else {
      Nan::ThrowError("Invalid trie.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be a string.");
  }
}

NAN_METHOD(TrieWrap::Write) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (info[0]->IsNumber()) {
    if (self->CheckState()) {
      int64_t fd = Nan::To<int64_t>(info[0]).FromJust();
      try {
        self->trie_.write(fd);
        info.GetReturnValue().SetUndefined();
      } catch (const marisa::Exception& ex) {
        Nan::ThrowError("Failed to write trie data.");
      }
    } else {
      Nan::ThrowError("Invalid trie.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be an integer.");
  }
}

NAN_METHOD(TrieWrap::Lookup) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  AgentWrap* wrap = Nan::Unwrap<AgentWrap>(Nan::New(AgentWrap::constructor), info[0]);
  if (NULL != wrap) {
    if (self->CheckState()) {
      info.GetReturnValue().Set(self->trie_.lookup(wrap->agent()));
    } else {
      Nan::ThrowError("Invalid trie.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be an agent.");
  }
}

NAN_METHOD(TrieWrap::ReverseLookup) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  AgentWrap* wrap = Nan::Unwrap<AgentWrap>(Nan::New(AgentWrap::constructor), info[0]);
  if (NULL != wrap) {
    if (self->CheckState()) {
      if (wrap->agent().query().id() < self->trie_.size()) {
        self->trie_.reverse_lookup(wrap->agent());
        info.GetReturnValue().SetUndefined();
      } else {
        Nan::ThrowRangeError("ID is out of range.");
      }
    } else {
      Nan::ThrowError("Invalid trie.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be an agent.");
  }
}

NAN_METHOD(TrieWrap::CommonPrefixSearch) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  AgentWrap* wrap = Nan::Unwrap<AgentWrap>(Nan::New(AgentWrap::constructor), info[0]);
  if (NULL != wrap) {
    if (self->CheckState()) {
      info.GetReturnValue().Set(self->trie_.common_prefix_search(wrap->agent()));
    } else {
      Nan::ThrowError("Invalid trie.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be an agent.");
  }
}

NAN_METHOD(TrieWrap::PredictiveSearch) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  AgentWrap* wrap = Nan::Unwrap<AgentWrap>(Nan::New(AgentWrap::constructor), info[0]);
  if (NULL != wrap) {
    if (self->CheckState()) {
      info.GetReturnValue().Set(self->trie_.predictive_search(wrap->agent()));
    } else {
      Nan::ThrowError("Invalid trie.");
    }
  } else {
    Nan::ThrowTypeError("Argument#1 must be an agent.");
  }
}

NAN_METHOD(TrieWrap::NumTries) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(static_cast<double>(self->trie_.num_tries()));
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::NumKeys) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(static_cast<double>(self->trie_.num_keys()));
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::NumNodes) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(static_cast<double>(self->trie_.num_nodes()));
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::TailMode) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(self->trie_.tail_mode());
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::NodeOrder) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(self->trie_.node_order());
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::Empty) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(self->trie_.empty());
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::Size) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(static_cast<double>(self->trie_.size()));
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::IoSize) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  if (self->CheckState()) {
    info.GetReturnValue().Set(static_cast<double>(self->trie_.io_size()));
  } else {
    Nan::ThrowError("Invalid trie.");
  }
}

NAN_METHOD(TrieWrap::Clear) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  self->trie_.clear();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(TrieWrap::Swap) {
  TrieWrap* self = NAN_UNWRAP(TrieWrap, info.Holder());
  TrieWrap* wrap = Nan::Unwrap<TrieWrap>(Nan::New(constructor), info[0]);
  if (NULL != wrap) {
    self->trie_.swap(wrap->trie_);
    info.GetReturnValue().SetUndefined();
  } else {
    Nan::ThrowTypeError("Argument#1 must be a trie.");
  }
}

bool TrieWrap::CheckState() const {
  try {
    trie_.num_nodes();
    return true;
  } catch (const marisa::Exception& ex) {
    return false;
  }
}
