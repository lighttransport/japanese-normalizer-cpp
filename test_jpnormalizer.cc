#include <iostream>

#define JP_NORMALIZER_IMPLEMENTATION
#include "jp_normalizer.hh"

int main(int argc, char **argv) {
  std::string text = "ﾜｶﾞﾊｲは㈱である.  ㈴ＭＡＥはまだ迺";

  if (argc > 1) {
    // NOTE: To read UTF-8 string from stdio in Windows, Terminal's encoding must be UTF-8.
    // TODO: Automatically convert UNICODE, CP932, etc string in stdio to UTF-8.
    text = argv[1];
  }

  std::cout << "input: " << text << "\n";
  std::string normalied_text = jpnormalizer::normalize(text);

  std::cout << "normalized: " << normalied_text << "\n";
  return EXIT_SUCCESS;
}
