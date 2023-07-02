// SPDX-License-Identifier: Apache 2.0
// Copyright 2023 - Present, Light Transport Entertainement Inc.
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
  uint32_t repeat{1};
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

static std::unordered_set<std::string> sHIPHENS = {"˗", "֊", "‐", "‑", "‒",
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

static void swap2(unsigned short* val) {
  unsigned short tmp = *val;
  unsigned char* dst = reinterpret_cast<unsigned char*>(val);
  unsigned char* src = reinterpret_cast<unsigned char*>(&tmp);

  dst[0] = src[1];
  dst[1] = src[0];
}

static void swap4(unsigned int* val) {
  unsigned int tmp = *val;
  unsigned char* dst = reinterpret_cast<unsigned char*>(val);
  unsigned char* src = reinterpret_cast<unsigned char*>(&tmp);

  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];
}

static inline void swap4(int* val) {
  int tmp = *val;
  unsigned char* dst = reinterpret_cast<unsigned char*>(val);
  unsigned char* src = reinterpret_cast<unsigned char*>(&tmp);

  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];
}

static inline void swap8(uint64_t* val) {
  uint64_t tmp = (*val);
  unsigned char* dst = reinterpret_cast<unsigned char*>(val);
  unsigned char* src = reinterpret_cast<unsigned char*>(&tmp);

  dst[0] = src[7];
  dst[1] = src[6];
  dst[2] = src[5];
  dst[3] = src[4];
  dst[4] = src[3];
  dst[5] = src[2];
  dst[6] = src[1];
  dst[7] = src[0];
}

static inline void swap8(int64_t* val) {
  int64_t tmp = (*val);
  unsigned char* dst = reinterpret_cast<unsigned char*>(val);
  unsigned char* src = reinterpret_cast<unsigned char*>(&tmp);

  dst[0] = src[7];
  dst[1] = src[6];
  dst[2] = src[5];
  dst[3] = src[4];
  dst[4] = src[3];
  dst[5] = src[2];
  dst[6] = src[1];
  dst[7] = src[0];
}

// For unaligned read

static void cpy2(unsigned short* dst_val, const unsigned short* src_val) {
  unsigned char* dst = reinterpret_cast<unsigned char*>(dst_val);
  const unsigned char* src = reinterpret_cast<const unsigned char*>(src_val);

  dst[0] = src[0];
  dst[1] = src[1];
}

static void cpy2(short* dst_val, const short* src_val) {
  unsigned char* dst = reinterpret_cast<unsigned char*>(dst_val);
  const unsigned char* src = reinterpret_cast<const unsigned char*>(src_val);

  dst[0] = src[0];
  dst[1] = src[1];
}

static void cpy4(unsigned int* dst_val, const unsigned int* src_val) {
  unsigned char* dst = reinterpret_cast<unsigned char*>(dst_val);
  const unsigned char* src = reinterpret_cast<const unsigned char*>(src_val);

  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
}

static void cpy4(int* dst_val, const int* src_val) {
  unsigned char* dst = reinterpret_cast<unsigned char*>(dst_val);
  const unsigned char* src = reinterpret_cast<const unsigned char*>(src_val);

  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
}

static void cpy8(uint64_t* dst_val, const uint64_t* src_val) {
  unsigned char* dst = reinterpret_cast<unsigned char*>(dst_val);
  const unsigned char* src = reinterpret_cast<const unsigned char*>(src_val);

  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
  dst[4] = src[4];
  dst[5] = src[5];
  dst[6] = src[6];
  dst[7] = src[7];
}

static void cpy8(int64_t* dst_val, const int64_t* src_val) {
  unsigned char* dst = reinterpret_cast<unsigned char*>(dst_val);
  const unsigned char* src = reinterpret_cast<const unsigned char*>(src_val);

  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
  dst[4] = src[4];
  dst[5] = src[5];
  dst[6] = src[6];
  dst[7] = src[7];
}

///
/// Simple stream reader
///
class StreamReader {
 public:
  explicit StreamReader(const uint8_t* binary, const size_t length,
                        const bool swap_endian)
      : binary_(binary), length_(length), swap_endian_(swap_endian), idx_(0) {
    (void)pad_;
  }

  bool seek_set(const uint64_t offset) const {
    if (offset > length_) {
      return false;
    }

    idx_ = offset;
    return true;
  }

  bool seek_from_currect(const int64_t offset) const {
    if ((int64_t(idx_) + offset) < 0) {
      return false;
    }

    if (size_t((int64_t(idx_) + offset)) > length_) {
      return false;
    }

    idx_ = size_t(int64_t(idx_) + offset);
    return true;
  }

  size_t read(const size_t n, const uint64_t dst_len,
              unsigned char* dst) const {
    size_t len = n;
    if ((idx_ + len) > length_) {
      len = length_ - idx_;
    }

    if (len > 0) {
      if (dst_len < len) {
        // dst does not have enough space. return 0 for a while.
        return 0;
      }

      memcpy(dst, &binary_[idx_], len);
      idx_ += len;
      return len;

    } else {
      return 0;
    }
  }

  bool read1(unsigned char* ret) const {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const unsigned char val = binary_[idx_];

    (*ret) = val;
    idx_ += 1;

    return true;
  }

  bool read1(char* ret) const {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const char val = static_cast<const char>(binary_[idx_]);

    (*ret) = val;
    idx_ += 1;

    return true;
  }

  bool read2(unsigned short* ret) const {
    if ((idx_ + 2) > length_) {
      return false;
    }

    unsigned short val = 0;
    cpy2(&val, reinterpret_cast<const unsigned short*>(&binary_[idx_]));

    if (swap_endian_) {
      swap2(&val);
    }

    (*ret) = val;
    idx_ += 2;

    return true;
  }

  bool read2(short* ret) const {
    if ((idx_ + 2) > length_) {
      return false;
    }

    short val = 0;
    cpy2(&val, reinterpret_cast<const short*>(&binary_[idx_]));

    if (swap_endian_) {
      swap2(reinterpret_cast<unsigned short*>(&val));
    }

    (*ret) = val;
    idx_ += 2;

    return true;
  }

  bool read4(unsigned int* ret) const {
    if ((idx_ + 4) > length_) {
      return false;
    }

    unsigned int val = 0;
    cpy4(&val, reinterpret_cast<const unsigned int*>(&binary_[idx_]));

    if (swap_endian_) {
      swap4(&val);
    }

    (*ret) = val;
    idx_ += 4;

    return true;
  }

  bool read4(int* ret) const {
    if ((idx_ + 4) > length_) {
      return false;
    }

    int val = 0;
    cpy4(&val, reinterpret_cast<const int*>(&binary_[idx_]));

    if (swap_endian_) {
      swap4(&val);
    }

    (*ret) = val;
    idx_ += 4;

    return true;
  }

  bool read8(uint64_t* ret) const {
    if ((idx_ + 8) > length_) {
      return false;
    }

    uint64_t val = 0;
    cpy8(&val, reinterpret_cast<const uint64_t*>(&binary_[idx_]));

    if (swap_endian_) {
      swap8(&val);
    }

    (*ret) = val;
    idx_ += 8;

    return true;
  }

  bool read8(int64_t* ret) const {
    if ((idx_ + 8) > length_) {
      return false;
    }

    int64_t val = 0;
    cpy8(&val, reinterpret_cast<const int64_t*>(&binary_[idx_]));

    if (swap_endian_) {
      swap8(&val);
    }

    (*ret) = val;
    idx_ += 8;

    return true;
  }

  //
  // Returns a memory address. The begining of address is computed in
  // relative(based on current seek pos). This function is useful when you just
  // want to access the content in read-only mode.
  //
  // Note that the function does not change seek position after the call.
  // This function does the bound check.
  //
  // @param[in] offset Extra byte offset. 0 = use current seek position.
  // @param[in] length Byte length to map.
  //
  // @return nullptr when failed to map address.
  //
  const uint8_t* map_addr(size_t offset, const size_t length) {
    if (length == 0) {
      return nullptr;
    }

    if ((idx_ + offset) > length_) {
      return nullptr;
    }

    if ((idx_ + offset + length) > length_) {
      return nullptr;
    }

    return &binary_[idx_ + offset];
  }

  //
  // Returns a memory address. The begining of address is specified by
  // absolute(ignores current seek pos). This function is useful when you just
  // want to access the content in read-only mode.
  //
  // Note that the function does not change seek position after the call.
  // This function does the bound check.
  //
  // @param[in] pos Absolute position in bytes.
  // @param[in] length Byte length to map.
  //
  // @return nullptr when failed to map address.
  //
  const uint8_t* map_abs_addr(size_t pos, const size_t length) {
    if (length == 0) {
      return nullptr;
    }

    if (pos > length_) {
      return nullptr;
    }

    if ((pos + length) > length_) {
      return nullptr;
    }

    return &binary_[pos];
  }

  size_t tell() const { return idx_; }
  bool eof() const { return idx_ >= length_; }

  const uint8_t* data() const { return binary_; }

  bool swap_endian() const { return swap_endian_; }

  size_t size() const { return length_; }

 private:
  const uint8_t* binary_;
  const size_t length_;
  bool swap_endian_;
  char pad_[7];
  mutable uint64_t idx_;
};

// TODO
// bool shorten_repeat(const detail::StreamReader &sr, uint32_t repeat_num,

#if 0
bool normalize_str(const detail::StreamReader &sr, const NormalizationOption option, std::string *result) {

  std::string dst_str;

  //char_codes = itertools.chain(
  //    range(19968, 40960),  # CJK UNIFIED IDEOGRAPHS
  //    range(12352, 12448),  # HIRAGANA
  //    range(12448, 12544),  # KATAKANA
  //    range(12289, 12352),  # CJK SYMBOLS AND PUNCTUATION
  //    range(65280, 65520)   # HALFWIDTH AND FULLWIDTH FORMS
  //)

  std::string prev_c = "\0";
  std::string curr_c;

  while (!sr.eof()) {

    if (!sr.read1(&curr_c)) {
      return false;
    }

    if (curr_c == ' ')

  }

  (*result) = dst_str;
  return true;
}
#endif

}  // namespace detail

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

std::string normalize(const std::string& str,
                      const NormalizationOption option) {
  (void)option;

  if (str.empty()) {
    return std::string();
  }

  uint64_t sz = str.size();
  std::cout << "buf len = " << sz << "\n";

  for (size_t i = 0; i <= sz;) {
    int len=0;
    std::string s = extract_utf8_char(str, uint32_t(i), len);
    if (len == 0) {
      // invalid char
      break;
    }

    i += uint64_t(len);
    std::cout << i << ", " << len << ", " << s << "\n";
  }

  std::string dst_str;

#if 0
  // TODO: endian
  detail::StreamReader sr = detail::StreamReader(reinterpret_cast<const uint8_t *>(&str[0]), sz, /* endian_swap */false);


  if (!detail::normalize_str(sr, option, &dst_str)) {
    return std::string();
  }
#endif

  return dst_str;
}

}  // namespace jpnormalizer

#endif  // JP_NORMALIZER_IMPLEMENTATION
