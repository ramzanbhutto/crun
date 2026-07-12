#pragma once

#include<filesystem>
#include "config.hpp"

namespace mounts{
  /*
    Swaps the process's view of / from the host filesystem to config.rootfs, and remounts /proc so ps/top/getpid() agree with reality inside the new PID namespace.
 */
  void pivot_root(Config& config);
}
