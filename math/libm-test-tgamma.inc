/* Test tgamma.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
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

#include "libm-test-driver.c"

static const struct test_f_f_data tgamma_test_data[] =
  {
    TEST_f_f (tgamma, plus_infty, plus_infty, ERRNO_UNCHANGED),
    TEST_f_f (tgamma, 0, plus_infty, DIVIDE_BY_ZERO_EXCEPTION|ERRNO_ERANGE),
    TEST_f_f (tgamma, minus_zero, minus_infty, DIVIDE_BY_ZERO_EXCEPTION|ERRNO_ERANGE),
    /* tgamma (x) == qNaN plus invalid exception for integer x <= 0.  */
    TEST_f_f (tgamma, -2, qnan_value, INVALID_EXCEPTION|ERRNO_EDOM),
    TEST_f_f (tgamma, -max_value, qnan_value, INVALID_EXCEPTION|ERRNO_EDOM),
    TEST_f_f (tgamma, minus_infty, qnan_value, INVALID_EXCEPTION|ERRNO_EDOM),
    TEST_f_f (tgamma, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_f_f (tgamma, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_f_f (tgamma, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_f_f (tgamma, -snan_value, qnan_value, INVALID_EXCEPTION),

    AUTO_TESTS_f_f (tgamma),
  };

static void
tgamma_test (void)
{
  ALL_RM_TEST (tgamma, 0, tgamma_test_data, RUN_TEST_LOOP_f_f, END);
}

static void
do_test (void)
{
  tgamma_test ();
}

/*
 * Local Variables:
 * mode:c
 * End:
 */
