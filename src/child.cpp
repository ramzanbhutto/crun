#include<vector>
#include<unistd.h>
#include "child.hpp"
#include "util.hpp"

std::string make_hostname(){
  return " crun-" + std::to_string(getpid()); 
}

int child_entry(void* arg){
  auto* config= static_cast<Config*>(arg);
  if(config->hostname.empty()) config->hostname= make_hostname();
  log_step("setting hostname to" +config->hostname);
  if(sethostname(config->hostname.c_str(), config->hostname.size())==-1) die("sethostname");

  std::vector<char*> argv;  // execvp accepts a null-terminated char*[] (c-type), not a vector<string>
  for(auto& a:config->argv) argv.push_back(a.data());
  argv.push_back(nullptr);

  log_step("exec'ing "+ config->argv[0]);
  execvp(argv[0], argv.data());

  die("execvp"); // only reached here if execvp failed
  return 1;
}
