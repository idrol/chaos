echo Test
qemu-system-i386 -s -S -kernel %1\src\kernel\kernel -d guest_errors -no-shutdown -no-reboot -monitor stdio -m 4G