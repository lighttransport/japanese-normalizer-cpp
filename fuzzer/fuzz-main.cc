#include <iostream>

#define JP_NORMALIZER_IMPLEMENTATION
#include "jp_normalizer.hh"

#ifdef __clang__
#if __has_warning("-Wc++20-compat")
// suppress UTF8 string literal in source code warning.
#pragma clang diagnostic ignored "-Wc++20-compat"
#endif
#endif

static int run_normalier(const uint8_t *data, size_t size) {
  if (size > (1024 * 1024 * 1024)) {
    return -1;
  }

  std::string input_str(reinterpret_cast<const char *>(data), size);
  jpnormalizer::NormalizationOption opt;
  // TODO: read option param from input stream.
  std::string ret = jpnormalizer::normalize(input_str);
  (void)ret;

  return 0;
}

extern "C"
int LLVMFuzzerTestOneInput(std::uint8_t const* data, std::size_t size)
{
   int ret = run_normalier(data, size);
   return ret;
}

