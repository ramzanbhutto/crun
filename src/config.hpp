#pragma once

#include<string>
#include<vector>
#include<sys/types.h>

struct Config{
  uid_t uid=0;
  std::string hostname;
  std::string rootfs;  // -m, not used until mounts.cpp lands
  std::vector<std::string> argv;  // command + args to exec inside
};
