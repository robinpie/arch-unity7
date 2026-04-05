/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "Validator.h"

namespace nux
{

  Validator::Validator()
#if !defined(NUX_OS_WINDOWS)
    : _regexp(nullptr)
#endif
  {
  }

  Validator::~Validator()
  {
#if !defined(NUX_OS_WINDOWS)
    if (_regexp)
      pcre2_code_free(_regexp);
#endif
  }

  bool Validator::InitRegExp()
  {
#if defined(NUX_OS_WINDOWS)
    regex_ = _regexp_str.c_str();
    return true;
#else
    int        errorcode;
    PCRE2_SIZE erroroffset;
    _regexp = pcre2_compile(
      reinterpret_cast<PCRE2_SPTR>(_regexp_str.c_str()),  /* pattern */
      PCRE2_ZERO_TERMINATED,                              /* pattern length */
      PCRE2_MULTILINE,                                    /* option bits */
      &errorcode,                                         /* error code */
      &erroroffset,                                       /* error offset */
      nullptr);                                           /* compile context */

    if (!_regexp)
    {
      PCRE2_UCHAR buffer[256];
      pcre2_get_error_message(errorcode, buffer, sizeof(buffer) / sizeof(buffer[0]));
      nuxDebugMsg("[Validator::InitRegExp] Invalid regular expression '%s' "
                  "at offset %d: %s",
                  _regexp_str.c_str(), (int)erroroffset, buffer);
      return false;
    }    
    return true;
#endif
  }

  Validator::State Validator::Validate(const char* str) const
  {
#if defined(NUX_OS_WINDOWS)
    if (str == NULL)
      return Validator::Invalid;
    std::string search_string = str;

    if (std::regex_match(search_string.begin(), search_string.end(), regex_))
    {
      return Validator::Acceptable;
    }
    return Validator::Acceptable;
#else
    if (!_regexp || !str)
      return Validator::Invalid;

    pcre2_match_data *match_data =
        pcre2_match_data_create_from_pattern(_regexp, nullptr);

    int rc = pcre2_match(
        _regexp,                                   /* compiled pattern */
        reinterpret_cast<PCRE2_SPTR>(str),         /* subject string */
        PCRE2_ZERO_TERMINATED,                     /* auto-determine length */
        0,                                         /* start offset */
        0,                                         /* match options */
        match_data,                                /* match data block */
        nullptr);                                  /* match context */

    pcre2_match_data_free(match_data);

    if (rc < 0)
    {
      return Validator::Invalid;
    }
    return Validator::Acceptable;
#endif
  }

}
