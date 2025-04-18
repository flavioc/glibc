/* Install given context.
   Copyright (C) 2005-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <sysdep.h>

#include "ucontext_i.h"

/*  int __setcontext (const ucontext_t *uc);  */

	.text
	.align  5
ENTRY(__setcontext)

	mov	r4, r8

	/* sigprocmask (SIG_SETMASK, &uc->uc_sigmask, NULL).  */
	mov	r4, r5
	add	#oSIGMASK, r5
	mov	#SIG_SETMASK, r4
	mov	#0, r6
	mov	#+SYS_ify(sigprocmask), r3
	trapa	#0x13
	mov     r0, r1
	mov	#-12, r2
	shad	r2, r1
	not	r1, r1			// r1=0 means r0 = -1 to -4095
	tst	r1, r1			// i.e. error in linux
	bf	.Lsetcontext_restore
.Lsyscall_error:
	SYSCALL_ERROR_HANDLER
.Lpseudo_end:
	rts
	 nop

.Lsetcontext_restore:
	mov	r8, r0
	add	#(oPC), r0
	mov.l	@r0+, r2
	lds.l	@r0+, pr

	/* Restore T frag.  */
	mov.l	@r0+, r1
	shlr	r1
	/* Skip GBR which is used for thread pointer.  */
	add	#4, r0

	lds.l	@r0+, mach
	lds.l	@r0+, macl

	mov	r8, r0
	add	#(oR9), r0
	mov.l	@r0+, r9
	mov.l	@r0+, r10
	mov.l	@r0+, r11
	mov.l	@r0+, r12
	mov.l	@r0+, r13
	mov.l	@r0+, r14
	mov.l	@r0+, r15

	mov	r8, r0
	mov.l	@(oR0,r0), r1
	mov.l	r1, @-r15
	cfi_adjust_cfa_offset(4)
	cfi_rel_offset (r1, 0)
	mov.l	r2, @-r15
	cfi_adjust_cfa_offset(4)
	cfi_rel_offset (r2, 0)

	mov.l	@(oR1,r0), r1
	mov.l	@(oR2,r0), r2
	mov.l	@(oR3,r0), r3
	mov.l	@(oR4,r0), r4
	mov.l	@(oR5,r0), r5
	mov.l	@(oR6,r0), r6
	mov.l	@(oR7,r0), r7
	mov.l	@(oR8,r0), r8
	mov.l	@r15+, r0
	cfi_adjust_cfa_offset(-4)
	jmp	@r0
	 mov.l	@r15+, r0

PSEUDO_END(__setcontext)

weak_alias (__setcontext, setcontext)
