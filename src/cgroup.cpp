#include<fstream>
#include<filesystem>
#include<unistd.h>

#include "cgroup.hpp"
#include "util.hpp"

namespace fs= std::filesystem;

namespace{
  constexpr const char* CGROUP_ROOT= "/sys/fs/cgroup"; // v2 single unified hierarchy - one tree, not per-controller like v1
}

Cgroup::Cgroup(const std::string& name, const Config& config) : path(std::string(CGROUP_ROOT) + "/" + name){
  log_step("creating cgroup "+ path);
  std::error_code ec;
  fs::create_directory(path,ec); // mkdir - the kernel auto-populates it with controller files the instant it exists
  if(ec) die("creating cgroup dir: "+ec.message());

  write_file(std::string(CGROUP_ROOT)+ "/cgroup.subtree_control", "+memory +pids +cpu");   // cgroups v2 requires controllers to be explicitly enabled on the PARENT(cgroup.subtree_control file) before a child cgroup is allowed to set limits for them
  write_file(path+ "/memory.max", std::to_string(config.mem_limit_mb*1024*1024)); // hard memory ceiling in bytes
  write_file(path+ "/pids.max", std::to_string(config.pids_max));  // max number of tasks/threads this cgroup can ever hold

  /*
   cpu.max format is "<quota> <period>", both microseconds. period 100000us (100ms) is the kernel default. quota is our slice of that period.
 */

  long quota= (config.cpu_quota_pct * 100000L) / 100; // convert our percentage into a microsecond quota of the 100ms period
  write_file(path + "/cpu.max", std::to_string(quota) + " 100000");
  
  log_step("joining cgroup, pid " + std::to_string(getpid()));
  write_file(path + "/cgroup.procs", std::to_string(getpid())); // adds THIS process (main(), the parent) - child inherits membership via clone
  
  active=true; // only flip this once every step above has actually succeeded
}

Cgroup::~Cgroup(){
  if(!active) return; // never became a real cgroup (or already cleaned up) - nothing to undo
  /*
   move back to the root cgroup first - a non-empty cgroup refuses to rmdir, so we have to vacate before we can clean up
  */

  std::ofstream root_procs(std::string(CGROUP_ROOT) + "/cgroup.procs");
  if(root_procs){
    root_procs<<getpid();
    root_procs.flush(); // must land before fs::remove() below or the kernel still sees us as a member -> EBUSY on rmdir
  }

  std::error_code ec;
  fs::remove(path, ec);  // rmdir - only succeeds now that we have actually left
  if(ec) std::cerr<<"=> warning: couldn't remove cgroup "<<path<<": "<<ec.message()<<std::endl;
}

void Cgroup::write_file(const std::string& file, const std::string& value){
  std::ofstream f1(file);
  if(!f1) die("opening "+file); // e.g. controller not delegated or file doesn't exist
  f1<<value;
  f1.flush();  // force the write out now rather than relying on implicit destructor timing later
  if(!f1) die("writing to "+file);  // e.g. value rejected by the kernel (bad format, over some hard limit, etc)
}
