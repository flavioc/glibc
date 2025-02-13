/* Helper functions for manipulating memory protection keys.
   Copyright (C) 2017-2025 Free Software Foundation, Inc.
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

#ifndef _ARCH_PKEY_H
#define _ARCH_PKEY_H

/* Return the value of the PKRU register.  */
static inline unsigned int
pkey_read (void)
{
  unsigned int result;
  __asm__ volatile (".byte 0x0f, 0x01, 0xee"
                    : "=a" (result) : "c" (0) : "rdx");
  return result;
}

/* Overwrite the PKRU register with VALUE.  */
static inline void
pkey_write (unsigned int value)
{
  __asm__ volatile (".byte 0x0f, 0x01, 0xef"
                    : : "a" (value), "c" (0), "d" (0));
}

#endif /* _ARCH_PKEY_H */
