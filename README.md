# Header-only Japanese text normalizer in C++11

Simple Japanese text normalizer written in portable C++11, based on neologdn.
https://github.com/ikegami-yukino/neologdn

Its good to embed Japanese normalization feature to your LLM(Large Language Model) apps.
(e.g. llama.cpp https://github.com/ggerganov/llama.cpp )

C++ STL のみ(and no std::regex) で日本語を正規化するシンプルなライブラリ [README-ja.md](README-ja.md)

## Rules

https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja

## Requirements

* UTF-8 text
  * UTF-16 and UTF-32 are not supported at the moment.

## Usage

```
// Define this only in one **.cc
#define JP_NORMALIZER_IMPLEMENTATION
#include "jp_normalizer.hh"

std::string text = "ﾊﾝｶｸｶﾅ";
std::string normalized_text = jpnormalizer::normalize(text);
// => "ハンカクカナ"

// Control normalization using NormalizationOptions
jpnormalizer::NormaliationOptions options;

options.repeat = 5;

std::string normalized_text = jpnormalizer::normalize(text, options);
```

## Limitation

Default up to 1GB tokens(~ 3GB in UTF-8 Japanase character).
You can set this limit in NormalizationOptions;

## Security

`jp_normalizer.hh` is tested on LLVM fuzzer.
No security issue(segfault, OOM) observed at the moment.

## TODO

* [x] Implement shorten repeat feature.
* [ ] More Enclosed CJK Letters and Months.
* [ ] wstring(WideChar) support in Windows
* [ ] UTF-16 text?(e.g. UNICODE UTF-16LE text in Windows)

## License

Apache 2.0 license

### Thrid party licenses

neologdn: Apache 2.0 License : https://github.com/ikegami-yukino/neologdn
