/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by
 *             Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 */

#include "zeitgeist-mimetypes.h"
#include "zeitgeist-ontology-interpretations.h"
#include "zeitgeist-ontology-manifestations.h"

static void _ensure_mimes_loaded   (void);
static void _ensure_schemes_loaded (void);

static gboolean    mimes_loaded   = FALSE;
static gboolean    schemes_loaded = FALSE;
static GHashTable *mimes          = NULL;
static GSList     *mimes_r        = NULL;
static GSList     *schemes        = NULL;

typedef struct
{
  gchar *scheme;
  gchar *manifestation_uri;
} UriScheme;

typedef struct
{
  GRegex *regex;
  gchar *interpretation_uri;
} MimeRegex;

static MimeRegex*
mime_regex_new (const gchar *mimetype_regex, const gchar *interpretation_uri)
{
  MimeRegex *m = g_slice_new (MimeRegex);
  m->regex = g_regex_new (mimetype_regex, 0, 0, NULL);
  m->interpretation_uri = g_strdup (interpretation_uri);
  return m;
}

/*static void
mime_regex_free (MimeRegex *m)
{
  g_regex_unref (m->regex);
  g_free (m->interpretation_uri);
  g_slice_free (MimeRegex, m);
}*/

static UriScheme*
uri_scheme_new (const gchar *uri_scheme, const gchar *manifestation_uri)
{
  UriScheme *s = g_slice_new (UriScheme);
  s->scheme = g_strdup (uri_scheme);
  s->manifestation_uri = g_strdup (manifestation_uri);
  return s;
}

/*static void
uri_scheme_free (UriScheme *s)
{
  g_free (s->scheme);
  g_free (s->manifestation_uri);
  g_slice_free (UriScheme, s);
}*/



/**
 * zeitgeist_register_mimetype:
 * @mimetype: A mimetype string. Fx. <emphasis>text/plain</emphasis>
 * @interpretation_uri: A URI defining the interpretation type to associate with @mimetype
 *
 * Associate a mimetype with a given interpretation type. Registered mimetypes
 * can be looked up with zeitgeist_interpretation_for_mimetype(). You can
 * register a regular expression as mimetype if you instead of this function
 * invoke zeitgeist_register_mimetype_regex().
 *
 * Mimetypes are first looked up by their exact name and then if none is
 * found the regular expressions will be checked as fallbacks.
 *
 * This library will install a wide range a common mimetypes for you, so unless
 * you have very specific needs you will normally not have to call this
 * function.
 *
 * See the list of common
 * <link linkend="zeitgeist-1.0-Interpretation-Ontology">interpretation types</link>.
 */
void
zeitgeist_register_mimetype (const gchar *mimetype,
                             const gchar *interpretation_uri)
{
  if (mimes == NULL)
    mimes = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  g_hash_table_insert (mimes, g_strdup (mimetype),
                       g_strdup (interpretation_uri));
}

/**
 * zeitgeist_register_mimetype_regex:
 * @mimetype_regex: A regular expression matching a certain range of mimetypes.
 *                  Fx. <emphasis>text/.*</emphasis> to match all
 *                 <emphasis>text</emphasis> sub types.
 * @interpretation_uri: A URI defining the interpretation type to associate with
 *                     the matched mimetypes
 *
 * Associate a range of mimetypes with a given interpretation type.
 * Registered mimetypes can be looked up with
 * zeitgeist_interpretation_for_mimetype(). If you only need to register one
 * specific mimetype it is more efficient if you instead of this function
 * call zeitgeist_register_mimetype().
 *
 * Mimetypes are first looked up by their exact name and then if none is
 * found the regular expressions will be checked as fallbacks.
 *
 * This library will install a wide range a common mimetypes for you, so unless
 * you have very specific needs you will normally not have to call this
 * function.
 *
 * See the list of common
 * <link linkend="zeitgeist-1.0-Interpretation-Ontology">interpretation types</link>.
 */ 
void
zeitgeist_register_mimetype_regex (const gchar *mimetype_regex,
                                    const gchar *interpretation_uri)
{
  mimes_r = g_slist_append (mimes_r,
                            mime_regex_new (mimetype_regex,
                                            interpretation_uri));
}

/**
 * zeitgeist_interpretation_for_mimetype:
 * @mimetype: A mimetype string. Fx. <emphasis>text/plain</emphasis>
 *
 * Look up the interpretation type associated with @mimetype. Please see the
 * list of common
 * <link linkend="zeitgeist-1.0-Interpretation-Ontology">interpretation types</link>.
 *
 * Returns: A URI defining the interpretation type associated with @mimetype or
 *          %NULL in case @mimetype is unknown.
 */ 
const gchar*
zeitgeist_interpretation_for_mimetype (const gchar *mimetype)
{
  const gchar *result;
  GSList      *iter;
  
  _ensure_mimes_loaded();

  /* First look in our hash table */
  result = g_hash_table_lookup (mimes,
                                mimetype);
  if (result != NULL)
    return result;

  /* Check our regexes */
  for (iter = mimes_r; iter; iter = iter->next)
    {
      MimeRegex *m = (MimeRegex*) iter->data;
      if (g_regex_match (m->regex, mimetype, 0, NULL))
        return m->interpretation_uri;
    }

  return NULL;
}

/**
 * zeitgeist_register_uri_scheme:
 * @uri_scheme: A URI scheme such as <emphasis>http://</emphasis>
 * @manifestation_uri: A URI defining the manifestation type to associate with
 *                     @uri_scheme
 *
 * Associate a URI scheme with a given manifestation type.
 * You can find the manifestation type of a given URI by passing it to
 * zeitgeist_manifestation_for_uri().
 *
 * This library will install a range a common URI schemes for you, so unless
 * you have very specific needs you will normally not have to call this
 * function.
 *
 * See the list of common
 * <link linkend="zeitgeist-1.0-Manifestation-Ontology">manifestation types</link>.
 */
void
zeitgeist_register_uri_scheme (const gchar *uri_scheme,
                               const gchar *manifestation_uri)
{
  schemes = g_slist_append (schemes,
                            uri_scheme_new (uri_scheme, manifestation_uri));
}

/**
 * zeitgeist_manifestation_for_uri:
 * @uri: A URI
 * 
 * Look up a manifestation type for a given URI. Eg. if you pass in
 * <emphasis>file:///tmp/foo.txt</emphasis> you will get back
 * #ZEITGEIST_NFO_FILE_DATA_OBJECT.
 *
 * See the list of common
 * <link linkend="zeitgeist-1.0-Manifestation-Ontology">manifestation types</link>.
 *
 * Returns: A manifestation type for @uri or %NULL in case no suitable
 *          manifestation type is known
 */
const gchar*
zeitgeist_manifestation_for_uri (const gchar *uri)
{
  GSList      *iter;
  
  _ensure_schemes_loaded();

  for (iter = schemes; iter; iter = iter->next)
    {
      UriScheme *s = (UriScheme*) iter->data;
      if (g_str_has_prefix (uri, s->scheme))
        return s->manifestation_uri;
    }

  return NULL;
}

static void
_ensure_mimes_loaded (void)
{
  if (mimes_loaded) return;

  zeitgeist_register_mimetype ("application/ecmascript", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/javascript", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/json", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/ms-excel", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype ("application/ms-powerpoint", ZEITGEIST_NFO_PRESENTATION);
  zeitgeist_register_mimetype ("application/msexcel", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype ("application/msword", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/ogg", ZEITGEIST_NFO_AUDIO);
  zeitgeist_register_mimetype ("application/pdf", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/postscript", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/ps", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/rtf", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/soap+xml", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/vnd.corel-draw", ZEITGEIST_NFO_VECTOR_IMAGE);
  zeitgeist_register_mimetype ("application/vnd.ms-excel", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype ("application/vnd.ms-powerpoint", ZEITGEIST_NFO_PRESENTATION);
  zeitgeist_register_mimetype ("application/x-7z-compressed", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-abiword", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/x-applix-presents", ZEITGEIST_NFO_PRESENTATION);
  zeitgeist_register_mimetype ("application/x-applix-spreadsheet", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype ("application/x-applix-word", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/x-archive", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-bzip", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-bzip-compressed-tar", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-cd-image", ZEITGEIST_NFO_FILESYSTEM_IMAGE);
  zeitgeist_register_mimetype ("application/x-compressed-tar", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-csh", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-deb", ZEITGEIST_NFO_SOFTWARE);
  zeitgeist_register_mimetype ("application/x-designer", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-desktop", ZEITGEIST_NFO_SOFTWARE);
  zeitgeist_register_mimetype ("application/x-dia-diagram", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-executable", ZEITGEIST_NFO_SOFTWARE);
  zeitgeist_register_mimetype ("application/x-fluid", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-glade", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-gnucash", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype ("application/x-gnumeric", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype ("application/x-gzip", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-java-archive", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-javascript", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-killustrator", ZEITGEIST_NFO_VECTOR_IMAGE);
  zeitgeist_register_mimetype ("application/x-kpresenter", ZEITGEIST_NFO_PRESENTATION);
  zeitgeist_register_mimetype ("application/x-kspread", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype ("application/x-kword", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("application/x-lzma", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-lzma-compressed-tar", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/x-m4", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-ms-dos-executable", ZEITGEIST_NFO_SOFTWARE);
  zeitgeist_register_mimetype ("application/x-perl", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-php", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-rpm", ZEITGEIST_NFO_SOFTWARE);
  zeitgeist_register_mimetype ("application/x-ruby", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-shellscript", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-shockwave-flash", ZEITGEIST_NFO_EXECUTABLE);
  zeitgeist_register_mimetype ("application/x-sql", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/x-stuffit", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("application/xhtml+xml", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/xml", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/xml-dtd", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("application/zip", ZEITGEIST_NFO_ARCHIVE);
  zeitgeist_register_mimetype ("audio/x-scpls", ZEITGEIST_NFO_MEDIA_LIST);
  zeitgeist_register_mimetype ("image/gif", ZEITGEIST_NFO_RASTER_IMAGE);
  zeitgeist_register_mimetype ("image/jpeg", ZEITGEIST_NFO_RASTER_IMAGE);
  zeitgeist_register_mimetype ("image/pjpeg", ZEITGEIST_NFO_RASTER_IMAGE);
  zeitgeist_register_mimetype ("image/png", ZEITGEIST_NFO_RASTER_IMAGE);
  zeitgeist_register_mimetype ("image/svg+xml", ZEITGEIST_NFO_VECTOR_IMAGE);
  zeitgeist_register_mimetype ("image/tiff", ZEITGEIST_NFO_RASTER_IMAGE);
  zeitgeist_register_mimetype ("image/vnd.djvu", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("image/vnd.microsoft.icon", ZEITGEIST_NFO_ICON);
  zeitgeist_register_mimetype ("image/x-xcf", ZEITGEIST_NFO_RASTER_IMAGE);
  zeitgeist_register_mimetype ("inode/directory", ZEITGEIST_NFO_FOLDER);
  zeitgeist_register_mimetype ("message/alternative", ZEITGEIST_NMO_EMAIL);
  zeitgeist_register_mimetype ("message/partial", ZEITGEIST_NMO_EMAIL);
  zeitgeist_register_mimetype ("message/related", ZEITGEIST_NMO_EMAIL);
  zeitgeist_register_mimetype ("text/css", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/csv", ZEITGEIST_NFO_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("text/html", ZEITGEIST_NFO_HTML_DOCUMENT);
  zeitgeist_register_mimetype ("text/javascript", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/plain", ZEITGEIST_NFO_TEXT_DOCUMENT);
  zeitgeist_register_mimetype ("text/vcard", ZEITGEIST_NCO_CONTACT);
  zeitgeist_register_mimetype ("text/x-c", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-c++", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-c++src", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-chdr", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-copying", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-credits", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-csharp", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-csrc", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-dsrc", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-eiffel", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-gettext-translation", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-gettext-translation-template", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-haskell", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-idl", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-java", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-jquery-tmpl", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-latex", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-lisp", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-lua", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-m4", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-makefile", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-objcsrc", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-ocaml", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-pascal", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-patch", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-python", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-sql", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-tcl", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-tex", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-troff", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-vala", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/x-vhdl", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype ("text/xml", ZEITGEIST_NFO_SOURCE_CODE);
  zeitgeist_register_mimetype_regex ("application/vnd.oasis.opendocument.text.*", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype_regex ("application/vnd.oasis.opendocument.presentation.*", ZEITGEIST_NFO_PRESENTATION);
  zeitgeist_register_mimetype_regex ("application/vnd.oasis.opendocument.spreadsheet.*", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype_regex ("application/vnd.oasis.opendocument.graphics.*", ZEITGEIST_NFO_VECTOR_IMAGE);
  zeitgeist_register_mimetype_regex ("application/vnd\\..*", ZEITGEIST_NFO_DOCUMENT);
  zeitgeist_register_mimetype_regex ("application/x-applix-.*", ZEITGEIST_NFO_DOCUMENT);
  zeitgeist_register_mimetype_regex ("application/vnd.ms-excel.*", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype_regex ("application/vnd.ms-powerpoint.*", ZEITGEIST_NFO_PRESENTATION);
  zeitgeist_register_mimetype_regex ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.*", ZEITGEIST_NFO_SPREADSHEET);
  zeitgeist_register_mimetype_regex ("application/vnd.openxmlformats-officedocument.presentationml.presentation.*", ZEITGEIST_NFO_PRESENTATION);
  zeitgeist_register_mimetype_regex ("application/vnd.openxmlformats-officedocument.wordprocessingml.document.*", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype_regex (".*/x-dvi", ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT);
  zeitgeist_register_mimetype_regex ("image/.*", ZEITGEIST_NFO_IMAGE);
  zeitgeist_register_mimetype_regex ("audio/.*", ZEITGEIST_NFO_AUDIO);
  zeitgeist_register_mimetype_regex ("video/.*", ZEITGEIST_NFO_VIDEO);

  mimes_loaded = TRUE;
}

static void
_ensure_schemes_loaded (void)
{
  if (schemes_loaded) return;

  zeitgeist_register_uri_scheme ("file://", ZEITGEIST_NFO_FILE_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("http://", ZEITGEIST_NFO_WEB_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("https://", ZEITGEIST_NFO_WEB_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("ssh://", ZEITGEIST_NFO_REMOTE_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("sftp://", ZEITGEIST_NFO_REMOTE_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("ftp://", ZEITGEIST_NFO_REMOTE_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("dav://", ZEITGEIST_NFO_REMOTE_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("davs://", ZEITGEIST_NFO_REMOTE_DATA_OBJECT);
  zeitgeist_register_uri_scheme ("smb://", ZEITGEIST_NFO_REMOTE_DATA_OBJECT);

  schemes_loaded = TRUE;
}

