# crun

A tiny container runtime, built from scratch in C++ to actually understand what Docker is doing under the hood.

Right now it can spin up a process in its own namespaces (PID, mount, UTS, etc.), chroot it into a rootfs and cage it with cgroup v2.

---

## Build
From the /crun root level.

```bash
./scripts/build.sh
```

This builds the project with CMake into `build/`. The binary ends up at `build/crun`.

---

## Get a rootfs

You need a rootfs to run something inside. Grab a minimal Alpine one with:
```bash
./scripts/fetch_rootfs.sh
```

This downloads and extracts Alpine into `rootfs/alpine`.

---

## Usage:
From the /crun root level.

```bash
sudo ./build/crun -m <rootfs> -u <uid> -c <cmd> [args...]
```

Example:

```bash
sudo ./build/crun -m rootfs/alpine -u 0 -c /bin/sh -c "echo hello"
```

> `-c` takes everything after it as the command and its arguments - including further flags. `-c /bin/sh -c "echo hi"` works because `/bin/sh` gets `-c "echo hi"` as its own arguments, not because crun parses two `-c` flags. Below is another working example:

```bash
sudo ./build/crun -m rootfs/alpine -u 0 -c /bin/echo hello
```

Auto cleanup confirmation:
To confirm the cgroup actually got cleaned up after the container exits:
```bash
ls /sys/fs/cgroup/ | grep crun
```

> This command should print nothing once crun exits, that confirms the cgroup was cleaned up automatically (RAII), not leaked. If a `crun-<pid>` entry is still listed, that means cleanup failed.
---

## Status

Completed: namespaces, mount isolation, cgroup v2. 

Plans: capabilities, user namespaces (`-u` flag in this version parses, but does nothing, as I have not added user namespaces yet) , networking, socketpair handshake between parent/child.

This is a learning project, not something you should run in production. Expect sharp edges.
