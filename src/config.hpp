#pragma once

#include<string>
#include<vector>
#include<sys/types.h>

struct Config{
  uid_t uid=0;
  std::string hostname;
  std::string rootfs; 
  std::vector<std::string> argv;  // command + args to exec inside
  
  long mem_limit_mb= 512;
  int pids_max= 64;
  int cpu_quota_pct= 50;
};
