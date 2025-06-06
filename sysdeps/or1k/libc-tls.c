/* Thread-local storage handling in the ELF dynamic linker.
   OpenRISC version.
   Copyright (C) 2022-2025 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <csu/libc-tls.c>
#include <dl-tls.h>

/* On OpenRISC, linker optimizations are not required, so __tls_get_addr
   can be called even in statically linked binaries.  In this case module
   must be always 1 and PT_TLS segment exist in the binary, otherwise it
   would not link.  */

void *
__tls_get_addr (tls_index *ti)
{
  dtv_t *dtv = THREAD_DTV ();
  return (char *) dtv[1].pointer.val + ti->ti_offset;
}
