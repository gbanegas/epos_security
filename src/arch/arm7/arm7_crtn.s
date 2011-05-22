# 1 "arm7_crtn.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "arm7_crtn.S"
.file "arm7_crtn.S"

# This work is licensed under the EPOS Software License v1.0.
# A copy of this license is available at the EPOS system source tree root.
# A copy of this license is also available online at:
# http:
# Note that EPOS Software License applies to both source code and executables.

.section .init
ldr pc, =__epos_library_app_entry

.section .fini
.globl _fini
.type _fini,function
bx lr
