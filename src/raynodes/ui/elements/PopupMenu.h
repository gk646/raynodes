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

#ifndef TEXTPOPUP_H
#define TEXTPOPUP_H

#include "shared/fwd.h"

// Return nullptr for a valid str else the fail reason
using ValidationFunc = const char* (*)(EditorContext&, const char*);
using CustomDraw = bool (*)(EditorContext& ec, const Rectangle& r);

struct EXPORT PopupMenu {
  static const char* InputText(EditorContext& ec, Rectangle& scaled, TextField& input, ValidationFunc func,
                               const char* header);

  static const char* InputTextEx(EditorContext& ec, Rectangle& scaled, TextField& input, ValidationFunc func,
                                          const char* header, CustomDraw draw);
};

#endif  //TEXTPOPUP_H