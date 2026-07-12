#pragma once

#include<string>
#include "config.hpp"

/* 
  // Wraps one cgroup v2 directory under /sys/fs/cgroup. RAII: constructor creates the dir + writes limits + joins this process to it. destructor moves the process back out and rmdir's it. Since cgroup membership is inherited across clone()/fork(), joining the PARENT here means the child lands in the same cgroup automatically the instant clone() runs.
*/ 

class Cgroup{
private:
  std::string path;
  bool active;
  void write_file(const std::string& file, const std::string& value); // small helper, one open+write+flush, used for every limit file below

public:
  Cgroup() : active(false) {}
  Cgroup(const std::string& name, const Config& config);
 ~Cgroup();

  // owns a real filesystem resource (a live cgroup dir) - copying would let two objects
  Cgroup(const Cgroup&)= delete;
  Cgroup& operator=(const Cgroup&)= delete;
};
