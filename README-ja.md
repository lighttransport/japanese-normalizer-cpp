# Header-only Japanese text normalizer in C++11

C++11 記述のシンプルな日本語テキスト正規化ライブラリです.
正規化のルールは neologdn を参考にしています.
unicodedata.normalize や sentencepiece normalizer のように完全な機能は有しておりませんが,
llama.cpp https://github.com/ggerganov/llama.cpp など LLM アプリに組み込んで使うなどのユースケースでは十分な機能を有していると思います.

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

std::string text = "...";
std::string normalized_text = jpnormalizer::normalize(text);

// Control normalization using NormalizationOptions
jpnormalizer::NormaliationOptions options;

options.repeat = 5;

std::string normalized_text = jpnormalizer::normalize(text, options);
```

## Limitation

1 文章(string) 2GB までになります.

You can set this limit in NormalizationOptions;

T.B.W.

## Security

`jp_normalizer.hh` は LLVM fuzzer でテストしています.
現状, 範囲外アクセスなどは見つかっていません.

## TODO

* [ ] More Enclosed CJK Letters and Months.
* [ ] wstring(WideChar) support in Windows
* [ ] UTF-16 text?(e.g. UNICODE UTF-16LE text in Windows)

## License

Apache 2.0 license

### Thrid party licenses

neologdn: Apache 2.0 License : https://github.com/ikegami-yukino/neologdn
