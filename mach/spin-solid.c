/* Copyright (C) 1994-2025 Free Software Foundation, Inc.
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

#include <spin-lock.h>
#include <mach/mach_traps.h>

void
__spin_lock_solid (spin_lock_t *lock)
{
  while (__spin_lock_locked (lock) || ! __spin_try_lock (lock))
    /* Yield to another thread (system call).  */
    __swtch_pri (0);
}
weak_alias (__spin_lock_solid, spin_lock_solid);
libc_hidden_def (__spin_lock_solid)
