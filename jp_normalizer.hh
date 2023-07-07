// SPDX-License-Identifier: Apache 2.0
// Copyright 2023 - Present, Light Transport Entertainement Inc.
//
// Assume this file is encoded in UTF-8
#pragma once

#include <cstdint>
#include <string>

namespace jpnormalizer {

struct NormalizationOption {
  enum class TildeMode {
    Remove,     // remove tilde
    Ignore,     // ignore(do nothing) for tilde char.
    Normzlize,  // convert to ASCII '~'
    Zenkaku,    // convert to Zenkaku '〜'
  };

  bool remove_space{true};
  uint32_t repeat{0};
  uint32_t max_repeat{8};
  TildeMode tilde{TildeMode::Remove};
};

std::string normalize(const std::string& str,
                      const NormalizationOption option = NormalizationOption());

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

#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace detail {

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

  uint32_t code = 0;
  if (s.size() == 1) {
    code = s[0] & 0xff;
  } else if (s.size() == 2) {
    code = ((uint32_t(s[0]) & 0xff) << 1) | (s[1] & 0xff);
  } else if (s.size() == 3) {
    code = ((uint32_t(s[0]) & 0xff) << 2) | ((uint32_t(s[1]) & 0xff) << 1) | (s[2] & 0xff);
  } else {
    code = ((uint32_t(s[0]) & 0xff) << 3) | ((uint32_t(s[1]) & 0xff) << 2) | ((uint32_t(s[2]) & 0xff) << 1) | (s[3] & 0xff);
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

}  // namespace detail

std::string normalize(const std::string& str,
                      const NormalizationOption option) {

  if (str.empty()) {
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
      if (((prev_c == " ") || detail::is_cjk_char(prev_c)) && option.remove_space) {
        continue;
      } else if ((prev_c != "*") && (loc > 0) && (detail::utf8_code(prev_c) < 128)) {
        latin_space = true;
        if (loc >= dst_buf.size()) {
          // ???
          return std::string();
        }
        dst_buf[loc] = c;
      } else if (option.remove_space) {
        if (loc == 0) {
          // ???
          return std::string();
        }
        loc--;
      } else {
        if (loc >= dst_buf.size()) {
          // ???
          return std::string();
        }
        dst_buf[loc] = c;
      }
    } else {
      if (sHYPHENS.count(c)) {
      } else if (sCHOONPUS.count(c)) {
      } else if (sTILDES.count(c)) {
      } else {
      }

    }

    prev_c = c;
    loc++;
  }

  std::string dst_str;
  // simply concat chars.
  for (size_t i = 0; i < loc; i++) {
    dst_str += dst_buf[i];
  }

  return dst_str;
}

}  // namespace jpnormalizer

#endif  // JP_NORMALIZER_IMPLEMENTATION
