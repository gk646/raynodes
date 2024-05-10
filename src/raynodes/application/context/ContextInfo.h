// Copyright (c) 2024 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINFO_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINFO_H_

struct Info {
  static constexpr auto applicationName = "raynodes";
  static constexpr auto fileEnding = ".rn";
  static constexpr const char* fileFilter[1] = {"*.rn"};
  static constexpr auto fileDescription = "raynodes save (.rn)";
  static constexpr auto wikiLink = "https://github.com/gk646/raynodes";
  static constexpr auto github = "https://github.com/gk646/raynodes";
  static constexpr auto about = "Copyright #226# 2024 gk646. MIT License";
  static constexpr int majorVersion = 0;
  static constexpr int middleVersion = 9;
  static constexpr int minorVersion = 9;

  static const char* getVersion(EditorContext& ec);
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINFO_H_