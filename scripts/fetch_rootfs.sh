#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."

mkdir -p rootfs/alpine
cd rootfs/alpine

ver="3.24.1"
branch="v3.24"
arch="x86_64"
url="https://dl-cdn.alpinelinux.org/alpine/${branch}/releases/${arch}/alpine-minirootfs-${ver}-${arch}.tar.gz"

echo "=> downloading alpine minirootfs"
curl -fSL "$url" -o rootfs.tar.gz
tar -xzf rootfs.tar.gz
rm rootfs.tar.gz
echo "=> done, extracted to $(pwd)"
