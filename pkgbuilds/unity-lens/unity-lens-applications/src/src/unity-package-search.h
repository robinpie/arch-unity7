/*
 * Copyright (C) 2010 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *
 */

#include <glib.h>
#include <gmenu-tree.h>
#include <unity-protocol.h>

typedef struct _UnityPackageSearcher UnityPackageSearcher;

typedef enum
{
  UNITY_PACKAGE_SEARCHTYPE_PREFIX,
  UNITY_PACKAGE_SEARCHTYPE_EXACT,
} UnityPackageSearchType;

typedef enum
{
  UNITY_PACKAGE_SORT_BY_NAME,
  UNITY_PACKAGE_SORT_BY_RELEVANCY,
} UnityPackageSort;

typedef struct
{
    GSList *results;
    gint    num_hits;
    gboolean fuzzy_search;
} UnityPackageSearchResult;

typedef struct
{
        gchar    *package_name;
        gchar    *application_name;
        gchar    *description;
        gchar    *desktop_file;
        gchar    *icon;
        gchar    *price;
        gboolean  needs_purchase;
        gint      relevancy;
        gboolean  is_master_scope;
} UnityPackageInfo;

typedef gboolean (*AppFilterCallback)(UnityPackageInfo *, void *);

#ifdef __cplusplus
extern "C" {
#endif
   

UnityPackageSearcher*     unity_package_searcher_new    ();

UnityPackageSearcher*     unity_package_searcher_new_for_menu    (GMenuTree *menu);
UnityPackageSearcher*     unity_package_searcher_new_for_scopes  (UnityProtocolScopeRegistry *scope_registry);

void                      unity_package_searcher_free   (UnityPackageSearcher *searcher);

UnityPackageSearchResult* unity_package_searcher_search (UnityPackageSearcher  *searcher,
                                                         const gchar           *search_string,
                                                         guint                  max_hits,
                                                         UnityPackageSearchType search_type,
                                                         UnityPackageSort       sort);

UnityPackageSearchResult* unity_package_searcher_get_random_apps (UnityPackageSearcher *searcher,
                                                                  const gchar          *filter_query,
                                                                  guint                 n_apps);

UnityPackageSearchResult* unity_package_searcher_get_apps (UnityPackageSearcher *searcher,
                                                           const gchar          *filter_query,
                                                           guint                 n_apps,
                                                           AppFilterCallback    cb,
                                                           gpointer data);

UnityPackageSearchResult* unity_package_search_result_new ();
void                      unity_package_search_result_free (UnityPackageSearchResult *result);

UnityPackageInfo*         unity_package_package_info_new ();
void                      unity_package_package_info_free (gpointer pkg);

UnityPackageSearchResult* unity_package_searcher_get_by_exact_names (UnityPackageSearcher *searcher, GSList *names);

UnityPackageInfo*         unity_package_searcher_get_by_desktop_file (UnityPackageSearcher *searcher, const gchar *desktop_file);

#ifdef __cplusplus
}
#endif
