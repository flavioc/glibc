/* Get CPUID feature leaf.  Linux/x86-64 version.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

#include <sysdeps/x86/get-cpuid-feature-leaf.c>

#ifdef __LP64__
_Static_assert (FEATURE_1_OFFSET == 72, "FEATURE_1_OFFSET != 72");
#else
_Static_assert (FEATURE_1_OFFSET == 40, "FEATURE_1_OFFSET != 40");
#endif
