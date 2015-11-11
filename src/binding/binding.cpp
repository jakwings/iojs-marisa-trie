#include "binding.hpp"
#include "marisa-wrap.hpp"


NAN_MODULE_INIT(Init) {
  MarisaWrap::Init(target);
}


NODE_MODULE(binding, Init)// NO ; HERE
