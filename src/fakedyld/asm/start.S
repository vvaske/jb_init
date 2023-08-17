.globl __dyld_start
.align 4

__dyld_start:
movn x8, #0xf
mov x7, sp
and x7, x7, x8
mov sp, x7
bl _main
movz x16, #0x1
svc 0x80

.globl dyld_stub_binder
dyld_stub_binder:
