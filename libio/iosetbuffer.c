/* Copyright (C) 1993-2025 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.

   As a special exception, if you link the code in this file with
   files compiled with a GNU compiler to produce an executable,
   that does not cause the resulting executable to be covered by
   the GNU Lesser General Public License.  This exception does not
   however invalidate any other reasons why the executable file
   might be covered by the GNU Lesser General Public License.
   This exception applies to code released by its copyright holders
   in files containing the exception.  */

#include "libioP.h"

void
_IO_setbuffer (FILE *fp, char *buf, size_t size)
{
  CHECK_FILE (fp, );
  _IO_acquire_lock (fp);
  fp->_flags &= ~_IO_LINE_BUF;
  if (!buf)
    size = 0;
  (void) _IO_SETBUF (fp, buf, size);
  if (_IO_vtable_offset (fp) == 0 && fp->_mode == 0 && _IO_CHECK_WIDE (fp))
    /* We also have to set the buffer using the wide char function.  */
    (void) _IO_WSETBUF (fp, buf, size);
  _IO_release_lock (fp);
}
libc_hidden_def (_IO_setbuffer)

weak_alias (_IO_setbuffer, setbuffer)
