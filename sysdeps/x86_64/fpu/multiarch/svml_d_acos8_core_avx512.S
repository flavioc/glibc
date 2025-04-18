/* Function acos vectorized with AVX-512.
   Copyright (C) 2021-2025 Free Software Foundation, Inc.
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
   https://www.gnu.org/licenses/.  */

/*
 * ALGORITHM DESCRIPTION:
 *
 *      SelMask = (|x| >= 0.5) ? 1 : 0;
 *      R = SelMask ? sqrt(0.5 - 0.5*|x|) : |x|
 *      acos(|x|) = SelMask ? 2*Poly(R) : (Pi/2 - Poly(R))
 *      acos(x) = sign(x) ? (Pi - acos(|x|)) : acos(|x|)
 *
 */

/* Offsets for data table __svml_dacos_data_internal
 */
#define SgnBit				0
#define OneHalf				64
#define SmallNorm			128
#define MOne				192
#define Two				256
#define sqrt_coeff_1			320
#define sqrt_coeff_2			384
#define sqrt_coeff_3			448
#define sqrt_coeff_4			512
#define poly_coeff_1			576
#define poly_coeff_2			640
#define poly_coeff_3			704
#define poly_coeff_4			768
#define poly_coeff_5			832
#define poly_coeff_6			896
#define poly_coeff_7			960
#define poly_coeff_8			1024
#define poly_coeff_9			1088
#define poly_coeff_10			1152
#define poly_coeff_11			1216
#define poly_coeff_12			1280
#define PiH				1344
#define Pi2H				1408

#include <sysdep.h>

	.section .text.evex512, "ax", @progbits
ENTRY(_ZGVeN8v_acos_skx)
	pushq	%rbp
	cfi_def_cfa_offset(16)
	movq	%rsp, %rbp
	cfi_def_cfa(6, 16)
	cfi_offset(6, -16)
	andq	$-64, %rsp
	subq	$192, %rsp
	vmovups	__svml_dacos_data_internal(%rip), %zmm7
	vmovups	OneHalf+__svml_dacos_data_internal(%rip), %zmm8

	/* S ~ 2*sqrt(Y) */
	vmovups	SmallNorm+__svml_dacos_data_internal(%rip), %zmm11
	vmovups	Two+__svml_dacos_data_internal(%rip), %zmm14
	vmovups	sqrt_coeff_1+__svml_dacos_data_internal(%rip), %zmm15
	vmovups	sqrt_coeff_2+__svml_dacos_data_internal(%rip), %zmm2
	vmovups	sqrt_coeff_3+__svml_dacos_data_internal(%rip), %zmm1
	vmovups	MOne+__svml_dacos_data_internal(%rip), %zmm10
	vmovaps	%zmm0, %zmm6

	/* x = -|arg| */
	vorpd	%zmm6, %zmm7, %zmm5
	vandpd	%zmm6, %zmm7, %zmm4

	/* Y = 0.5 + 0.5*(-x) */
	vfmadd231pd {rn-sae}, %zmm5, %zmm8, %zmm8

	/* x^2 */
	vmulpd	{rn-sae}, %zmm5, %zmm5, %zmm9
	vrsqrt14pd %zmm8, %zmm12
	vcmppd	$17, {sae}, %zmm11, %zmm8, %k1
	vcmppd	$17, {sae}, %zmm10, %zmm5, %k0
	vmovups	poly_coeff_5+__svml_dacos_data_internal(%rip), %zmm10
	vmovups	poly_coeff_7+__svml_dacos_data_internal(%rip), %zmm11
	vminpd	{sae}, %zmm8, %zmm9, %zmm3
	vmovups	poly_coeff_3+__svml_dacos_data_internal(%rip), %zmm9
	vxorpd	%zmm12, %zmm12, %zmm12{%k1}
	vaddpd	{rn-sae}, %zmm8, %zmm8, %zmm0
	vcmppd	$21, {sae}, %zmm8, %zmm3, %k4

	/* X<X^2 iff X<0 */
	vcmppd	$17, {sae}, %zmm3, %zmm6, %k2
	vmulpd	{rn-sae}, %zmm12, %zmm12, %zmm13
	vmulpd	{rn-sae}, %zmm12, %zmm0, %zmm7
	vmovups	poly_coeff_4+__svml_dacos_data_internal(%rip), %zmm12

	/* polynomial */
	vmovups	poly_coeff_1+__svml_dacos_data_internal(%rip), %zmm8
	vfmsub213pd {rn-sae}, %zmm14, %zmm13, %zmm0
	vmovups	sqrt_coeff_4+__svml_dacos_data_internal(%rip), %zmm13
	vfmadd231pd {rn-sae}, %zmm3, %zmm9, %zmm12
	vmovups	poly_coeff_11+__svml_dacos_data_internal(%rip), %zmm9
	vfmadd231pd {rn-sae}, %zmm0, %zmm15, %zmm2
	vmovups	poly_coeff_9+__svml_dacos_data_internal(%rip), %zmm15
	vmulpd	{rn-sae}, %zmm0, %zmm7, %zmm14
	vfmadd213pd {rn-sae}, %zmm1, %zmm0, %zmm2
	vmovups	poly_coeff_2+__svml_dacos_data_internal(%rip), %zmm1
	kmovw	%k0, %edx
	vfmadd213pd {rn-sae}, %zmm13, %zmm0, %zmm2
	vfmadd231pd {rn-sae}, %zmm3, %zmm8, %zmm1
	vmovups	poly_coeff_10+__svml_dacos_data_internal(%rip), %zmm8
	vmulpd	{rn-sae}, %zmm3, %zmm3, %zmm0
	vfnmadd213pd {rn-sae}, %zmm7, %zmm14, %zmm2
	vmovups	poly_coeff_6+__svml_dacos_data_internal(%rip), %zmm7
	vfmadd231pd {rn-sae}, %zmm3, %zmm15, %zmm8
	vfmadd213pd {rn-sae}, %zmm12, %zmm0, %zmm1
	vblendmpd %zmm2, %zmm5, %zmm2{%k4}
	vfmadd231pd {rn-sae}, %zmm3, %zmm10, %zmm7
	vmovups	poly_coeff_8+__svml_dacos_data_internal(%rip), %zmm10
	vfmadd231pd {rn-sae}, %zmm3, %zmm11, %zmm10
	vmovups	poly_coeff_12+__svml_dacos_data_internal(%rip), %zmm11
	kandw	%k4, %k2, %k3
	vfmadd213pd {rn-sae}, %zmm10, %zmm0, %zmm7
	vfmadd231pd {rn-sae}, %zmm3, %zmm9, %zmm11
	vmulpd	{rn-sae}, %zmm0, %zmm0, %zmm10
	vfmadd213pd {rn-sae}, %zmm7, %zmm10, %zmm1
	vfmadd213pd {rn-sae}, %zmm8, %zmm0, %zmm1
	vfmadd213pd {rn-sae}, %zmm11, %zmm0, %zmm1
	vmovups	Pi2H+__svml_dacos_data_internal(%rip), %zmm0
	vmulpd	{rn-sae}, %zmm3, %zmm1, %zmm1
	vxorpd	%zmm4, %zmm2, %zmm3
	vxorpd	%zmm0, %zmm0, %zmm0{%k4}
	vfmadd213pd {rn-sae}, %zmm3, %zmm3, %zmm1
	vorpd	PiH+__svml_dacos_data_internal(%rip), %zmm0, %zmm0{%k3}
	vaddpd	{rn-sae}, %zmm1, %zmm0, %zmm0
	testl	%edx, %edx

	/* Go to special inputs processing branch */
	jne	L(SPECIAL_VALUES_BRANCH)
	# LOE rbx r12 r13 r14 r15 edx zmm0 zmm6

	/* Restore registers
	 * and exit the function
	 */

L(EXIT):
	movq	%rbp, %rsp
	popq	%rbp
	cfi_def_cfa(7, 8)
	cfi_restore(6)
	ret
	cfi_def_cfa(6, 16)
	cfi_offset(6, -16)

	/* Branch to process
	 * special inputs
	 */

L(SPECIAL_VALUES_BRANCH):
	vmovups	%zmm6, 64(%rsp)
	vmovups	%zmm0, 128(%rsp)
	# LOE rbx r12 r13 r14 r15 edx zmm0

	xorl	%eax, %eax
	# LOE rbx r12 r13 r14 r15 eax edx

	vzeroupper
	movq	%r12, 16(%rsp)
	/*  DW_CFA_expression: r12 (r12) (DW_OP_lit8; DW_OP_minus; DW_OP_const4s: -64; DW_OP_and; DW_OP_const4s: -176; DW_OP_plus)  */
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0x50, 0xff, 0xff, 0xff, 0x22
	movl	%eax, %r12d
	movq	%r13, 8(%rsp)
	/*  DW_CFA_expression: r13 (r13) (DW_OP_lit8; DW_OP_minus; DW_OP_const4s: -64; DW_OP_and; DW_OP_const4s: -184; DW_OP_plus)  */
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0x48, 0xff, 0xff, 0xff, 0x22
	movl	%edx, %r13d
	movq	%r14, (%rsp)
	/*  DW_CFA_expression: r14 (r14) (DW_OP_lit8; DW_OP_minus; DW_OP_const4s: -64; DW_OP_and; DW_OP_const4s: -192; DW_OP_plus)  */
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0x40, 0xff, 0xff, 0xff, 0x22
	# LOE rbx r15 r12d r13d

	/* Range mask
	 * bits check
	 */

L(RANGEMASK_CHECK):
	btl	%r12d, %r13d

	/* Call scalar math function */
	jc	L(SCALAR_MATH_CALL)
	# LOE rbx r15 r12d r13d

	/* Special inputs
	 * processing loop
	 */

L(SPECIAL_VALUES_LOOP):
	incl	%r12d
	cmpl	$8, %r12d

	/* Check bits in range mask */
	jl	L(RANGEMASK_CHECK)
	# LOE rbx r15 r12d r13d

	movq	16(%rsp), %r12
	cfi_restore(12)
	movq	8(%rsp), %r13
	cfi_restore(13)
	movq	(%rsp), %r14
	cfi_restore(14)
	vmovups	128(%rsp), %zmm0

	/* Go to exit */
	jmp	L(EXIT)
	/*  DW_CFA_expression: r12 (r12) (DW_OP_lit8; DW_OP_minus; DW_OP_const4s: -64; DW_OP_and; DW_OP_const4s: -176; DW_OP_plus)  */
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0x50, 0xff, 0xff, 0xff, 0x22
	/*  DW_CFA_expression: r13 (r13) (DW_OP_lit8; DW_OP_minus; DW_OP_const4s: -64; DW_OP_and; DW_OP_const4s: -184; DW_OP_plus)  */
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0x48, 0xff, 0xff, 0xff, 0x22
	/*  DW_CFA_expression: r14 (r14) (DW_OP_lit8; DW_OP_minus; DW_OP_const4s: -64; DW_OP_and; DW_OP_const4s: -192; DW_OP_plus)  */
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0x40, 0xff, 0xff, 0xff, 0x22
	# LOE rbx r12 r13 r14 r15 zmm0

	/* Scalar math function call
	 * to process special input
	 */

L(SCALAR_MATH_CALL):
	movl	%r12d, %r14d
	vmovsd	64(%rsp, %r14, 8), %xmm0
	call	acos@PLT
	# LOE rbx r14 r15 r12d r13d xmm0

	vmovsd	%xmm0, 128(%rsp, %r14, 8)

	/* Process special inputs in loop */
	jmp	L(SPECIAL_VALUES_LOOP)
	# LOE rbx r15 r12d r13d
END(_ZGVeN8v_acos_skx)

	.section .rodata, "a"
	.align	64

#ifdef __svml_dacos_data_internal_typedef
typedef unsigned int VUINT32;
typedef struct {
	__declspec(align(64)) VUINT32 SgnBit[8][2];
	__declspec(align(64)) VUINT32 OneHalf[8][2];
	__declspec(align(64)) VUINT32 SmallNorm[8][2];
	__declspec(align(64)) VUINT32 MOne[8][2];
	__declspec(align(64)) VUINT32 Two[8][2];
	__declspec(align(64)) VUINT32 sqrt_coeff[4][8][2];
	__declspec(align(64)) VUINT32 poly_coeff[12][8][2];
	__declspec(align(64)) VUINT32 PiH[8][2];
	__declspec(align(64)) VUINT32 Pi2H[8][2];
} __svml_dacos_data_internal;
#endif
__svml_dacos_data_internal:
	/* SgnBit */
	.quad	0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000
	/* OneHalf */
	.align	64
	.quad	0x3fe0000000000000, 0x3fe0000000000000, 0x3fe0000000000000, 0x3fe0000000000000, 0x3fe0000000000000, 0x3fe0000000000000, 0x3fe0000000000000, 0x3fe0000000000000
	/* SmallNorm */
	.align	64
	.quad	0x3000000000000000, 0x3000000000000000, 0x3000000000000000, 0x3000000000000000, 0x3000000000000000, 0x3000000000000000, 0x3000000000000000, 0x3000000000000000
	/* MOne */
	.align	64
	.quad	0xbff0000000000000, 0xbff0000000000000, 0xbff0000000000000, 0xbff0000000000000, 0xbff0000000000000, 0xbff0000000000000, 0xbff0000000000000, 0xbff0000000000000
	/* Two */
	.align	64
	.quad	0x4000000000000000, 0x4000000000000000, 0x4000000000000000, 0x4000000000000000, 0x4000000000000000, 0x4000000000000000, 0x4000000000000000, 0x4000000000000000
	/* sqrt_coeff[4] */
	.align	64
	.quad	0xbf918000993B24C3, 0xbf918000993B24C3, 0xbf918000993B24C3, 0xbf918000993B24C3, 0xbf918000993B24C3, 0xbf918000993B24C3, 0xbf918000993B24C3, 0xbf918000993B24C3 /* sqrt_coeff4 */
	.quad	0x3fa400006F70D42D, 0x3fa400006F70D42D, 0x3fa400006F70D42D, 0x3fa400006F70D42D, 0x3fa400006F70D42D, 0x3fa400006F70D42D, 0x3fa400006F70D42D, 0x3fa400006F70D42D /* sqrt_coeff3 */
	.quad	0xbfb7FFFFFFFFFE97, 0xbfb7FFFFFFFFFE97, 0xbfb7FFFFFFFFFE97, 0xbfb7FFFFFFFFFE97, 0xbfb7FFFFFFFFFE97, 0xbfb7FFFFFFFFFE97, 0xbfb7FFFFFFFFFE97, 0xbfb7FFFFFFFFFE97 /* sqrt_coeff2 */
	.quad	0x3fcFFFFFFFFFFF9D, 0x3fcFFFFFFFFFFF9D, 0x3fcFFFFFFFFFFF9D, 0x3fcFFFFFFFFFFF9D, 0x3fcFFFFFFFFFFF9D, 0x3fcFFFFFFFFFFF9D, 0x3fcFFFFFFFFFFF9D, 0x3fcFFFFFFFFFFF9D /* sqrt_coeff1 */
	/* poly_coeff[12] */
	.align	64
	.quad	0x3fa07520C70EB909, 0x3fa07520C70EB909, 0x3fa07520C70EB909, 0x3fa07520C70EB909, 0x3fa07520C70EB909, 0x3fa07520C70EB909, 0x3fa07520C70EB909, 0x3fa07520C70EB909 /* poly_coeff12 */
	.quad	0xbf90FB17F7DBB0ED, 0xbf90FB17F7DBB0ED, 0xbf90FB17F7DBB0ED, 0xbf90FB17F7DBB0ED, 0xbf90FB17F7DBB0ED, 0xbf90FB17F7DBB0ED, 0xbf90FB17F7DBB0ED, 0xbf90FB17F7DBB0ED /* poly_coeff11 */
	.quad	0x3f943F44BFBC3BAE, 0x3f943F44BFBC3BAE, 0x3f943F44BFBC3BAE, 0x3f943F44BFBC3BAE, 0x3f943F44BFBC3BAE, 0x3f943F44BFBC3BAE, 0x3f943F44BFBC3BAE, 0x3f943F44BFBC3BAE /* poly_coeff10 */
	.quad	0x3f7A583395D45ED5, 0x3f7A583395D45ED5, 0x3f7A583395D45ED5, 0x3f7A583395D45ED5, 0x3f7A583395D45ED5, 0x3f7A583395D45ED5, 0x3f7A583395D45ED5, 0x3f7A583395D45ED5 /* poly_coeff9 */
	.quad	0x3f88F8DC2AFCCAD6, 0x3f88F8DC2AFCCAD6, 0x3f88F8DC2AFCCAD6, 0x3f88F8DC2AFCCAD6, 0x3f88F8DC2AFCCAD6, 0x3f88F8DC2AFCCAD6, 0x3f88F8DC2AFCCAD6, 0x3f88F8DC2AFCCAD6 /* poly_coeff8 */
	.quad	0x3f8C6DBBCB88BD57, 0x3f8C6DBBCB88BD57, 0x3f8C6DBBCB88BD57, 0x3f8C6DBBCB88BD57, 0x3f8C6DBBCB88BD57, 0x3f8C6DBBCB88BD57, 0x3f8C6DBBCB88BD57, 0x3f8C6DBBCB88BD57 /* poly_coeff7 */
	.quad	0x3f91C6DCF538AD2E, 0x3f91C6DCF538AD2E, 0x3f91C6DCF538AD2E, 0x3f91C6DCF538AD2E, 0x3f91C6DCF538AD2E, 0x3f91C6DCF538AD2E, 0x3f91C6DCF538AD2E, 0x3f91C6DCF538AD2E /* poly_coeff6 */
	.quad	0x3f96E89CEBDEFadd, 0x3f96E89CEBDEFadd, 0x3f96E89CEBDEFadd, 0x3f96E89CEBDEFadd, 0x3f96E89CEBDEFadd, 0x3f96E89CEBDEFadd, 0x3f96E89CEBDEFadd, 0x3f96E89CEBDEFadd /* poly_coeff5 */
	.quad	0x3f9F1C72E13AD8BE, 0x3f9F1C72E13AD8BE, 0x3f9F1C72E13AD8BE, 0x3f9F1C72E13AD8BE, 0x3f9F1C72E13AD8BE, 0x3f9F1C72E13AD8BE, 0x3f9F1C72E13AD8BE, 0x3f9F1C72E13AD8BE /* poly_coeff4 */
	.quad	0x3fa6DB6DB3B445F8, 0x3fa6DB6DB3B445F8, 0x3fa6DB6DB3B445F8, 0x3fa6DB6DB3B445F8, 0x3fa6DB6DB3B445F8, 0x3fa6DB6DB3B445F8, 0x3fa6DB6DB3B445F8, 0x3fa6DB6DB3B445F8 /* poly_coeff3 */
	.quad	0x3fb333333337E0DE, 0x3fb333333337E0DE, 0x3fb333333337E0DE, 0x3fb333333337E0DE, 0x3fb333333337E0DE, 0x3fb333333337E0DE, 0x3fb333333337E0DE, 0x3fb333333337E0DE /* poly_coeff2 */
	.quad	0x3fc555555555529C, 0x3fc555555555529C, 0x3fc555555555529C, 0x3fc555555555529C, 0x3fc555555555529C, 0x3fc555555555529C, 0x3fc555555555529C, 0x3fc555555555529C /* poly_coeff1 */
	/* PiH */
	.align	64
	.quad	0x400921fb54442d18, 0x400921fb54442d18, 0x400921fb54442d18, 0x400921fb54442d18, 0x400921fb54442d18, 0x400921fb54442d18, 0x400921fb54442d18, 0x400921fb54442d18
	/* Pi2H */
	.align	64
	.quad	0x3ff921fb54442d18, 0x3ff921fb54442d18, 0x3ff921fb54442d18, 0x3ff921fb54442d18, 0x3ff921fb54442d18, 0x3ff921fb54442d18, 0x3ff921fb54442d18, 0x3ff921fb54442d18
	.align	64
	.type	__svml_dacos_data_internal, @object
	.size	__svml_dacos_data_internal, .-__svml_dacos_data_internal
