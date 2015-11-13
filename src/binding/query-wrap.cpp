#include "query-wrap.hpp"


Nan::Persistent<v8::FunctionTemplate> QueryWrap::constructor;

NAN_MODULE_INIT(QueryWrap::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(QueryWrap::New);
  tpl->SetClassName(Nan::New("Query").ToLocalChecked());
  // Field#0 must be used by ObjectWrap::Wrap to export the instance of QueryWrap.
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetIndexedPropertyHandler(tpl->InstanceTemplate(), QueryWrap::GetChar);

  Nan::SetPrototypeMethod(tpl, "ptr", QueryWrap::Ptr);
  Nan::SetPrototypeMethod(tpl, "length", QueryWrap::Length);
  Nan::SetPrototypeMethod(tpl, "id", QueryWrap::Id);
  Nan::SetPrototypeMethod(tpl, "clear", QueryWrap::Clear);
  Nan::SetPrototypeMethod(tpl, "swap", QueryWrap::Swap);

  constructor.Reset(tpl);
  Nan::Set(target, Nan::New("Query").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(QueryWrap::New) {
  if (info.IsConstructCall()) {
    QueryWrap* self = NULL;
    QueryWrap* wrap = NULL;
    if (info[0]->IsUndefined() || info[0]->IsNull()) {
      self = new QueryWrap();
    } else if (NULL != (wrap = NAN_UNWRAP2(QueryWrap, info[0]))) {
      self = new QueryWrap(wrap->query_);
    } else {
      Nan::ThrowTypeError("Argument#1 must be a query if provided.");
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

NAN_INDEX_GETTER(QueryWrap::GetChar) {
  QueryWrap* self = NAN_UNWRAP(QueryWrap, info.This());
  if (index < self->query_.length()) {
    info.GetReturnValue().Set(static_cast<unsigned char>(self->query_[index]));
  } else {
    info.GetReturnValue().SetUndefined();
  }
}

NAN_METHOD(QueryWrap::Ptr) {
  QueryWrap* self = NAN_UNWRAP(QueryWrap, info.This());
  if (self->query_.length() > 0) {
    info.GetReturnValue().Set(Nan::New(self->query_.ptr(), self->query_.length()).ToLocalChecked());;
  } else {
    info.GetReturnValue().SetEmptyString();
  }
}

NAN_METHOD(QueryWrap::Length) {
  QueryWrap* self = NAN_UNWRAP(QueryWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->query_.length()));
}

NAN_METHOD(QueryWrap::Id) {
  QueryWrap* self = NAN_UNWRAP(QueryWrap, info.This());
  info.GetReturnValue().Set(static_cast<double>(self->query_.id()));
}

NAN_METHOD(QueryWrap::Clear) {
  QueryWrap* self = NAN_UNWRAP(QueryWrap, info.This());
  self->query_.clear();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(QueryWrap::Swap) {
  QueryWrap* self = NAN_UNWRAP(QueryWrap, info.This());
  QueryWrap* wrap = NAN_UNWRAP2(QueryWrap, info[0]);
  if (NULL != wrap) {
    self->query_.swap(wrap->query_);
    info.GetReturnValue().SetUndefined();
  } else {
    Nan::ThrowTypeError("Argument#1 must be a query.");
  }
}

void QueryWrap::SetStr(const char* ptr) {
  query_.set_str(ptr);
}

void QueryWrap::SetStr(const char* ptr, const size_t length) {
  query_.set_str(ptr, length);
}

void QueryWrap::SetId(const size_t id) {
  query_.set_id(id);
}
