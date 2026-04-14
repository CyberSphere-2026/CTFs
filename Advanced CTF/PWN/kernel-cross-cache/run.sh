#!/bin/sh


qemu-system-x86_64 \
  -cpu qemu64,+smep,+smap \
  -m 512M \
  -kernel ./bzImage \
  -initrd ./initramfs.cpio.gz \
  -append "console=ttyS0 kaslr quiet panic=1" \
  -nographic \
  -monitor /dev/null \
  -no-reboot 

