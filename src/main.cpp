#include<iostream>
#include<cstdlib>
#include<sys/wait.h>
#include<getopt.h>
#include "config.hpp"
#include "namespaces.hpp"
#include "util.hpp"

namespace{
  void usage(const char* prog){
    std::cerr<<"usage: "<<prog<<" -m <rootfs> -u <uid> -c <cmd> [args...]\n";
    std::exit(1);
  }

  std::string make_hostname(){
    return " crun-"+std::to_string(getpid()); 
  }

}

int main(int argc, char**argv){
  Config config;
  int opt;
  while((opt=getopt(argc,argv,"m:u:c:"))!=-1){
    switch(opt){
      case 'm':
           config.rootfs= optarg;
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
  config.hostname= make_hostname();

  auto spawned= ns::spawn_contained(config);
  log_step("cloned child, pid "+ std::to_string(spawned.pid));

  int status=0;
  if(waitpid(spawned.pid, &status, 0)==1) die("waitpid");

  free(spawned.stack);

  return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
