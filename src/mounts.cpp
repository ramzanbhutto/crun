#include<unistd.h>
#include<sys/mount.h>
#include<sys/syscall.h>
#include "mounts.hpp"
#include "util.hpp"

namespace fs= std::filesystem;
namespace{
  int sys_pivot_root(const char* new_root, const char* put_old){
    return syscall(SYS_pivot_root, new_root, put_old);  // glibc has no wrapper for pivot_root, so i hit the syscall table directly
  }
}
namespace mounts{
 void pivot_root(Config& config){
   log_step("remounting / as MS_PRIVATE");
   // stop mount events here from propagating to/from the hosts mount namespace so no event can see other events subtree that is visible in linux mount points on MS_SHARED mount flag
   if(::mount(nullptr,"/",nullptr, MS_REC | MS_PRIVATE, nullptr)==-1) die("remounting / as private");  // MS_PRIVATE = stops propagating  , MS_REC= makes that apply recursively to everything mounted under / 
   
   log_step("bind-mounting rootfs onto itself");
   // pivot_root requires new_root to already be a mountpoint - this trick makes it one without moving anything
   if(::mount(config.rootfs.c_str(), config.rootfs.c_str(), nullptr, MS_BIND | MS_PRIVATE, nullptr)==-1) die("bind mounting rootfs onto itself");

   fs::path root= config.rootfs;
   // pid-namespaced temp dir name so concurrent containers never collide
   fs::path old_root_dir= root / (".oldroot." +std::to_string(getpid()));
   
   log_step("creating temp dir for old root");
   std::error_code ec;
   fs::create_directory(old_root_dir, ec);
   if(ec) die("creating old root dir: " + ec.message());

   log_step("pivoting root");
   // swaps / : new_root becomes /, the old host root becomes reachable only at old_root_dir
   if(sys_pivot_root(root.c_str(), old_root_dir.c_str())==-1) die("pivot_root");

   if(::chdir("/")==-1) die("chdir to new root");     // cwd still points at the pre-pivot roots inode - reset it or we are standing nowhere sane

   fs::path old_root= "/" / old_root_dir.filename();     // recompute the old roots path now that we are living inside the new /

   log_step("unmounting old root");
   if(::umount2(old_root.c_str(), MNT_DETACH)==-1) die("umount2 old root");    // MNT_DETACH = lazy unmount, avoids "device busy" errors

   fs::remove(old_root, ec);     // nothing is mounted there anymore, safe to delete the now-empty temp dir
   if(ec) die("removing old root dir: " +ec.message());
   
   log_step("mounting /proc");
   if(::mount("proc", "/proc", "proc", 0, nullptr)==-1) die("mounting /proc");    // without this, ps/top/getpid()-callers inside the container see the HOST's real /proc, not this namespace's
 }

}
