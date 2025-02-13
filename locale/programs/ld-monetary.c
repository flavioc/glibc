/* Copyright (C) 1995-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <byteswap.h>
#include <langinfo.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/uio.h>

#include <assert.h>

#include "localedef.h"
#include "linereader.h"
#include "localeinfo.h"
#include "locfile.h"


/* The real definition of the struct for the LC_MONETARY locale.  */
struct locale_monetary_t
{
  const char *int_curr_symbol;
  const char *currency_symbol;
  const char *mon_decimal_point;
  const char *mon_thousands_sep;
  uint32_t mon_decimal_point_wc;
  uint32_t mon_thousands_sep_wc;
  char *mon_grouping;
  size_t mon_grouping_len;
  const char *positive_sign;
  const char *negative_sign;
  signed char int_frac_digits;
  signed char frac_digits;
  signed char p_cs_precedes;
  signed char p_sep_by_space;
  signed char n_cs_precedes;
  signed char n_sep_by_space;
  signed char p_sign_posn;
  signed char n_sign_posn;
  signed char int_p_cs_precedes;
  signed char int_p_sep_by_space;
  signed char int_n_cs_precedes;
  signed char int_n_sep_by_space;
  signed char int_p_sign_posn;
  signed char int_n_sign_posn;
  const char *duo_int_curr_symbol;
  const char *duo_currency_symbol;
  signed char duo_int_frac_digits;
  signed char duo_frac_digits;
  signed char duo_p_cs_precedes;
  signed char duo_p_sep_by_space;
  signed char duo_n_cs_precedes;
  signed char duo_n_sep_by_space;
  signed char duo_p_sign_posn;
  signed char duo_n_sign_posn;
  signed char duo_int_p_cs_precedes;
  signed char duo_int_p_sep_by_space;
  signed char duo_int_n_cs_precedes;
  signed char duo_int_n_sep_by_space;
  signed char duo_int_p_sign_posn;
  signed char duo_int_n_sign_posn;
  uint32_t uno_valid_from;
  uint32_t uno_valid_to;
  uint32_t duo_valid_from;
  uint32_t duo_valid_to;
  uint32_t conversion_rate[2];
  char *crncystr;
};


/* The content iof the field int_curr_symbol has to be taken from
   ISO-4217.  We test for correct values.  */
#define DEFINE_INT_CURR(str) str,
static const char *const valid_int_curr[] =
  {
#   include "../iso-4217.def"
  };
#define NR_VALID_INT_CURR ((sizeof (valid_int_curr) \
			    / sizeof (valid_int_curr[0])))
#undef DEFINE_INT_CURR


/* Prototypes for local functions.  */
static int curr_strcmp (const char *s1, const char **s2);


static void
monetary_startup (struct linereader *lr, struct localedef_t *locale,
		  int ignore_content)
{
  if (!ignore_content)
    {
      struct locale_monetary_t *monetary;

      locale->categories[LC_MONETARY].monetary = monetary =
	(struct locale_monetary_t *) xmalloc (sizeof (*monetary));

      memset (monetary, '\0', sizeof (struct locale_monetary_t));

      monetary->mon_grouping = NULL;
      monetary->mon_grouping_len = 0;

      monetary->int_frac_digits = -2;
      monetary->frac_digits = -2;
      monetary->p_cs_precedes = -2;
      monetary->p_sep_by_space = -2;
      monetary->n_cs_precedes = -2;
      monetary->n_sep_by_space = -2;
      monetary->p_sign_posn = -2;
      monetary->n_sign_posn = -2;
      monetary->int_p_cs_precedes = -2;
      monetary->int_p_sep_by_space = -2;
      monetary->int_n_cs_precedes = -2;
      monetary->int_n_sep_by_space = -2;
      monetary->int_p_sign_posn = -2;
      monetary->int_n_sign_posn = -2;
      monetary->duo_int_frac_digits = -2;
      monetary->duo_frac_digits = -2;
      monetary->duo_p_cs_precedes = -2;
      monetary->duo_p_sep_by_space = -2;
      monetary->duo_n_cs_precedes = -2;
      monetary->duo_n_sep_by_space = -2;
      monetary->duo_p_sign_posn = -2;
      monetary->duo_n_sign_posn = -2;
      monetary->duo_int_p_cs_precedes = -2;
      monetary->duo_int_p_sep_by_space = -2;
      monetary->duo_int_n_cs_precedes = -2;
      monetary->duo_int_n_sep_by_space = -2;
      monetary->duo_int_p_sign_posn = -2;
      monetary->duo_int_n_sign_posn = -2;
    }

  if (lr != NULL)
    {
      lr->translate_strings = 1;
      lr->return_widestr = 0;
    }
}


void
monetary_finish (struct localedef_t *locale, const struct charmap_t *charmap)
{
  struct locale_monetary_t *monetary
    = locale->categories[LC_MONETARY].monetary;
  int nothing = 0;

  /* Now resolve copying and also handle completely missing definitions.  */
  if (monetary == NULL)
    {
      /* First see whether we were supposed to copy.  If yes, find the
	 actual definition.  */
      if (locale->copy_name[LC_MONETARY] != NULL)
	{
	  /* Find the copying locale.  This has to happen transitively since
	     the locale we are copying from might also copying another one.  */
	  struct localedef_t *from = locale;

	  do
	    from = find_locale (LC_MONETARY, from->copy_name[LC_MONETARY],
				from->repertoire_name, charmap);
	  while (from->categories[LC_MONETARY].monetary == NULL
		 && from->copy_name[LC_MONETARY] != NULL);

	  monetary = locale->categories[LC_MONETARY].monetary
	    = from->categories[LC_MONETARY].monetary;
	}

      /* If there is still no definition issue a warning and create an
	 empty one.  */
      if (monetary == NULL)
	{
	  record_warning (_("\
No definition for %s category found"), "LC_MONETARY");
	  monetary_startup (NULL, locale, 0);
	  monetary = locale->categories[LC_MONETARY].monetary;
	  nothing = 1;
	}
    }

  /* Generally speaking there are 3 standards the define the default,
     warning, and error behaviour of LC_MONETARY.  They are ISO/IEC TR 30112,
     ISO/IEC 9899:2018 (ISO C17), and POSIX.1-2017.  Within 30112 we have the
     definition of a standard i18n FDCC-set, which for LC_MONETARY has the
     following default values:
	int_curr_symbol		""
	currency_symbol		""
	mon_decimal_point	"<U002C>" i.e. ","
	mon_thousand_sep	""
	mon_grouping		"\177" i.e. CHAR_MAX
	positive_sign		""
	negative_sign		"<U002E>" i.e. "."
	int_frac_digits		-1
	frac_digits		-1
	p_cs_precedes		-1
	p_sep_by_space		-1
	n_cs_precedes		-1
	n_sep_by_space		-1
	p_sign_posn		-1
	n_sign_posn		-1
    Under 30112 a keyword that is not provided implies an empty string ""
    for string values or a -1 for integer values, and indicates the value
    is unspecified with no default implied.  No errors are considered.
    The exception is mon_grouping which is a string with a terminating
    CHAR_MAX.
    For POSIX Issue 7 we have:
    https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap07.html
    and again values not provided default to "" or -1, and indicate the value
    is not available to the locale.  The exception is mon_grouping which is
    a string with a terminating CHAR_MAX.  For the POSIX locale the values of
    LC_MONETARY should be:
	int_curr_symbol		""
	currency_symbol		""
	mon_decimal_point	""
	mon_thousands_sep	""
	mon_grouping		"\177" i.e. CHAR_MAX
	positive_sign		""
	negative_sign		""
	int_frac_digits		-1
	frac_digits		-1
	p_cs_precedes		-1
	p_sep_by_space		-1
	n_cs_precedes		-1
	n_sep_by_space		-1
	p_sign_posn		-1
	n_sign_posn		-1
	int_p_cs_precedes	-1
	int_p_sep_by_space	-1
	int_n_cs_precedes	-1
	int_n_sep_by_space	-1
	int_p_sign_posn		-1
	int_n_sign_posn		-1
    Like with 30112, POSIX also considers no error if the keywords are
    missing, only that if the cateory as a whole is missing the referencing
    of the category results in unspecified behaviour.
    For ISO C17 there is no default value provided, but the localeconv
    specification in 7.11.2.1 admits that members of char * type may point
    to "" to indicate a value is not available or is of length zero.
    The exception is decimal_point (not mon_decimal_point) which must be a
    defined non-empty string.  The values of char, which are generally
    mapped to integer values in 30112 and POSIX, must be non-negative
    numbers that map to CHAR_MAX when a value is not available in the
    locale.
    In ISO C17 for the "C" locale all values are empty strings "", or
    CHAR_MAX, with the exception of decimal_point which is "." (defined
    in LC_NUMERIC).  ISO C17 makes no exception for mon_grouping like
    30112 and POSIX, but a value of "" is functionally equivalent to
    "\177" since neither defines a grouping (though the latter terminates
    the grouping).

    Lastly, we must consider the legacy C/POSIX locale that implemented
    as a builtin in glibc and wether a default value mapping to the
    C/POSIX locale may benefit the user from a compatibility perspective.

    Thus given 30112, POSIX, ISO C, and the builtin C/POSIX locale we
    need to pick appropriate defaults below.   */

  /* The members of LC_MONETARY are handled in the order of their definition
     in locale/categories.def.  Please keep them in that order.  */

  /* The purpose of TEST_ELEM is to define a default value for the fields
     in the category if the field was not defined in the cateory.  If the
     category was present but we didn't see a definition for the field then
     we also issue a warning, otherwise the only warning you get is the one
     earlier when a default category is created (completely missing category).
     This missing field warning is glibc-specific since no standard requires
     this warning, but we consider it valuable to print a warning for all
     missing fields in the category.  */
#define TEST_ELEM(cat, initval) \
  if (monetary->cat == NULL)						      \
    {									      \
      if (! nothing)							      \
	record_warning (_("%s: field `%s' not defined"),		      \
			"LC_MONETARY", #cat);				      \
      monetary->cat = initval;						      \
    }

  /* Keyword: int_curr_symbol.  */
  TEST_ELEM (int_curr_symbol, "");
  /* The international currency symbol must come from ISO 4217.  */
  if (monetary->int_curr_symbol != NULL)
    {
      /* POSIX says this should be a 3-character symbol from ISO 4217
	 along with a 4th character that is a divider, but the POSIX
	 locale is documented as having a special case of "", and we
	 support that also, so allow other locales to be created with
	 a blank int_curr_symbol.  */
      int ics_len = strlen (monetary->int_curr_symbol);
      if (ics_len != 4 && ics_len != 0)
	{
	  if (! nothing)
	    record_error (0, 0, _("\
%s: value of field `int_curr_symbol' has wrong length"),
			  "LC_MONETARY");
	}
      else if (ics_len == 4)
	{ /* Check the first three characters against ISO 4217 */
	  char symbol[4];
	  strncpy (symbol, monetary->int_curr_symbol, 3);
	  symbol[3] = '\0';
	  /* A user may disable this waning for testing purposes or
	     for building a locale with a 3 letter country code that
	     was not yet supported in our ISO 4217 list.
	     See the use of --no-warnings=intcurrsym.  */
	  if (bsearch (symbol, valid_int_curr, NR_VALID_INT_CURR,
		       sizeof (const char *),
		       (comparison_fn_t) curr_strcmp) == NULL
	      && warn_int_curr_symbol)
	    record_warning (_("\
%s: value of field `int_curr_symbol' does \
not correspond to a valid name in ISO 4217 [--no-warnings=intcurrsym]"),
			    "LC_MONETARY");
	}
    }

  /* Keyword: currency_symbol */
  TEST_ELEM (currency_symbol, "");

  /* Keyword: mon_decimal_point */
  /* ISO C17 7.11.2.1.3 explicitly allows mon_decimal_point to be the
     empty string e.g. "".  This indicates the value is not available in the
     current locale or is of zero length.  However, if the value was never
     defined then we issue a warning and use a glibc-specific default.  ISO
     30112 in the i18n FDCC-Set uses <U002C> ",", and POSIX Issue 7 in the
     POSIX locale uses "".  It is specific to glibc that the default is <U002E>
     "."; we retain this existing behaviour for backwards compatibility.  */
  if (monetary->mon_decimal_point == NULL)
    {
      if (! nothing)
	record_warning (_("%s: field `%s' not defined, using defaults"),
			"LC_MONETARY", "mon_decimal_point");
      monetary->mon_decimal_point = ".";
      monetary->mon_decimal_point_wc = L'.';
    }

  /* Keyword: mon_thousands_sep */
  if (monetary->mon_thousands_sep == NULL)
    {
      if (! nothing)
	record_warning (_("%s: field `%s' not defined, using defaults"),
			"LC_MONETARY", "mon_thousands_sep");
      monetary->mon_thousands_sep = "";
      monetary->mon_thousands_sep_wc = L'\0';
    }

  /* Keyword: mon_grouping */
  if (monetary->mon_grouping_len == 0)
    {
      if (! nothing)
	record_warning (_("%s: field `%s' not defined"),
			"LC_MONETARY", "mon_grouping");
      /* Missing entries are given 1 element in their bytearray with
	 a value of CHAR_MAX which indicates that "No further grouping
	 is to be performed" (functionally equivalent to ISO C's "C"
	 locale default of ""). */
      monetary->mon_grouping = (char *) "\177";
      monetary->mon_grouping_len = 1;
    }

  /* Keyword: positive_sign */
  TEST_ELEM (positive_sign, "");

  /* Keyword: negative_sign */
  TEST_ELEM (negative_sign, "");

#undef TEST_ELEM
#define TEST_ELEM(cat, min, max, initval) \
  if (monetary->cat == -2)						      \
    {									      \
       if (! nothing)							      \
	 record_warning (_("%s: field `%s' not defined"),		      \
			 "LC_MONETARY", #cat);				      \
       monetary->cat = initval;						      \
    }									      \
  else if ((monetary->cat < min || monetary->cat > max)			      \
	   && min < max							      \
	   && !be_quiet && !nothing)					      \
    record_error (0, 0, _("\
%s: value for field `%s' must be in range %d...%d"),			      \
		  "LC_MONETARY", #cat, min, max)

  TEST_ELEM (int_frac_digits, 1, 0, -1);
  TEST_ELEM (frac_digits, 1, 0, -1);
  TEST_ELEM (p_cs_precedes, -1, 1, -1);
  TEST_ELEM (p_sep_by_space, -1, 2, -1);
  TEST_ELEM (n_cs_precedes, -1, 1, -1);
  TEST_ELEM (n_sep_by_space, -1, 2, -1);
  TEST_ELEM (p_sign_posn, -1, 4, -1);
  TEST_ELEM (n_sign_posn, -1, 4, -1);

  /* Keyword: crncystr */
  monetary->crncystr = (char *) xmalloc (strlen (monetary->currency_symbol)
					 + 2);
  monetary->crncystr[0] = monetary->p_cs_precedes ? '-' : '+';
  strcpy (&monetary->crncystr[1], monetary->currency_symbol);

#undef TEST_ELEM
#define TEST_ELEM(cat, alt, min, max) \
  if (monetary->cat == -2)						      \
    monetary->cat = monetary->alt;					      \
  else if ((monetary->cat < min || monetary->cat > max)	&& ! nothing)	      \
    record_error (0, 0, _("\
%s: value for field `%s' must be in range %d...%d"),			      \
		  "LC_MONETARY", #cat, min, max)

  TEST_ELEM (int_p_cs_precedes, p_cs_precedes, -1, 1);
  TEST_ELEM (int_p_sep_by_space, p_sep_by_space, -1, 2);
  TEST_ELEM (int_n_cs_precedes, n_cs_precedes, -1, 1);
  TEST_ELEM (int_n_sep_by_space, n_sep_by_space, -1, 2);
  TEST_ELEM (int_p_sign_posn, p_sign_posn, -1, 4);
  TEST_ELEM (int_n_sign_posn, n_sign_posn, -1, 4);

  /* The non-POSIX.2 extensions are optional.  */
  if (monetary->duo_int_curr_symbol == NULL)
    monetary->duo_int_curr_symbol = monetary->int_curr_symbol;
  if (monetary->duo_currency_symbol == NULL)
    monetary->duo_currency_symbol = monetary->currency_symbol;

  if (monetary->duo_int_frac_digits == -2)
    monetary->duo_int_frac_digits = monetary->int_frac_digits;
  if (monetary->duo_frac_digits == -2)
    monetary->duo_frac_digits = monetary->frac_digits;

  TEST_ELEM (duo_p_cs_precedes, p_cs_precedes, -1, 1);
  TEST_ELEM (duo_p_sep_by_space, p_sep_by_space, -1, 2);
  TEST_ELEM (duo_n_cs_precedes, n_cs_precedes, -1, 1);
  TEST_ELEM (duo_n_sep_by_space, n_sep_by_space, -1, 2);
  TEST_ELEM (duo_int_p_cs_precedes, int_p_cs_precedes, -1, 1);
  TEST_ELEM (duo_int_p_sep_by_space, int_p_sep_by_space, -1, 2);
  TEST_ELEM (duo_int_n_cs_precedes, int_n_cs_precedes, -1, 1);
  TEST_ELEM (duo_int_n_sep_by_space, int_n_sep_by_space, -1, 2);
  TEST_ELEM (duo_p_sign_posn, p_sign_posn, -1, 4);
  TEST_ELEM (duo_n_sign_posn, n_sign_posn, -1, 4);
  TEST_ELEM (duo_int_p_sign_posn, int_p_sign_posn, -1, 4);
  TEST_ELEM (duo_int_n_sign_posn, int_n_sign_posn, -1, 4);

  if (monetary->uno_valid_from == 0)
    monetary->uno_valid_from = 10101;
  if (monetary->uno_valid_to == 0)
    monetary->uno_valid_to = 99991231;
  if (monetary->duo_valid_from == 0)
    monetary->duo_valid_from = 10101;
  if (monetary->duo_valid_to == 0)
    monetary->duo_valid_to = 99991231;

  /* Keyword: conversion_rate */
  if (monetary->conversion_rate[0] == 0)
    {
      monetary->conversion_rate[0] = 1;
      monetary->conversion_rate[1] = 1;
    }

  /* A value for monetary-decimal-point-wc was set when
     monetary_decimal_point was set, likewise for monetary-thousands-sep-wc.  */
}


void
monetary_output (struct localedef_t *locale, const struct charmap_t *charmap,
		 const char *output_path)
{
  struct locale_monetary_t *monetary
    = locale->categories[LC_MONETARY].monetary;
  struct locale_file file;

  init_locale_data (&file, _NL_ITEM_INDEX (_NL_NUM_LC_MONETARY));
  add_locale_string (&file, monetary->int_curr_symbol);
  add_locale_string (&file, monetary->currency_symbol);
  add_locale_string (&file, monetary->mon_decimal_point);
  add_locale_string (&file, monetary->mon_thousands_sep);
  add_locale_raw_data (&file, monetary->mon_grouping,
		       monetary->mon_grouping_len);
  add_locale_string (&file, monetary->positive_sign);
  add_locale_string (&file, monetary->negative_sign);
  add_locale_char (&file, monetary->int_frac_digits);
  add_locale_char (&file, monetary->frac_digits);
  add_locale_char (&file, monetary->p_cs_precedes);
  add_locale_char (&file, monetary->p_sep_by_space);
  add_locale_char (&file, monetary->n_cs_precedes);
  add_locale_char (&file, monetary->n_sep_by_space);
  add_locale_char (&file, monetary->p_sign_posn);
  add_locale_char (&file, monetary->n_sign_posn);
  add_locale_string (&file, monetary->crncystr);
  add_locale_char (&file, monetary->int_p_cs_precedes);
  add_locale_char (&file, monetary->int_p_sep_by_space);
  add_locale_char (&file, monetary->int_n_cs_precedes);
  add_locale_char (&file, monetary->int_n_sep_by_space);
  add_locale_char (&file, monetary->int_p_sign_posn);
  add_locale_char (&file, monetary->int_n_sign_posn);
  add_locale_string (&file, monetary->duo_int_curr_symbol);
  add_locale_string (&file, monetary->duo_currency_symbol);
  add_locale_char (&file, monetary->duo_int_frac_digits);
  add_locale_char (&file, monetary->duo_frac_digits);
  add_locale_char (&file, monetary->duo_p_cs_precedes);
  add_locale_char (&file, monetary->duo_p_sep_by_space);
  add_locale_char (&file, monetary->duo_n_cs_precedes);
  add_locale_char (&file, monetary->duo_n_sep_by_space);
  add_locale_char (&file, monetary->duo_int_p_cs_precedes);
  add_locale_char (&file, monetary->duo_int_p_sep_by_space);
  add_locale_char (&file, monetary->duo_int_n_cs_precedes);
  add_locale_char (&file, monetary->duo_int_n_sep_by_space);
  add_locale_char (&file, monetary->duo_p_sign_posn);
  add_locale_char (&file, monetary->duo_n_sign_posn);
  add_locale_char (&file, monetary->duo_int_p_sign_posn);
  add_locale_char (&file, monetary->duo_int_n_sign_posn);
  add_locale_uint32 (&file, monetary->uno_valid_from);
  add_locale_uint32 (&file, monetary->uno_valid_to);
  add_locale_uint32 (&file, monetary->duo_valid_from);
  add_locale_uint32 (&file, monetary->duo_valid_to);
  add_locale_uint32_array (&file, monetary->conversion_rate, 2);
  add_locale_uint32 (&file, monetary->mon_decimal_point_wc);
  add_locale_uint32 (&file, monetary->mon_thousands_sep_wc);
  add_locale_string (&file, charmap->code_set_name);
  write_locale_data (output_path, LC_MONETARY, "LC_MONETARY", &file);
}


static int
curr_strcmp (const char *s1, const char **s2)
{
  return strcmp (s1, *s2);
}


/* The parser for the LC_MONETARY section of the locale definition.  */
void
monetary_read (struct linereader *ldfile, struct localedef_t *result,
	       const struct charmap_t *charmap, const char *repertoire_name,
	       int ignore_content)
{
  struct repertoire_t *repertoire = NULL;
  struct locale_monetary_t *monetary;
  struct token *now;
  enum token_t nowtok;

  /* Get the repertoire we have to use.  */
  if (repertoire_name != NULL)
    repertoire = repertoire_read (repertoire_name);

  /* The rest of the line containing `LC_MONETARY' must be free.  */
  lr_ignore_rest (ldfile, 1);

  do
    {
      now = lr_token (ldfile, charmap, result, NULL, verbose);
      nowtok = now->tok;
    }
  while (nowtok == tok_eol);

  /* If we see `copy' now we are almost done.  */
  if (nowtok == tok_copy)
    {
      handle_copy (ldfile, charmap, repertoire_name, result, tok_lc_monetary,
		   LC_MONETARY, "LC_MONETARY", ignore_content);
      return;
    }

  /* Prepare the data structures.  */
  monetary_startup (ldfile, result, ignore_content);
  monetary = result->categories[LC_MONETARY].monetary;

  while (1)
    {
      /* Of course we don't proceed beyond the end of file.  */
      if (nowtok == tok_eof)
	break;

      /* Ignore empty lines.  */
      if (nowtok == tok_eol)
	{
	  now = lr_token (ldfile, charmap, result, NULL, verbose);
	  nowtok = now->tok;
	  continue;
	}

      switch (nowtok)
	{
#define STR_ELEM(cat) \
	case tok_##cat:							      \
	  /* Ignore the rest of the line if we don't need the input of	      \
	     this line.  */						      \
	  if (ignore_content)						      \
	    {								      \
	      lr_ignore_rest (ldfile, 0);				      \
	      break;							      \
	    }								      \
									      \
	  now = lr_token (ldfile, charmap, result, NULL, verbose);	      \
	  if (now->tok != tok_string)					      \
	    goto err_label;						      \
	  else if (monetary->cat != NULL)				      \
	    lr_error (ldfile, _("%s: field `%s' declared more than once"),    \
		      "LC_MONETARY", #cat);				      \
	  else if (!ignore_content && now->val.str.startmb == NULL)	      \
	    {								      \
	      lr_error (ldfile, _("\
%s: unknown character in field `%s'"), "LC_MONETARY", #cat);		      \
	      monetary->cat = "";					      \
	    }								      \
	  else if (!ignore_content)					      \
	    monetary->cat = now->val.str.startmb;			      \
	  lr_ignore_rest (ldfile, 1);					      \
	  break

	  STR_ELEM (int_curr_symbol);
	  STR_ELEM (currency_symbol);
	  STR_ELEM (positive_sign);
	  STR_ELEM (negative_sign);
	  STR_ELEM (duo_int_curr_symbol);
	  STR_ELEM (duo_currency_symbol);

#define STR_ELEM_WC(cat) \
	case tok_##cat:							      \
	  /* Ignore the rest of the line if we don't need the input of	      \
	     this line.  */						      \
	  if (ignore_content)						      \
	    {								      \
	      lr_ignore_rest (ldfile, 0);				      \
	      break;							      \
	    }								      \
									      \
	  ldfile->return_widestr = 1;					      \
	  now = lr_token (ldfile, charmap, result, repertoire, verbose);      \
	  if (now->tok != tok_string)					      \
	    goto err_label;						      \
	  if (monetary->cat != NULL)					      \
	    lr_error (ldfile, _("\
%s: field `%s' declared more than once"), "LC_MONETARY", #cat);		      \
	  else if (!ignore_content && now->val.str.startmb == NULL)	      \
	    {								      \
	      lr_error (ldfile, _("\
%s: unknown character in field `%s'"), "LC_MONETARY", #cat);		      \
	      monetary->cat = "";					      \
	      monetary->cat##_wc = L'\0';				      \
	    }								      \
	  else if (now->val.str.startwc != NULL && now->val.str.lenwc > 2)    \
	    {								      \
	      lr_error (ldfile, _("\
%s: value for field `%s' must be a single character"), "LC_MONETARY", #cat);  \
	    }								      \
	  else if (!ignore_content)					      \
	    {								      \
	      monetary->cat = now->val.str.startmb;			      \
									      \
	      if (now->val.str.startwc != NULL)				      \
		monetary->cat##_wc = *now->val.str.startwc;		      \
	    }								      \
	  ldfile->return_widestr = 0;					      \
	  break

	  STR_ELEM_WC (mon_decimal_point);
	  STR_ELEM_WC (mon_thousands_sep);

#define INT_ELEM(cat) \
	case tok_##cat:							      \
	  /* Ignore the rest of the line if we don't need the input of	      \
	     this line.  */						      \
	  if (ignore_content)						      \
	    {								      \
	      lr_ignore_rest (ldfile, 0);				      \
	      break;							      \
	    }								      \
									      \
	  now = lr_token (ldfile, charmap, result, NULL, verbose);	      \
	  if (now->tok != tok_minus1 && now->tok != tok_number)		      \
	    goto err_label;						      \
	  else if (monetary->cat != -2)					      \
	    lr_error (ldfile, _("%s: field `%s' declared more than once"),    \
		      "LC_MONETARY", #cat);				      \
	  else if (!ignore_content)					      \
	    monetary->cat = now->tok == tok_minus1 ? -1 : now->val.num;	      \
	  break

	  INT_ELEM (int_frac_digits);
	  INT_ELEM (frac_digits);
	  INT_ELEM (p_cs_precedes);
	  INT_ELEM (p_sep_by_space);
	  INT_ELEM (n_cs_precedes);
	  INT_ELEM (n_sep_by_space);
	  INT_ELEM (p_sign_posn);
	  INT_ELEM (n_sign_posn);
	  INT_ELEM (int_p_cs_precedes);
	  INT_ELEM (int_p_sep_by_space);
	  INT_ELEM (int_n_cs_precedes);
	  INT_ELEM (int_n_sep_by_space);
	  INT_ELEM (int_p_sign_posn);
	  INT_ELEM (int_n_sign_posn);
	  INT_ELEM (duo_int_frac_digits);
	  INT_ELEM (duo_frac_digits);
	  INT_ELEM (duo_p_cs_precedes);
	  INT_ELEM (duo_p_sep_by_space);
	  INT_ELEM (duo_n_cs_precedes);
	  INT_ELEM (duo_n_sep_by_space);
	  INT_ELEM (duo_p_sign_posn);
	  INT_ELEM (duo_n_sign_posn);
	  INT_ELEM (duo_int_p_cs_precedes);
	  INT_ELEM (duo_int_p_sep_by_space);
	  INT_ELEM (duo_int_n_cs_precedes);
	  INT_ELEM (duo_int_n_sep_by_space);
	  INT_ELEM (duo_int_p_sign_posn);
	  INT_ELEM (duo_int_n_sign_posn);
	  INT_ELEM (uno_valid_from);
	  INT_ELEM (uno_valid_to);
	  INT_ELEM (duo_valid_from);
	  INT_ELEM (duo_valid_to);

	case tok_mon_grouping:
	  /* Ignore the rest of the line if we don't need the input of
	     this line.  */
	  if (ignore_content)
	    {
	      lr_ignore_rest (ldfile, 0);
	      break;
	    }

	  now = lr_token (ldfile, charmap, result, NULL, verbose);
	  if (now->tok != tok_minus1 && now->tok != tok_number)
	    goto err_label;
	  else
	    {
	      size_t act = 0;
	      size_t max = 10;
	      char *grouping = ignore_content ? NULL : xmalloc (max);

	      do
		{
		  if (act + 1 >= max)
		    {
		      max *= 2;
		      grouping = xrealloc (grouping, max);
		    }

		  if (act > 0 && grouping[act - 1] == '\177')
		    {
		      lr_error (ldfile, _("\
%s: `-1' must be last entry in `%s' field"),
				"LC_MONETARY", "mon_grouping");
		      lr_ignore_rest (ldfile, 0);
		      break;
		    }

		  if (now->tok == tok_minus1)
		    {
		      if (!ignore_content)
			grouping[act++] = '\177';
		    }
		  else if (now->val.num == 0)
		    {
		      /* A value of 0 disables grouping from here on but
			 we must not store a NUL character since this
			 terminates the string.  Use something different
			 which must not be used otherwise.  */
		      if (!ignore_content)
			grouping[act++] = '\377';
		    }
		  else if (now->val.num > 126)
		    lr_error (ldfile, _("\
%s: values for field `%s' must be smaller than 127"),
			      "LC_MONETARY", "mon_grouping");
		  else if (!ignore_content)
		    grouping[act++] = now->val.num;

		  /* Next must be semicolon.  */
		  now = lr_token (ldfile, charmap, result, NULL, verbose);
		  if (now->tok != tok_semicolon)
		    break;

		  now = lr_token (ldfile, charmap, result, NULL, verbose);
		}
	      while (now->tok == tok_minus1 || now->tok == tok_number);

	      if (now->tok != tok_eol)
		goto err_label;

	      if (!ignore_content)
		{
		  /* A single -1 means no grouping.  */
		  if (act == 1 && grouping[0] == '\177')
		    act--;
		  grouping[act++] = '\0';

		  monetary->mon_grouping = xrealloc (grouping, act);
		  monetary->mon_grouping_len = act;
		}
	    }
	  break;

	case tok_conversion_rate:
	  /* Ignore the rest of the line if we don't need the input of
	     this line.  */
	  if (ignore_content)
	    {
	      lr_ignore_rest (ldfile, 0);
	      break;
	    }

	  now = lr_token (ldfile, charmap, result, NULL, verbose);
	  if (now->tok != tok_number)
	    goto err_label;
	  if (now->val.num == 0)
	    {
	    invalid_conversion_rate:
	      lr_error (ldfile, _("conversion rate value cannot be zero"));
	      if (!ignore_content)
		{
		  monetary->conversion_rate[0] = 1;
		  monetary->conversion_rate[1] = 1;
		}
	      break;
	    }
	  if (!ignore_content)
	    monetary->conversion_rate[0] = now->val.num;
	  /* Next must be a semicolon.  */
	  now = lr_token (ldfile, charmap, result, NULL, verbose);
	  if (now->tok != tok_semicolon)
	    goto err_label;
	  /* And another number.  */
	  now = lr_token (ldfile, charmap, result, NULL, verbose);
	  if (now->tok != tok_number)
	    goto err_label;
	  if (now->val.num == 0)
	    goto invalid_conversion_rate;
	  if (!ignore_content)
	    monetary->conversion_rate[1] = now->val.num;
	  /* The rest of the line must be empty.  */
	  lr_ignore_rest (ldfile, 1);
	  break;

	case tok_end:
	  /* Next we assume `LC_MONETARY'.  */
	  now = lr_token (ldfile, charmap, result, NULL, verbose);
	  if (now->tok == tok_eof)
	    break;
	  if (now->tok == tok_eol)
	    lr_error (ldfile, _("%s: incomplete `END' line"), "LC_MONETARY");
	  else if (now->tok != tok_lc_monetary)
	    lr_error (ldfile, _("\
%1$s: definition does not end with `END %1$s'"), "LC_MONETARY");
	  lr_ignore_rest (ldfile, now->tok == tok_lc_monetary);
	  return;

	default:
	err_label:
	  SYNTAX_ERROR (_("%s: syntax error"), "LC_MONETARY");
	}

      /* Prepare for the next round.  */
      now = lr_token (ldfile, charmap, result, NULL, verbose);
      nowtok = now->tok;
    }

  /* When we come here we reached the end of the file.  */
  lr_error (ldfile, _("%s: premature end of file"), "LC_MONETARY");
}
