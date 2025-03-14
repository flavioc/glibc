/* Copy SIZE bytes from SRC to DEST.  For SUN4V Niagara-2.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.
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

#define ASI_BLK_INIT_QUAD_LDD_P	0xe2
#define ASI_BLK_P		0xf0
#define ASI_P			0x80
#define ASI_PNF			0x82

#define FPRS_FEF		0x04

#define VISEntryHalf			\
	rd	%fprs, %o5;		\
	wr	%g0, FPRS_FEF, %fprs

#define VISExitHalf			\
	and	%o5, FPRS_FEF, %o5;	\
	wr	%o5, 0x0, %fprs

#define STORE_ASI		ASI_BLK_INIT_QUAD_LDD_P

#define LOAD(type,addr,dest)	type [addr], dest
#define LOAD_BLK(addr,dest)	ldda [addr] ASI_BLK_P, dest
#define STORE(type,src,addr)	type src, [addr]
#define STORE_BLK(src,addr)	stda src, [addr] ASI_BLK_P
#define STORE_INIT(src,addr)	stxa src, [addr] STORE_ASI

#ifndef XCC
#define USE_BPR
#define XCC xcc
#endif

#define FREG_FROB(x0, x1, x2, x3, x4, x5, x6, x7, x8) \
	faligndata	%x0, %x1, %f0; \
	faligndata	%x1, %x2, %f2; \
	faligndata	%x2, %x3, %f4; \
	faligndata	%x3, %x4, %f6; \
	faligndata	%x4, %x5, %f8; \
	faligndata	%x5, %x6, %f10; \
	faligndata	%x6, %x7, %f12; \
	faligndata	%x7, %x8, %f14;

#define FREG_MOVE_1(x0) \
	fsrc2		%x0, %f0;
#define FREG_MOVE_2(x0, x1) \
	fsrc2		%x0, %f0; \
	fsrc2		%x1, %f2;
#define FREG_MOVE_3(x0, x1, x2) \
	fsrc2		%x0, %f0; \
	fsrc2		%x1, %f2; \
	fsrc2		%x2, %f4;
#define FREG_MOVE_4(x0, x1, x2, x3) \
	fsrc2		%x0, %f0; \
	fsrc2		%x1, %f2; \
	fsrc2		%x2, %f4; \
	fsrc2		%x3, %f6;
#define FREG_MOVE_5(x0, x1, x2, x3, x4) \
	fsrc2		%x0, %f0; \
	fsrc2		%x1, %f2; \
	fsrc2		%x2, %f4; \
	fsrc2		%x3, %f6; \
	fsrc2		%x4, %f8;
#define FREG_MOVE_6(x0, x1, x2, x3, x4, x5) \
	fsrc2		%x0, %f0; \
	fsrc2		%x1, %f2; \
	fsrc2		%x2, %f4; \
	fsrc2		%x3, %f6; \
	fsrc2		%x4, %f8; \
	fsrc2		%x5, %f10;
#define FREG_MOVE_7(x0, x1, x2, x3, x4, x5, x6) \
	fsrc2		%x0, %f0; \
	fsrc2		%x1, %f2; \
	fsrc2		%x2, %f4; \
	fsrc2		%x3, %f6; \
	fsrc2		%x4, %f8; \
	fsrc2		%x5, %f10; \
	fsrc2		%x6, %f12;
#define FREG_MOVE_8(x0, x1, x2, x3, x4, x5, x6, x7) \
	fsrc2		%x0, %f0; \
	fsrc2		%x1, %f2; \
	fsrc2		%x2, %f4; \
	fsrc2		%x3, %f6; \
	fsrc2		%x4, %f8; \
	fsrc2		%x5, %f10; \
	fsrc2		%x6, %f12; \
	fsrc2		%x7, %f14;
#define FREG_LOAD_1(base, x0) \
	LOAD(ldd, base + 0x00, %x0)
#define FREG_LOAD_2(base, x0, x1) \
	LOAD(ldd, base + 0x00, %x0); \
	LOAD(ldd, base + 0x08, %x1);
#define FREG_LOAD_3(base, x0, x1, x2) \
	LOAD(ldd, base + 0x00, %x0); \
	LOAD(ldd, base + 0x08, %x1); \
	LOAD(ldd, base + 0x10, %x2);
#define FREG_LOAD_4(base, x0, x1, x2, x3) \
	LOAD(ldd, base + 0x00, %x0); \
	LOAD(ldd, base + 0x08, %x1); \
	LOAD(ldd, base + 0x10, %x2); \
	LOAD(ldd, base + 0x18, %x3);
#define FREG_LOAD_5(base, x0, x1, x2, x3, x4) \
	LOAD(ldd, base + 0x00, %x0); \
	LOAD(ldd, base + 0x08, %x1); \
	LOAD(ldd, base + 0x10, %x2); \
	LOAD(ldd, base + 0x18, %x3); \
	LOAD(ldd, base + 0x20, %x4);
#define FREG_LOAD_6(base, x0, x1, x2, x3, x4, x5) \
	LOAD(ldd, base + 0x00, %x0); \
	LOAD(ldd, base + 0x08, %x1); \
	LOAD(ldd, base + 0x10, %x2); \
	LOAD(ldd, base + 0x18, %x3); \
	LOAD(ldd, base + 0x20, %x4); \
	LOAD(ldd, base + 0x28, %x5);
#define FREG_LOAD_7(base, x0, x1, x2, x3, x4, x5, x6) \
	LOAD(ldd, base + 0x00, %x0); \
	LOAD(ldd, base + 0x08, %x1); \
	LOAD(ldd, base + 0x10, %x2); \
	LOAD(ldd, base + 0x18, %x3); \
	LOAD(ldd, base + 0x20, %x4); \
	LOAD(ldd, base + 0x28, %x5); \
	LOAD(ldd, base + 0x30, %x6);

#if IS_IN (libc)

	.register	%g2,#scratch
	.register	%g3,#scratch
	.register	%g6,#scratch

	.text

ENTRY(__mempcpy_niagara2)
	ba,pt		%XCC, 101f
	 add		%o0, %o2, %g5
END(__mempcpy_niagara2)

	.align		32
ENTRY(__memcpy_niagara2)
100:	/* %o0=dst, %o1=src, %o2=len */
	mov		%o0, %g5
101:
# ifndef USE_BPR
	srl		%o2, 0, %o2
# endif
	cmp		%o2, 0
	be,pn		%XCC, 85f
218:	 or		%o0, %o1, %o3
	cmp		%o2, 16
	blu,a,pn	%XCC, 80f
	 or		%o3, %o2, %o3

	/* 2 blocks (128 bytes) is the minimum we can do the block
	 * copy with.  We need to ensure that we'll iterate at least
	 * once in the block copy loop.  At worst we'll need to align
	 * the destination to a 64-byte boundary which can chew up
	 * to (64 - 1) bytes from the length before we perform the
	 * block copy loop.
	 *
	 * However, the cut-off point, performance wise, is around
	 * 4 64-byte blocks.
	 */
	cmp		%o2, (4 * 64)
	blu,pt		%XCC, 75f
	 andcc		%o3, 0x7, %g0

	/* %o0:	dst
	 * %o1:	src
	 * %o2:	len  (known to be >= 128)
	 *
	 * The block copy loops can use %o4, %g2, %g3 as
	 * temporaries while copying the data.  %o5 must
	 * be preserved between VISEntryHalf and VISExitHalf
	 */

	LOAD(prefetch, %o1 + 0x000, #one_read)
	LOAD(prefetch, %o1 + 0x040, #one_read)
	LOAD(prefetch, %o1 + 0x080, #one_read)

	/* Align destination on 64-byte boundary.  */
	andcc		%o0, (64 - 1), %o4
	be,pt		%XCC, 2f
	 sub		%o4, 64, %o4
	sub		%g0, %o4, %o4	! bytes to align dst
	sub		%o2, %o4, %o2
1:	subcc		%o4, 1, %o4
	LOAD(ldub, %o1, %g1)
	STORE(stb, %g1, %o0)
	add		%o1, 1, %o1
	bne,pt		%XCC, 1b
	add		%o0, 1, %o0

2:
	/* Clobbers o5/g1/g2/g3/g7/icc/xcc.  We must preserve
	 * o5 from here until we hit VISExitHalf.
	 */
	VISEntryHalf

	membar		#Sync
	alignaddr	%o1, %g0, %g0

	add		%o1, (64 - 1), %o4
	andn		%o4, (64 - 1), %o4
	andn		%o2, (64 - 1), %g1
	sub		%o2, %g1, %o2

	and		%o1, (64 - 1), %g2
	add		%o1, %g1, %o1
	sub		%o0, %o4, %g3
	brz,pt		%g2, 190f
	 cmp		%g2, 32
	blu,a		5f
	 cmp		%g2, 16
	cmp		%g2, 48
	blu,a		4f
	 cmp		%g2, 40
	cmp		%g2, 56
	blu		170f
	 nop
	ba,a,pt		%xcc, 180f

4:	/* 32 <= low bits < 48 */
	blu		150f
	 nop
	ba,a,pt		%xcc, 160f
5:	/* 0 < low bits < 32 */
	blu,a		6f
	 cmp		%g2, 8
	cmp		%g2, 24
	blu		130f
	 nop
	ba,a,pt		%xcc, 140f
6:	/* 0 < low bits < 16 */
	bgeu		120f
	 nop
	/* fall through for 0 < low bits < 8 */
110:	sub		%o4, 64, %g2
	LOAD_BLK(%g2, %f0)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f2, f4, f6, f8, f10, f12, f14, f16)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_8(f16, f18, f20, f22, f24, f26, f28, f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

120:	sub		%o4, 56, %g2
	FREG_LOAD_7(%g2, f0, f2, f4, f6, f8, f10, f12)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f2, f4, f6, f8, f10, f12, f16, f18)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_7(f18, f20, f22, f24, f26, f28, f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

130:	sub		%o4, 48, %g2
	FREG_LOAD_6(%g2, f0, f2, f4, f6, f8, f10)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f2, f4, f6, f8, f10, f16, f18, f20)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_6(f20, f22, f24, f26, f28, f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

140:	sub		%o4, 40, %g2
	FREG_LOAD_5(%g2, f0, f2, f4, f6, f8)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f2, f4, f6, f8, f16, f18, f20, f22)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_5(f22, f24, f26, f28, f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

150:	sub		%o4, 32, %g2
	FREG_LOAD_4(%g2, f0, f2, f4, f6)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f2, f4, f6, f16, f18, f20, f22, f24)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_4(f24, f26, f28, f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

160:	sub		%o4, 24, %g2
	FREG_LOAD_3(%g2, f0, f2, f4)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f2, f4, f16, f18, f20, f22, f24, f26)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_3(f26, f28, f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

170:	sub		%o4, 16, %g2
	FREG_LOAD_2(%g2, f0, f2)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f2, f16, f18, f20, f22, f24, f26, f28)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_2(f28, f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

180:	sub		%o4, 8, %g2
	FREG_LOAD_1(%g2, f0)
1:	STORE_INIT(%g0, %o4 + %g3)
	LOAD_BLK(%o4, %f16)
	FREG_FROB(f0, f16, f18, f20, f22, f24, f26, f28, f30)
	STORE_BLK(%f0, %o4 + %g3)
	FREG_MOVE_1(f30)
	subcc		%g1, 64, %g1
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)
	ba,pt		%xcc, 195f
	 nop

190:
1:	STORE_INIT(%g0, %o4 + %g3)
	subcc		%g1, 64, %g1
	LOAD_BLK(%o4, %f0)
	STORE_BLK(%f0, %o4 + %g3)
	add		%o4, 64, %o4
	bne,pt		%XCC, 1b
	 LOAD(prefetch, %o4 + 64, #one_read)

195:
	add		%o4, %g3, %o0
	membar		#Sync

	VISExitHalf

	/* %o2 contains any final bytes still needed to be copied
	 * over. If anything is left, we copy it one byte at a time.
	 */
	brz,pt		%o2, 85f
	 sub		%o0, %o1, %o3
	ba,a,pt		%XCC, 90f

	.align		64
75: /* 16 < len <= 64 */
	bne,pn		%XCC, 75f
	 sub		%o0, %o1, %o3

72:
	andn		%o2, 0xf, %o4
	and		%o2, 0xf, %o2
1:	subcc		%o4, 0x10, %o4
	LOAD(ldx, %o1, %o5)
	add		%o1, 0x08, %o1
	LOAD(ldx, %o1, %g1)
	sub		%o1, 0x08, %o1
	STORE(stx, %o5, %o1 + %o3)
	add		%o1, 0x8, %o1
	STORE(stx, %g1, %o1 + %o3)
	bgu,pt		%XCC, 1b
	 add		%o1, 0x8, %o1
73:	andcc		%o2, 0x8, %g0
	be,pt		%XCC, 1f
	 nop
	sub		%o2, 0x8, %o2
	LOAD(ldx, %o1, %o5)
	STORE(stx, %o5, %o1 + %o3)
	add		%o1, 0x8, %o1
1:	andcc		%o2, 0x4, %g0
	be,pt		%XCC, 1f
	 nop
	sub		%o2, 0x4, %o2
	LOAD(lduw, %o1, %o5)
	STORE(stw, %o5, %o1 + %o3)
	add		%o1, 0x4, %o1
1:	cmp		%o2, 0
	be,pt		%XCC, 85f
	 nop
	ba,pt		%xcc, 90f
	 nop

75:
	andcc		%o0, 0x7, %g1
	sub		%g1, 0x8, %g1
	be,pn		%icc, 2f
	 sub		%g0, %g1, %g1
	sub		%o2, %g1, %o2

1:	subcc		%g1, 1, %g1
	LOAD(ldub, %o1, %o5)
	STORE(stb, %o5, %o1 + %o3)
	bgu,pt		%icc, 1b
	 add		%o1, 1, %o1

2:	add		%o1, %o3, %o0
	andcc		%o1, 0x7, %g1
	bne,pt		%icc, 8f
	 sll		%g1, 3, %g1

	cmp		%o2, 16
	bgeu,pt		%icc, 72b
	 nop
	ba,a,pt		%xcc, 73b

8:	mov		64, %o3
	andn		%o1, 0x7, %o1
	LOAD(ldx, %o1, %g2)
	sub		%o3, %g1, %o3
	andn		%o2, 0x7, %o4
	sllx		%g2, %g1, %g2
1:	add		%o1, 0x8, %o1
	LOAD(ldx, %o1, %g3)
	subcc		%o4, 0x8, %o4
	srlx		%g3, %o3, %o5
	or		%o5, %g2, %o5
	STORE(stx, %o5, %o0)
	add		%o0, 0x8, %o0
	bgu,pt		%icc, 1b
	 sllx		%g3, %g1, %g2

	srl		%g1, 3, %g1
	andcc		%o2, 0x7, %o2
	be,pn		%icc, 85f
	 add		%o1, %g1, %o1
	ba,pt		%xcc, 90f
	 sub		%o0, %o1, %o3

	.align		64
80: /* 0 < len <= 16 */
	andcc		%o3, 0x3, %g0
	bne,pn		%XCC, 90f
	 sub		%o0, %o1, %o3

1:
	subcc		%o2, 4, %o2
	LOAD(lduw, %o1, %g1)
	STORE(stw, %g1, %o1 + %o3)
	bgu,pt		%XCC, 1b
	 add		%o1, 4, %o1

85:	retl
	 mov		%g5, %o0

	.align		32
90:
	subcc		%o2, 1, %o2
	LOAD(ldub, %o1, %g1)
	STORE(stb, %g1, %o1 + %o3)
	bgu,pt		%XCC, 90b
	 add		%o1, 1, %o1
	retl
	 mov		%g5, %o0

END(__memcpy_niagara2)

#endif
