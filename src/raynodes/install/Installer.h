#ifndef INSTALLER_H
#define INSTALLER_H

#include "shared/fwd.h"

// Currently only used to install the VC Runtime on windows

struct EXPORT Installer {
  static bool NeedsInstallation();
  static bool Install();
};

#endif  //INSTALLER_H