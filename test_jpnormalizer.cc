#include <iostream>

#define JP_NORMALIZER_IMPLEMENTATION
#include "jp_normalizer.hh"

#ifdef __clang__
#if __has_warning("-Wc++20-compat")
// suppress UTF8 string literal in source code warning.
#pragma clang diagnostic ignored "-Wc++20-compat"
#endif
#endif

#define CHECK_TEXT(input_txt, expected_txt) do { \
  std::string ret = jpnormalizer::normalize(input_txt); \
  if (ret.compare(expected_txt) != 0) { \
    std::cerr << "fail: expected \"" << expected_txt << "\" but got \"" << ret << "\"\n"; \
  } else { \
    std::cout << "ok: \"" << ret << "\"\n"; \
  } \
} while (0)

#define CHECK_TEXT_OPT(input_txt, expected_txt, opt) do { \
  std::string ret = jpnormalizer::normalize(input_txt, opt); \
  if (ret.compare(expected_txt) != 0) { \
    std::cerr << "fail: expected \"" << expected_txt << "\" but got \"" << ret << "\"\n"; \
  } else { \
    std::cout << "ok: \"" << ret << "\"\n"; \
  } \
} while (0)

static void unit_test() {
  jpnormalizer::NormalizationOption opt;

  CHECK_TEXT("ﾊﾝｶｸｶﾅ", "ハンカクカナ");
  CHECK_TEXT("全角記号！？＠＃", "全角記号!?@#");
  CHECK_TEXT("全角記号例外「・」", "全角記号例外「・」");
  CHECK_TEXT("長音短縮ウェーーーーイ", "長音短縮ウェーイ");
  CHECK_TEXT("チルダ削除ウェ~∼∾〜〰～イ", "チルダ削除ウェイ");
  CHECK_TEXT("いろんなハイフン˗֊‐‑‒–⁃⁻₋−", "いろんなハイフン-");
  CHECK_TEXT("　　　ＰＲＭＬ　　副　読　本　　　", "PRML副読本");
  CHECK_TEXT(" Natural Language Processing ", "Natural Language Processing");

  opt.repeat = 6;
  CHECK_TEXT_OPT("かわいいいいいいいいい", "かわいいいいいい", opt);

  opt.repeat = 1;
  CHECK_TEXT_OPT("無駄無駄無駄無駄ァ", "無駄ァ", opt);

  opt.repeat = 0;
  opt.tilde = jpnormalizer::NormalizationOption::TildeMode::Normalize;
  CHECK_TEXT_OPT("1995〜2001年", "1995~2001年", opt);
  opt.tilde = jpnormalizer::NormalizationOption::TildeMode::Zenkaku;
  CHECK_TEXT_OPT("1995~2001年", "1995〜2001年", opt);
  opt.tilde = jpnormalizer::NormalizationOption::TildeMode::Ignore;
  CHECK_TEXT_OPT("1995〜2001年", "1995〜2001年", opt);
  opt.tilde = jpnormalizer::NormalizationOption::TildeMode::Remove;
  CHECK_TEXT_OPT("1995〜2001年", "19952001年", opt);
  opt = jpnormalizer::NormalizationOption();
  CHECK_TEXT("1995〜2001年", "19952001年");

  // Addtional
  opt.parenthesized_ideographs = true;
  CHECK_TEXT_OPT("ﾜｶﾞﾊｲは㈱である", "ワガハイは(株)である", opt);
}

int main(int argc, char **argv) {
  jpnormalizer::NormalizationOption opt;
  const auto test_text = u8"ﾜｶﾞﾊｲは㈱である.  ㈴ＭＡＥはまだ迺";

  std::string text;
  if (argc > 1) {
    // NOTE: To read UTF-8 string from stdio in Windows, Terminal's encoding must be UTF-8.
    // TODO: Automatically convert UNICODE, CP932, etc string in stdio to UTF-8.
    text = argv[1];
  } else {
    unit_test();
    text = std::string(test_text);
    opt.parenthesized_ideographs = true;
  }

  std::cout << "input: " << text << "\n";
  std::string normalied_text = jpnormalizer::normalize(text, opt);

  std::cout << "normalized: " << normalied_text << "\n";


  return EXIT_SUCCESS;
}
