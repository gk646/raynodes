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

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

#include <cstdio>
#include "plugin/PluginLoader.h"
#include "plugin/PluginInterface.h"

//Hide all the ugly #ifdefs

namespace {
const char* getLastError(char* buffer, int size) {
#if defined(_WIN32)
  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, size, nullptr);

  return buffer;
#else
  return dlerror();
#endif
}
}  // namespace

void PluginContainer::free() {
  if (handle == nullptr) return;  // Guard against multiple frees
#if defined(_WIN32)
  FreeLibrary(static_cast<HMODULE>(handle));
#else
  dlclose(handle);
#endif
  handle = nullptr;  // Prevent double-free
}
PluginContainer PluginLoader::GetPluginInstance(const char* path, const char* funcName) {
  char buff[64]{};
  void* handle;
#if defined(_WIN32)
  handle = LoadLibraryA(path);
#else
  handle = dlopen(path, RTLD_LAZY);
#endif

  if (!handle) {
    fprintf(stderr, "Filed to load plugin %s", getLastError(buff, 64));
    return {nullptr, nullptr};
  }

  void* symbol;
#if defined(_WIN32)
  symbol = (void*)GetProcAddress(static_cast<HMODULE>(handle), funcName);
#else
  symbol = dlsym(handle, funcName);
#endif

  if (!symbol) {
    fprintf(stderr, "Filed to load symbol %s", getLastError(buff, 64));
#if defined(_WIN32)
    FreeLibrary(static_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
    return {nullptr, nullptr};
  }
  return {handle, nullptr, reinterpret_cast<RaynodesPluginI* (*)()>(symbol)()};
}