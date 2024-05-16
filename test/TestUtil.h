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

#ifndef REGISTERNODES_H
#define REGISTERNODES_H

#include "shared/fwd.h"
#include "application/EditorContext.h"
#include "plugin/RegisterInterface.h"
#include <cxutil/cxio.h>
#include "../plugins/BuiltIns/components/TextFieldC.h"

namespace {
void registerNodes(EditorContext& ec) {
  PluginContainer pc{nullptr, "_Dummy_", nullptr};
  NodeRegister nr{ec, pc};
  ComponentRegister cr{ec, pc};
  cr.registerComponent<TextFieldC<>>("TextField");
  nr.registerNode("DummyN", {{"DummyC", "TextField"}});
}

}  // namespace

namespace TestUtil {
// Returns a basic context with a component and a node registered
inline EditorContext getBasicContext() {
  EditorContext ec{0, nullptr};
  registerNodes(ec);
  return ec;
}
// Call this to use relative and short paths inside the test dir
inline void SetupCWD() {
  char buff[256];
  snprintf(buff, 256, "%s/../test", GetWorkingDirectory());
  if (!ChangeDirectory(buff)) {
    fprintf(stderr, "Failed to change CWD\n");
  }
}
}  // namespace TestUtil
#endif  //REGISTERNODES_H