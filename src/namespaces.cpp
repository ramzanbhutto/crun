#include<signal.h>
#include<cstdlib>
#include<sched.h>
#include "namespaces.hpp"
#include "child.hpp"
#include "util.hpp"

namespace ns{
  Spawned spawn_contained(Config& config){
    char* stack= static_cast<char*>(malloc(STACK_SIZE));
    if(!stack) die("allocating child stack");
    int flags= CLONE_NEWNS    // own mount table
              | CLONE_NEWPID   // own process tree, child becomes pid 1
              | CLONE_NEWUTS   // own hostname/domainname
              | CLONE_NEWIPC   // own IPC (message queues, semaphores...)
              | CLONE_NEWNET   // own network stack, no interfaces yet
              | SIGCHLD;       // so waitpid() in main() actually works

    // stack grows down on x86_64, so clone() wants a pointer to the *top* of the block, not the start
  
    pid_t pid= clone(child_entry, stack+STACK_SIZE, flags, &config);
    if(pid==-1){
      free(stack);
      die("clone");
    }
    
    return {pid, stack};
  }
}
