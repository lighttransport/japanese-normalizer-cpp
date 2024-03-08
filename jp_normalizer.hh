// SPDX-License-Identifier: Apache 2.0
// Copyright 2023 - Present, Light Transport Entertainement Inc.
//
// Assume this file is encoded in UTF-8
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace jpnormalizer {

struct NormalizationOption {
  enum class TildeMode {
    Remove,     // remove tilde
    Ignore,     // ignore(do nothing) for tilde char.
    Normalize,  // convert to ASCII '~'
    Zenkaku,    // convert to Zenkaku '〜'
  };

  uint32_t max_tokens{1024ull*1024ull*1024ull}; // default 1 GB tokens(~= 3GB in Japanese UTF-8 chars)

  bool remove_space{true};
  uint32_t repeat{0};
  uint32_t max_repeat_substr_len{8};
  TildeMode tilde{TildeMode::Remove};

  // jpnormalizer specific feature.
  bool parenthesized_ideographs{true};
};

struct DedupNormalizationOption {
  // Placeholder token for digits
  char digit_placeholder{'0'};
};

std::string normalize(const std::string& str,
                      const NormalizationOption option = NormalizationOption());

std::string normalize_for_dedup(const std::string& str,
                      const DedupNormalizationOption option = DedupNormalizationOption());

std::unordered_set<std::string> get_digits();
std::unordered_set<std::string> get_digits_and_parentized_ideographs();
const std::unordered_set<std::string> &get_unicode_puncts();

#if defined(WIN32)
// TODO: wstring version
#endif

}  // namespace jpnormalizer

#if defined(JP_NORMALIZER_IMPLEMENTATION)

#include <cstring>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jpnormalizer {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif


static std::unordered_map<std::string, char> sASCII{
    {"ａ", 'a'}, {"ｂ", 'b'}, {"ｃ", 'c'}, {"ｄ", 'd'}, {"ｅ", 'e'},
    {"ｆ", 'f'}, {"ｇ", 'g'}, {"ｈ", 'h'}, {"ｉ", 'i'}, {"ｊ", 'j'},
    {"ｋ", 'k'}, {"ｌ", 'l'}, {"ｍ", 'm'}, {"ｎ", 'n'}, {"ｏ", 'o'},
    {"ｐ", 'p'}, {"ｑ", 'q'}, {"ｒ", 'r'}, {"ｓ", 's'}, {"ｔ", 't'},
    {"ｕ", 'u'}, {"ｖ", 'v'}, {"ｗ", 'w'}, {"ｘ", 'x'}, {"ｙ", 'y'},
    {"ｚ", 'z'}, {"Ａ", 'A'}, {"Ｂ", 'B'}, {"Ｃ", 'C'}, {"Ｄ", 'D'},
    {"Ｅ", 'E'}, {"Ｆ", 'F'}, {"Ｇ", 'G'}, {"Ｈ", 'H'}, {"Ｉ", 'I'},
    {"Ｊ", 'J'}, {"Ｋ", 'K'}, {"Ｌ", 'L'}, {"Ｍ", 'M'}, {"Ｎ", 'N'},
    {"Ｏ", 'O'}, {"Ｐ", 'P'}, {"Ｑ", 'Q'}, {"Ｒ", 'R'}, {"Ｓ", 'S'},
    {"Ｔ", 'T'}, {"Ｕ", 'U'}, {"Ｖ", 'V'}, {"Ｗ", 'W'}, {"Ｘ", 'X'},
    {"Ｙ", 'Y'}, {"Ｚ", 'Z'}, {"！", '!'}, {"”", '"'},  {"＃", '#'},
    {"＄", '$'}, {"％", '%'}, {"＆", '&'}, {"’", '\''}, {"（", '{'},
    {"）", '}'}, {"＊", '*'}, {"＋", '+'}, {"，", ','}, {"−", '-'},
    {"．", '.'}, {"／", '/'}, {"：", ':'}, {"；", ';'}, {"＜", '<'},
    {"＝", '='}, {"＞", '>'}, {"？", '?'}, {"＠", '@'}, {"［", '['},
    {"¥", '\\'}, {"］", ']'}, {"＾", '^'}, {"＿", '_'}, {"‘", '`'},
    {"｛", '{'}, {"｜", '|'}, {"｝", '}'}};

static std::unordered_map<std::string, std::string> sKANA = {
    {"ｱ", "ア"}, {"ｲ", "イ"}, {"ｳ", "ウ"}, {"ｴ", "エ"}, {"ｵ", "オ"},
    {"ｶ", "カ"}, {"ｷ", "キ"}, {"ｸ", "ク"}, {"ｹ", "ケ"}, {"ｺ", "コ"},
    {"ｻ", "サ"}, {"ｼ", "シ"}, {"ｽ", "ス"}, {"ｾ", "セ"}, {"ｿ", "ソ"},
    {"ﾀ", "タ"}, {"ﾁ", "チ"}, {"ﾂ", "ツ"}, {"ﾃ", "テ"}, {"ﾄ", "ト"},
    {"ﾅ", "ナ"}, {"ﾆ", "ニ"}, {"ﾇ", "ヌ"}, {"ﾈ", "ネ"}, {"ﾉ", "ノ"},
    {"ﾊ", "ハ"}, {"ﾋ", "ヒ"}, {"ﾌ", "フ"}, {"ﾍ", "ヘ"}, {"ﾎ", "ホ"},
    {"ﾏ", "マ"}, {"ﾐ", "ミ"}, {"ﾑ", "ム"}, {"ﾒ", "メ"}, {"ﾓ", "モ"},
    {"ﾔ", "ヤ"}, {"ﾕ", "ユ"}, {"ﾖ", "ヨ"}, {"ﾗ", "ラ"}, {"ﾘ", "リ"},
    {"ﾙ", "ル"}, {"ﾚ", "レ"}, {"ﾛ", "ロ"}, {"ﾜ", "ワ"}, {"ｦ", "ヲ"},
    {"ﾝ", "ン"}, {"ｧ", "ァ"}, {"ｨ", "ィ"}, {"ｩ", "ゥ"}, {"ｪ", "ェ"},
    {"ｫ", "ォ"}, {"ｯ", "ッ"}, {"ｬ", "ャ"}, {"ｭ", "ュ"}, {"ｮ", "ョ"},
    {"｡", "。"}, {"､", "、"}, {"･", "・"}, {"゛", "ﾞ"}, {"゜", "ﾟ"},
    {"｢", "「"}, {"｣", "」"}, {"ｰ", "ー"}};

static std::unordered_map<std::string, std::string> sDIGIT = {
    {"０", "0"}, {"１", "1"}, {"２", "2"}, {"３", "3"}, {"４", "4"},
    {"５", "5"}, {"６", "6"}, {"７", "7"}, {"８", "8"}, {"９", "9"}};
static std::unordered_map<std::string, std::string> sKANA_TEN = {
    {"カ", "ガ"}, {"キ", "ギ"}, {"ク", "グ"}, {"ケ", "ゲ"}, {"コ", "ゴ"},
    {"サ", "ザ"}, {"シ", "ジ"}, {"ス", "ズ"}, {"セ", "ゼ"}, {"ソ", "ゾ"},
    {"タ", "ダ"}, {"チ", "ヂ"}, {"ツ", "ヅ"}, {"テ", "デ"}, {"ト", "ド"},
    {"ハ", "バ"}, {"ヒ", "ビ"}, {"フ", "ブ"}, {"ヘ", "ベ"}, {"ホ", "ボ"},
    {"ウ", "ヴ"}, {"う", "ゔ"}};
static std::unordered_map<std::string, std::string> sKANA_MARU = {
    {"ハ", "パ"}, {"ヒ", "ピ"}, {"フ", "プ"}, {"ヘ", "ペ"}, {"ホ", "ポ"},
    {"は", "ぱ"}, {"ひ", "ぴ"}, {"ふ", "ぷ"}, {"へ", "ぺ"}, {"ほ", "ぽ"}};

// not in neologdn
static std::unordered_map<std::string, std::string> sParenthesizedIdeographs = {
    {"㈠", "(一)"}, {"㈡", "(二)"}, {"㈢", "(三)"}, {"㈣", "(四)"},
    {"㈤", "(五)"}, {"㈥", "(六)"}, {"㈦", "(七)"}, {"㈧", "(八)"},
    {"㈨", "(九)"}, {"㈩", "(十)"}, {"㈪", "(月)"}, {"㈫", "(火)"},
    {"㈬", "(水)"}, {"㈭", "(木)"}, {"㈮", "(金)"}, {"㈯", "(土)"},

    {"㈰", "(日)"}, {"㈱", "(株)"}, {"㈲", "(有)"}, {"㈳", "(社)"},
    {"㈴", "(名)"}, {"㈵", "(特)"}, {"㈶", "(財)"}, {"㈷", "(祝)"},
    {"㈸", "(労)"}, {"㈹", "(代)"}, {"㈺", "(呼)"}, {"㈻", "(学)"},
    {"㈼", "(監)"}, {"㈽", "(企)"}, {"㈾", "(資)"}, {"㈿", "(協)"},
    {"㉀", "(祭)"}, {"㉁", "(休)"}, {"㉂", "(自)"}, {"㉃", "(至)"}};

static std::unordered_set<std::string> sHYPHENS = {"˗", "֊", "‐", "‑", "‒",
                                                   "–", "⁃", "⁻", "₋", "−"};

static std::unordered_set<std::string> sCHOONPUS = {"﹣", "－", "ｰ", "—",
                                                    "―",  "─",  "━", "ー"};

static std::unordered_set<std::string> sTILDES = {"~",  "∼",  "∾",
                                                  "〜", "〰", "～"};

// TODO: We can simply use direct string comparision for space character.
static std::unordered_set<std::string> sSPACE = {" ", "　"};

static std::unordered_set<std::string> sUNICODE_PUNCT = {
    "，",
    "。",
    "、",
    "„"
    "”",
    "“",
    "«",
    "»",
    "１",
    "」",
    "「",
    "《",
    "》",
    "´",
    "∶",
    "：",
    "？",
    "！",
    "（",
    "）",
    "；",
    "–",
    "—",
    "．",
    "～",
    "’",
    "…",
    "━",
    "〈",
    "〉",
    "【",
    "】",
    "％",
    "►",
    ":",
    ";",
    "-",
    ",",
    ".",
    "?",
    "!",
    ")",
    "(",
    "<",
    ">",
    "[",
    "]",
    "\"",
    "'"
};


#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace detail {

    inline uint32_t utf8_len(const uint8_t c) {
      if (c <= 127) {
        // ascii
        return 1;
      } else if ((c & 0xE0) == 0xC0) {
        return 2;
      } else if ((c & 0xF0) == 0xE0) {
        return 3;
      } else if ((c & 0xF8) == 0xF0) {
        return 4;
      }

      // invalid
      return 0;
    }


    inline uint32_t to_codepoint(const char *s, uint32_t &len) {
      if (!s) {
        return ~0u;
      }

      uint32_t char_len = utf8_len(uint8_t(*s));

      uint32_t code = 0;
      if (char_len == 1) {
        unsigned char s0 = static_cast<unsigned char>(s[0]);
        if (s0 > 0x7f) {
          len = 0;
          return ~0u;
        }
        code = uint32_t(s0) & 0x7f;
      } else if (char_len == 2) {
        // 11bit: 110y-yyyx 10xx-xxxx
        unsigned char s0 = static_cast<unsigned char>(s[0]);
        unsigned char s1 = static_cast<unsigned char>(s[1]);

        if (((s0 & 0xe0) == 0xc0) && ((s1 & 0xc0) == 0x80)) {
          code = (uint32_t(s0 & 0x1f) << 6) | (s1 & 0x3f);
        } else {
          len = 0;
          return ~0u;
        }
      } else if (char_len == 3) {
        // 16bit: 1110-yyyy 10yx-xxxx 10xx-xxxx
        unsigned char s0 = static_cast<unsigned char>(s[0]);
        unsigned char s1 = static_cast<unsigned char>(s[1]);
        unsigned char s2 = static_cast<unsigned char>(s[2]);
        if (((s0 & 0xf0) == 0xe0) && ((s1 & 0xc0) == 0x80) &&
            ((s2 & 0xc0) == 0x80)) {
          code =
              (uint32_t(s0 & 0xf) << 12) | (uint32_t(s1 & 0x3f) << 6) | (s2 & 0x3f);
        } else {
          len = 0;
          return ~0u;
        }
      } else if (char_len == 4) {
        // 21bit: 1111-0yyy 10yy-xxxx 10xx-xxxx 10xx-xxxx
        unsigned char s0 = static_cast<unsigned char>(s[0]);
        unsigned char s1 = static_cast<unsigned char>(s[1]);
        unsigned char s2 = static_cast<unsigned char>(s[2]);
        unsigned char s3 = static_cast<unsigned char>(s[3]);
        if (((s0 & 0xf8) == 0xf0) && ((s1 & 0xc0) == 0x80) &&
            ((s2 & 0xc0) == 0x80) && ((s2 & 0xc0) == 0x80)) {
          code = (uint32_t(s0 & 0x7) << 18) | (uint32_t(s1 & 0x3f) << 12) |
                 (uint32_t(s2 & 0x3f) << 6) | uint32_t(s3 & 0x3f);
        } else {
          len = 0;
          return ~0u;
        }
      } else {
        len = 0;
        return ~0u;
      }

      len = char_len;
      return code;
    }


inline std::string codepoint_to_utf8(uint32_t code) {
  if (code <= 0x7f) {
    return std::string(1, char(code));
  } else if (code <= 0x7ff) {
    // 11bit: 110y-yyyx 10xx-xxxx
    uint8_t buf[2];
    buf[0] = uint8_t(((code >> 6) & 0x1f) | 0xc0);
    buf[1] = uint8_t(((code >> 0) & 0x3f) | 0x80);
    return std::string(reinterpret_cast<const char *>(&buf[0]), 2);
  } else if (code <= 0xffff) {
    // 16bit: 1110-yyyy 10yx-xxxx 10xx-xxxx
    uint8_t buf[3];
    buf[0] = uint8_t(((code >> 12) & 0x0f) | 0xe0);
    buf[1] = uint8_t(((code >>  6) & 0x3f) | 0x80);
    buf[2] = uint8_t(((code >>  0) & 0x3f) | 0x80);
    return std::string(reinterpret_cast<const char *>(&buf[0]), 3);
  } else if (code <= 0x10ffff) {
    // 21bit: 1111-0yyy 10yy-xxxx 10xx-xxxx 10xx-xxxx
    uint8_t buf[4];
    buf[0] = uint8_t(((code >> 18) & 0x07) | 0xF0);
    buf[1] = uint8_t(((code >> 12) & 0x3F) | 0x80);
    buf[2] = uint8_t(((code >>  6) & 0x3F) | 0x80);
    buf[3] = uint8_t(((code >>  0) & 0x3F) | 0x80);
    return std::string(reinterpret_cast<const char *>(&buf[0]), 4);
  }

  // invalid
  return std::string();
}


inline std::vector<uint32_t> to_codepoints(const std::string &str)
{
  size_t i = 0;
  std::vector<uint32_t> codes;
  while (i < str.size()) {
    uint32_t len;
    uint32_t code = to_codepoint(&str[i], len);

    codes.push_back(code);
    i += len;
  }

  return codes;
}

inline std::string codepoints_to_string(const std::vector<uint32_t> &codepoints)
{
  std::string ret;

  for (const auto &code : codepoints) {
    ret += codepoint_to_utf8(code);
  }

  return ret;
}

inline std::string extract_utf8_char(const std::string& str, uint32_t start_i,
                                     int& len) {
  len = 0;

  if ((start_i + 1) > str.size()) {
    len = 0;
    return std::string();
  }

  unsigned char c = static_cast<unsigned char>(str[start_i]);

  if (c <= 127) {
    // ascii
    len = 1;
    return str.substr(start_i, 1);
  } else if ((c & 0xE0) == 0xC0) {
    if ((start_i + 2) > str.size()) {
      len = 0;
      return std::string();
    }
    len = 2;
    return str.substr(start_i, 2);
  } else if ((c & 0xF0) == 0xE0) {
    if ((start_i + 3) > str.size()) {
      len = 0;
      return std::string();
    }
    len = 3;
    return str.substr(start_i, 3);
  } else if ((c & 0xF8) == 0xF0) {
    if ((start_i + 4) > str.size()) {
      len = 0;
      return std::string();
    }
    len = 4;
    return str.substr(start_i, 4);
  } else {
    // invalid utf8
    len = 0;
    return std::string();
  }
}

inline uint32_t utf8_code(const std::string &s) {
  if (s.empty() || (s.size() > 4)) {
    return ~0u; // invalid
  }

  // TODO: endianness
  uint32_t code = 0;
  if (s.size() == 1) {
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    if (s0 > 0x7f) {
      return ~0u;
    }
    code = uint32_t(s0) & 0x7f;
  } else if (s.size() == 2) {
    // 11bit: 110y-yyyx	10xx-xxxx
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    unsigned char s1 = static_cast<unsigned char>(s[1]);

    if (((s0 & 0xe0) == 0xc0) && ((s1 & 0xc0) == 0x80)) {
      code = (uint32_t(s0 & 0x1f) << 6) | (s1 & 0x3f);
    } else {
      return ~0u;
    }
  } else if (s.size() == 3) {
    // 16bit: 1110-yyyy	10yx-xxxx	10xx-xxxx
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    unsigned char s1 = static_cast<unsigned char>(s[1]);
    unsigned char s2 = static_cast<unsigned char>(s[2]);
    if (((s0 & 0xf0) == 0xe0) && ((s1 & 0xc0) == 0x80) && ((s2 & 0xc0) == 0x80)) {
      code = (uint32_t(s0 & 0xf) << 12) | (uint32_t(s1 & 0x3f) << 6) | (s2 & 0x3f);
    } else {
      return ~0u;
    }
  } else {
    // 21bit: 1111-0yyy	10yy-xxxx	10xx-xxxx	10xx-xxxx
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    unsigned char s1 = static_cast<unsigned char>(s[1]);
    unsigned char s2 = static_cast<unsigned char>(s[2]);
    unsigned char s3 = static_cast<unsigned char>(s[3]);
    if (((s0 & 0xf8) == 0xf0) && ((s1 & 0xc0) == 0x80) && ((s2 & 0xc0) == 0x80) && ((s2 & 0xc0) == 0x80)) {
      code = (uint32_t(s0 & 0x7) << 18) | (uint32_t(s1 & 0x3f) << 12) | (uint32_t(s2 & 0x3f) << 6) | uint32_t(s3 & 0x3f);
    } else {
      return ~0u;
    }
  }

  return code;
}

inline bool is_cjk_char(const std::string &s) {

  //    range(19968, 40960),  # CJK UNIFIED IDEOGRAPHS
  //    range(12352, 12448),  # HIRAGANA
  //    range(12448, 12544),  # KATAKANA
  //    range(12289, 12352),  # CJK SYMBOLS AND PUNCTUATION
  //    range(65280, 65520)   # HALFWIDTH AND FULLWIDTH FORMS

  uint32_t code = utf8_code(s);

  if ((code >= 19968) && (code < 40960)) {
    return true;
  } else if ((code >= 12352) && (code < 12448)) {
    return true;
  } else if ((code >= 12448) && (code < 12544)) {
    return true;
  } else if ((code >= 12289) && (code < 12352)) {
    return true;
  } else if ((code >= 65280) && (code < 65520)) {
    return true;
  }

  return false;
}


inline bool is_equal(const std::vector<uint32_t> &in,
  size_t s_pos0, size_t s_pos1, size_t len) {

  // return false when pos is out-of-range.
  if ((s_pos0 >= in.size()) ||
      (s_pos1 >= in.size()) ||
      ((s_pos0 + len) > in.size()) ||
      ((s_pos1 + len) > in.size())) {
    return false;
  }

  return std::equal(std::begin(in) + int64_t(s_pos0), std::begin(in) + int64_t(s_pos0 + len),
      std::begin(in) + int64_t(s_pos1));
}

static std::vector<uint32_t> shorten_repeat_codepoints(const std::vector<uint32_t> &u8_codepoints, uint32_t repeat_threshold, uint32_t max_repeat_substr_len=8) {

  std::vector<uint32_t> text = u8_codepoints;
  size_t i = 0;
  while (i < text.size()) {
    size_t text_len = text.size();

    // upper bound of repeat size = 1/2 of input text.
    size_t ceil_repeat_len = (text_len - i) / 2;

    if (max_repeat_substr_len < ceil_repeat_len) {
      ceil_repeat_len = max_repeat_substr_len + 1;
    }

    for (size_t repeat_len = 1; repeat_len < ceil_repeat_len; repeat_len++) {
      size_t right_start = i + repeat_len;
      size_t right_end = right_start + repeat_len;

      size_t num_repeat = 1;
      while (right_end <= text_len) {
        if (!is_equal(text, i, right_start, repeat_len)) {
          break;
        }

        num_repeat++;
        right_start += repeat_len;
        right_end += repeat_len;
      }

      if (num_repeat > repeat_threshold) {
        // cut out repeated substr
#if 0
        std::vector<uint32_t> buf;

        for (size_t k = 0; k < (std::min)(i + repeat_len * repeat_threshold, text.size()); k++) {
          buf.push_back(text[k]);
        }

        for (size_t k = i + repeat_len * num_repeat; k < text.size(); k++) {
          buf.push_back(text[k]);
        }

        text.swap(buf);
#else
        text.erase(text.begin() + int64_t((std::min)(i + repeat_len * repeat_threshold, text.size())), text.begin() + int64_t(i + repeat_len * num_repeat));
#endif
      }
    }

    i++;
  }

  return text;
}

static std::string shorten_repeat(const std::string &text, uint32_t repeat_threshold, uint32_t max_repeat_substr_len=8) {

  std::vector<uint32_t> codepoints = to_codepoints(text);

  std::vector<uint32_t> ret = shorten_repeat_codepoints(codepoints, repeat_threshold, max_repeat_substr_len);

  return codepoints_to_string(ret);
}


}  // namespace detail

std::string normalize(const std::string& str,
                      const NormalizationOption option) {

  if (str.empty()) {
    return std::string();
  }

  if (str.size() > option.max_tokens) {
    return std::string();
  }

  ///
  /// Decompose input string into UTF8 char list.
  ///
  uint64_t sz = str.size();
  std::vector<std::string> utf8_chars;

  for (size_t i = 0; i <= sz;) {
    int len=0;
    std::string s = detail::extract_utf8_char(str, uint32_t(i), len);
    if (len == 0) {
      // invalid char
      break;
    }

    i += uint64_t(len);
    utf8_chars.push_back(s);
  }

  std::vector<std::string> dst_buf;
  // normalized text should not exceed input length.
  dst_buf.resize(utf8_chars.size());

  std::string prev_c = "\0";
  bool latin_space = false;
  uint64_t loc = 0;

  for (size_t i = 0 ; i  < utf8_chars.size(); i++) {
    std::string c = utf8_chars[i];

    if (sSPACE.count(c)) {
      c = " ";
      //std::cout << "c is space: prev_c = " << prev_c << ", utf8 code " << detail::utf8_code(prev_c) << "\n";
      if (((prev_c == " ") || detail::is_cjk_char(prev_c)) && option.remove_space) {
        continue;
      } else if ((prev_c != "*") && (loc > 0) && (detail::utf8_code(prev_c) < 128)) {
        //std::cout << "latin space\n";
        latin_space = true;
        if (loc >= dst_buf.size()) {
          // ???
          return std::string();
        }
        dst_buf[loc] = c;
      } else if (option.remove_space) {
        if (loc == 0) {
          prev_c = c;
          continue;
        } else {
          loc--;
        }
      } else {
        if (loc >= dst_buf.size()) {
          // ???
          return std::string();
        }
        dst_buf[loc] = c;
      }
    } else {
      if (sHYPHENS.count(c)) {
        if (prev_c == "-") {
          continue;
        } else {
          c = "-";
          dst_buf[loc] = c;
        }
      } else if (sCHOONPUS.count(c)) {
        if (prev_c == "ー") { // zenkaku
          continue;
        } else {
          c = "ー";
          dst_buf[loc] = c;
        }
      } else if (sTILDES.count(c)) {
        if (option.tilde == NormalizationOption::TildeMode::Ignore) {
          // pass
        } else if (option.tilde == NormalizationOption::TildeMode::Normalize) {
          c = "~";
        } else if (option.tilde == NormalizationOption::TildeMode::Zenkaku) {
          c = "〜";
        } else {
          continue;
        }

        dst_buf[loc] = c;

      } else {

        if (sASCII.count(c)) {
          c = sASCII.at(c);
        } else if (sDIGIT.count(c)) {
          c = sDIGIT.at(c);
        } else if (sKANA.count(c)) {
          c = sKANA.at(c);
        } else {
          // Additional unicode normalizations
          if (option.parenthesized_ideographs && sParenthesizedIdeographs.count(c)) {
            c = sParenthesizedIdeographs.at(c);
          }
        }


        if ((c == "ﾞ") && (sKANA_TEN.count(prev_c))) {
          if (loc == 0) {
            //std::cerr << "b loc = 0\n";
            return std::string();
          }
          loc--;
          c = sKANA_TEN.at(prev_c);
        } else if ((c == "ﾟ") && (sKANA_MARU.count(prev_c))) {
          if (loc == 0) {
            //std::cerr << "c loc = 0\n";
            return std::string();
          }
          loc--;
          c = sKANA_MARU.at(prev_c);
        }

        //std::cout << "latin_space " << latin_space << "\n";
        //std::cout << "is_cjk_char " << detail::is_cjk_char(c) << "\n";

        // TODO: allow all non-latin char?
        if (latin_space && detail::is_cjk_char(c) && option.remove_space) {
          //std::cout << "latin space: c = " << c << "\n";
          if (loc == 0) {
            return std::string();
          }
          loc--;
        }

        latin_space = false;
        dst_buf[loc] = c;
      }
    }

    prev_c = c;
    loc++;
  }

  if (loc == 0) {
    // This should not happen though.
    //std::cerr << "e loc = 0\n";
    return std::string();
  }

  if (dst_buf[loc-1] == " ") {
    loc--;
  }

  std::string dst_str;
  // simply concat chars.
  for (size_t i = 0; i < loc; i++) {
    dst_str += dst_buf[i];
  }

  if (option.repeat > 0) {
    return detail::shorten_repeat(dst_str, option.repeat, option.max_repeat_substr_len);
  }

  return dst_str;
}

#if 0 // TODO
// replace all digits to a placeholder character
std::string normalize_for_dedup(const std::string& str,
                      const DedupNormalizationOption dedup_option) {

  NormalizationOption option;
  option.tilde = NormalizationOption::TildeMode::Remove;
  option.remove_space = true;
  option.repeat = 8;
  option.max_repeat = 8;
  option.parenthesized_ideographs = true;


  if (str.empty()) {
    return std::string();
  }

  if (str.size() > option.max_tokens) {
    return std::string();
  }

  ///
  /// Decompose input string into UTF8 char list.
  ///
  uint64_t sz = str.size();
  std::vector<std::string> utf8_chars;

  for (size_t i = 0; i <= sz;) {
    int len=0;
    std::string s = detail::extract_utf8_char(str, uint32_t(i), len);
    if (len == 0) {
      // invalid char
      break;
    }

    i += uint64_t(len);
    utf8_chars.push_back(s);
  }

  std::vector<std::string> dst_buf;
  // normalized text should not exceed input length.
  dst_buf.resize(utf8_chars.size());

  std::string prev_c = "\0";
  bool latin_space = false;
  uint64_t loc = 0;

  for (size_t i = 0 ; i  < utf8_chars.size(); i++) {
    std::string c = utf8_chars[i];

    if (sSPACE.count(c)) {
      c = " ";
      //std::cout << "c is space: prev_c = " << prev_c << ", utf8 code " << detail::utf8_code(prev_c) << "\n";
      if (((prev_c == " ") || detail::is_cjk_char(prev_c)) && option.remove_space) {
        continue;
      } else if ((prev_c != "*") && (loc > 0) && (detail::utf8_code(prev_c) < 128)) {
        //std::cout << "latin space\n";
        latin_space = true;
        if (loc >= dst_buf.size()) {
          // ???
          return std::string();
        }
        dst_buf[loc] = c;
      } else if (option.remove_space) {
        if (loc == 0) {
          prev_c = c;
          continue;
        } else {
          loc--;
        }
      } else {
        if (loc >= dst_buf.size()) {
          // ???
          return std::string();
        }
        dst_buf[loc] = c;
      }
    } else {
      if (sHYPHENS.count(c)) {
        if (prev_c == "-") {
          continue;
        } else {
          c = "-";
          dst_buf[loc] = c;
        }
      } else if (sCHOONPUS.count(c)) {
        if (prev_c == "ー") { // zenkaku
          continue;
        } else {
          c = "ー";
          dst_buf[loc] = c;
        }
      } else if (sTILDES.count(c)) {
        if (option.tilde == NormalizationOption::TildeMode::Ignore) {
          // pass
        } else if (option.tilde == NormalizationOption::TildeMode::Normalize) {
          c = "~";
        } else if (option.tilde == NormalizationOption::TildeMode::Zenkaku) {
          c = "〜";
        } else {
          continue;
        }

        dst_buf[loc] = c;

      } else {

        if (sASCII.count(c)) {
          c = sASCII.at(c);
        } else if (sDIGIT.count(c)) {
          c = sDIGIT.at(c);
        } else if (sKANA.count(c)) {
          c = sKANA.at(c);
        } else {
          // Additional unicode normalizations
          if (option.parenthesized_ideographs && sParenthesizedIdeographs.count(c)) {
            c = sParenthesizedIdeographs.at(c);
          }
        }


        if ((c == "ﾞ") && (sKANA_TEN.count(prev_c))) {
          if (loc == 0) {
            //std::cerr << "b loc = 0\n";
            return std::string();
          }
          loc--;
          c = sKANA_TEN.at(prev_c);
        } else if ((c == "ﾟ") && (sKANA_MARU.count(prev_c))) {
          if (loc == 0) {
            //std::cerr << "c loc = 0\n";
            return std::string();
          }
          loc--;
          c = sKANA_MARU.at(prev_c);
        }

        //std::cout << "latin_space " << latin_space << "\n";
        //std::cout << "is_cjk_char " << detail::is_cjk_char(c) << "\n";

        // TODO: allow all non-latin char?
        if (latin_space && detail::is_cjk_char(c) && option.remove_space) {
          //std::cout << "latin space: c = " << c << "\n";
          if (loc == 0) {
            return std::string();
          }
          loc--;
        }

        latin_space = false;
        dst_buf[loc] = c;
      }
    }

    prev_c = c;
    loc++;
  }

  if (loc == 0) {
    // This should not happen though.
    //std::cerr << "e loc = 0\n";
    return std::string();
  }

  if (dst_buf[loc-1] == " ") {
    loc--;
  }

  std::string dst_str;
  // simply concat chars.
  for (size_t i = 0; i < loc; i++) {
    dst_str += dst_buf[i];
  }

  if (option.repeat > 0) {
    return detail::shorten_repeat(dst_str, option.repeat, option.max_repeat);
  }

  return dst_str;
}
#endif

const std::unordered_set<std::string> &get_unicode_puncts() {
  return sUNICODE_PUNCT;
}

std::unordered_set<std::string> get_digits() {

  std::unordered_set<std::string> dst;

  for (const auto &it : sDIGIT) {
    dst.insert(it.first);
    dst.insert(it.second);
  }

  return dst;
}

std::unordered_set<std::string> get_digits_and_parentized_ideographs() {

  std::unordered_set<std::string> dst = get_digits();

  for (const auto &it : sParenthesizedIdeographs) {
    dst.insert(it.first);
    dst.insert(it.second);
  }

  return dst;

}

}  // namespace jpnormalizer

#endif  // JP_NORMALIZER_IMPLEMENTATION
