#pragma once

#include "config.hpp"

namespace ns{
 constexpr std::size_t STACK_SIZE= 1024*1024; // 1Mb - execve() replaces this anyway, its only needed until then

 struct Spawned{
   pid_t pid;
   char* stack; // caller owns this, i used free() after waitpid()
 };
 
 Spawned spawn_contained(Config& config);
}
