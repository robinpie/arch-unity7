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


#ifndef _UNITY_RATINGS_DB_H_
#define _UNITY_RATINGS_DB_H_


#include <glib.h>

G_BEGIN_DECLS

/* This struct *must* start with 3 32 bit integers, to smoothly be able
 * to store the results we read from the Software Center Ratings DB */
typedef struct {
  gint32        average_rating;
  gint32        total_rating;
  gint32        dampened_rating;
} UnityRatingsResult;

typedef struct _UnityRatingsDatabase UnityRatingsDatabase;

UnityRatingsDatabase* unity_ratings_database_new   (GError                **error);

void                  unity_ratings_database_free  (UnityRatingsDatabase   *self);

gboolean              unity_ratings_database_query (UnityRatingsDatabase   *self,
                                                    const gchar            *pkgname,
                                                    UnityRatingsResult     *out_result);


G_END_DECLS

#endif /* _UNITY_RATINGS_DB_H_ */
