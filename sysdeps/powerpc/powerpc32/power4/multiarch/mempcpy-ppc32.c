/* PowerPC32 default implementation of mempcpy.
   Copyright (C) 2013-2025 Free Software Foundation, Inc.
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

#define MEMPCPY  __mempcpy_ppc

#undef libc_hidden_def
#define libc_hidden_def(name)
#undef weak_alias
#define weak_alias(a, b)

#if defined SHARED
# undef libc_hidden_builtin_def
# define libc_hidden_builtin_def(name)  \
  __hidden_ver1 (__mempcpy_ppc, __GI_mempcpy, __mempcpy_ppc);
#endif

#include <string/mempcpy.c>
