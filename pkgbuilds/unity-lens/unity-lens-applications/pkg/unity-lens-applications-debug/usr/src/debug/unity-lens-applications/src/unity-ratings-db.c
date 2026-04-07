/*
 * Copyright (C) 2011 Canonical Ltd
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

#include "unity-ratings-db.h"
#include <string.h>
#include <db.h>

/* Hacky check, but we *really* need to have the same libdb version as the
 * software-center is using to avoid a cascade of fail */

struct _UnityRatingsDatabase {
  DB_ENV  *env;
  DB      *db;
};

/* IMPLEMENTATION NOTE: It is paramount that we create our BDB environment
 *                      in "concurrent" mode. This makes the database
 *                      single-writer-multiple-reader safe. Hence allowing
 *                      us to read the DB while S-C is updating it.
 */


#define UNITY_RATINGS_DB_PATH "software-center/reviews.ubuntu.com_reviews_api_1.0_review-stats-pkgnames.p__5.3.db"
#define UNITY_RATINGS_DB_ENV_PATH UNITY_RATINGS_DB_PATH".dbenv"

UnityRatingsDatabase*
unity_ratings_database_new (GError                **error)
{
  UnityRatingsDatabase *self;
  DB_ENV *env;
  DB     *db;
  int     error_code;
  gchar  *path;
  
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);
  
  /* Create a database environment in concurrent mode,
   * enabling single-writer-multiple-reader support */
  path = g_build_filename (g_get_user_cache_dir (),
                           UNITY_RATINGS_DB_ENV_PATH,
                           NULL);
  db_env_create (&env, 0);
  error_code = env->open (env,
                          path,                        /* db env home dir */
                          DB_INIT_CDB | DB_INIT_MPOOL, /* use concurrent mode */
                          0600 /* access mode (user-rw only) */);
  
  if (error_code)
    {
      g_set_error (error, G_FILE_ERROR, error_code,
                   "Unable to open ratings database environment '%s': %s",
                   path, db_strerror (error_code)); // FIXME: Leak db_strerror() return?
      env->close (env, 0);
      g_free (path);
      return NULL;
    }
  
  g_free (path);
  
  /* Create a db handle in our environment */
  path = g_build_filename (g_get_user_cache_dir (),
                           UNITY_RATINGS_DB_PATH,
                           NULL);
  db_create (&db, env, 0);
  error_code = db->open (db,
                         NULL,       /* transaction context */
                         path,       /* file name */
                         NULL,       /* db name */
                         DB_HASH,    /* db type */
                         DB_RDONLY,  /* flags - read-only */
                         0600        /* access mode (user-rw only) */);
  
  if (error_code)
    {
      g_set_error (error, G_FILE_ERROR, error_code,
                   "Unable to open ratings database '%s': %s",
                   path, db_strerror (error_code)); // FIXME: Leak db_strerror() return?
      g_free (path);
      db->close (db, 0);
      env->close (env, 0);
      return NULL;
    }
  
  g_free (path);
  
  self = g_new0 (UnityRatingsDatabase, 1);
  self->env = env;
  self->db = db;
  
  return self;
}

void
unity_ratings_database_free (UnityRatingsDatabase   *self)
{
  g_return_if_fail (self != NULL);
  
  self->db->close (self->db, 0);
  self->env->close (self->env, 0);
  
  g_free (self);
}

gboolean
unity_ratings_database_query (UnityRatingsDatabase   *self,
                              const gchar            *pkgname,
                              UnityRatingsResult     *out_result)
{
  DBT     key = { 0 }, data = { 0 };
  gint    error_code;
  
  
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (pkgname != NULL, FALSE);
  g_return_val_if_fail (out_result != NULL, FALSE);
  
  /* Make sure the DBTs are zeroed. Otherwise libdb throws a fit */
  key.data = (gchar*)pkgname;
  key.size = strlen (pkgname);
  key.ulen = key.size;
  key.dlen = 0;
  key.doff = 0;
  key.flags = DB_DBT_USERMEM;
  data.data = out_result;
  data.size = 0;
  data.ulen = sizeof (UnityRatingsResult);
  data.dlen = 0;
  data.doff = 0;
  data.flags= DB_DBT_USERMEM;
  
  error_code = self->db->get (self->db,
                              NULL,
                              &key,
                              &data,
                              0);
  
  /* error_code == 0 means success, everything else is bad */
  if (error_code == DB_NOTFOUND)
    {
      goto not_found;
    }
  else if (error_code)
    {
      g_warning ("Error looking up ratings for '%s': %s",
                 pkgname, db_strerror (error_code)); // FIXME : leak strerror?
      goto not_found;
    }
  
  if (data.size != 3*sizeof (gint32))
    {
      g_critical ("Unexpected datum size from ratings database %i bytes. "
                  "Expected %lu bytes", data.size, 3*sizeof (gint32));
      goto not_found;
    }
  
  return TRUE;
  
  not_found:
    out_result->average_rating = 0;
    out_result->total_rating = 0;
    out_result->dampened_rating = 0;
    return FALSE;
}

