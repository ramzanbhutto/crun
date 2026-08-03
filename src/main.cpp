#include<iostream>
#include<cstdlib>
#include<sys/wait.h>
#include<filesystem>
#include<unistd.h>
#include<getopt.h>
#include "config.hpp"
#include "namespaces.hpp"
#include "util.hpp"
#include "cgroup.hpp"

namespace{
  void usage(const char* prog){
    std::cerr<<"usage: sudo "<<prog<<" -m <rootfs> -u <uid> -c <cmd> [args...]\n";
    std::exit(1);
  }

}

int main(int argc, char**argv){
  Config config;
  int opt;
  while((opt=getopt(argc,argv,"m:u:c:"))!=-1){
    switch(opt){
      case 'm':
           config.rootfs= std::filesystem::absolute(optarg).string();
           break;
      case 'u':
           config.uid= static_cast<uid_t>(std::stoi(optarg));
           break;
      case 'c':
           config.argv.push_back(optarg);
           for(int i=optind; i<argc; i++)  config.argv.push_back(argv[i]);
           optind= argc;
           break;
      default:
           usage(argv[0]);
    }
  }

  if(config.argv.empty()) usage(argv[0]);

  std::string cg_name= "crun-" + std::to_string(getpid());
  Cgroup cgroup(cg_name,config); // joins us to it now. child inherits it via clone()

  
  auto spawned= ns::spawn_contained(config);
  log_step("cloned child, pid "+ std::to_string(spawned.pid));

  int status=0;
  if(waitpid(spawned.pid, &status, 0)==pid_t(-1)) die("waitpid");

  free(spawned.stack);

  // cgroup goes out of scope here at end of main() - destructor runs automatically, moves us out, rmdir's the cgroup. No manual cleanup call is needed
  return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
