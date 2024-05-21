#include "Installer.h"

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#endif

#include <filesystem>

bool Installer::NeedsInstallation() {
#if defined(_WIN32)
  HKEY hKey;
  // Check 64 bit
  const auto lRes64 = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64"), 0, KEY_READ, &hKey);
  if (lRes64 == ERROR_SUCCESS) {
    RegCloseKey(hKey);
    return false;
  }
  // Check 32 bit
  const auto lRes32 = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x86"), 0, KEY_READ, &hKey);
  if (lRes32 == ERROR_SUCCESS) {
    RegCloseKey(hKey);
    return false;
  }
  return true;

#endif
  return false;
}

bool Installer::Install() {
#if defined(_WIN32)
  system("VC_redist.x64.exe /norestart");
  return std::filesystem::remove("VC_redist.x64.exe");
#endif
  return false;
}