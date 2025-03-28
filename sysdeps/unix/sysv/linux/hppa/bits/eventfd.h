/* Copyright (C) 2007-2025 Free Software Foundation, Inc.
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

#ifndef	_SYS_EVENTFD_H
# error "Never use <bits/eventfd.h> directly; include <sys/eventfd.h> instead."
#endif

/* Flags for eventfd.  */
enum
  {
    EFD_SEMAPHORE = 1,
#define EFD_SEMAPHORE EFD_SEMAPHORE
    EFD_CLOEXEC = 010000000,
#define EFD_CLOEXEC EFD_CLOEXEC
    EFD_NONBLOCK = 00200000
#define EFD_NONBLOCK EFD_NONBLOCK
  };
