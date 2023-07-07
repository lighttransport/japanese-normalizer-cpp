#include <iostream>

#define JP_NORMALIZER_IMPLEMENTATION
#include "jp_normalizer.hh"

#ifdef __clang__
#if __has_warning("-Wc++20-compat")
// suppress UTF8 string literal in source code warning.
#pragma clang diagnostic ignored "-Wc++20-compat"
#endif
#endif

int main(int argc, char **argv) {
  const auto test_text = u8"ﾜｶﾞﾊｲは㈱である.  ㈴ＭＡＥはまだ迺";

  std::string text;
  if (argc > 1) {
    // NOTE: To read UTF-8 string from stdio in Windows, Terminal's encoding must be UTF-8.
    // TODO: Automatically convert UNICODE, CP932, etc string in stdio to UTF-8.
    text = argv[1];
  } else {
    text = std::string(test_text);
  }

  std::cout << "input: " << text << "\n";
  std::string normalied_text = jpnormalizer::normalize(text);

  std::cout << "normalized: " << normalied_text << "\n";
  return EXIT_SUCCESS;
}
