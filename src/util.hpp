#pragma once

#include<iostream>
#include<string>
#include<cerrno>
#include<cstring>
#include<cstdlib>

inline void die(const std::string& what){
  std::cerr<<"=> "<<what<<" failed: "<<std::strerror(errno)<<"\n";
  std::exit(1);
}

inline void log_step(const std::string& msg){
  std::cerr<<"=> "<<msg<<"\n";
}
