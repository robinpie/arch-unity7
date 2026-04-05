/*
 * Copyright (C) 2009 Canonical, Ltd.
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
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 */

#if !defined (_ZEITGEIST_H_INSIDE_) && !defined (ZEITGEIST_COMPILATION)
#error "Only <zeitgeist.h> can be included directly."
#endif

#ifndef _ZEITGEIST_SIMPLE_RESULT_SET_H_
#define _ZEITGEIST_SIMPLE_RESULT_SET_H_

#include <glib.h>
#include <glib-object.h>
#include <zeitgeist-event.h>
#include <zeitgeist-result-set.h>

G_BEGIN_DECLS

#define ZEITGEIST_TYPE_SIMPLE_RESULT_SET (zeitgeist_simple_result_set_get_type ())

#define ZEITGEIST_SIMPLE_RESULT_SET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
        ZEITGEIST_TYPE_SIMPLE_RESULT_SET, ZeitgeistSimpleResultSet))
        
#define ZEITGEIST_SIMPLE_RESULT_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
        ZEITGEIST_TYPE_SIMPLE_RESULT_SET, ZeitgeistSimpleResultSetClass))
        
#define ZEITGEIST_IS_SIMPLE_RESULT_SET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
        ZEITGEIST_TYPE_SIMPLE_RESULT_SET))
        
#define ZEITGEIST_IS_SIMPLE_RESULT_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
        ZEITGEIST_TYPE_SIMPLE_RESULT_SET))
        
#define ZEITGEIST_SIMPLE_RESULT_SET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
        ZEITGEIST_TYPE_SIMPLE_RESULT_SET, ZeitgeistSimpleResultSetClass))

typedef struct _ZeitgeistSimpleResultSet ZeitgeistSimpleResultSet;
typedef struct _ZeitgeistSimpleResultSetClass ZeitgeistSimpleResultSetClass;

struct _ZeitgeistSimpleResultSet
{
  GObject  parent_instance;
};

struct _ZeitgeistSimpleResultSetClass
{
  GObjectClass  parent_class;
};

ZeitgeistResultSet* _zeitgeist_simple_result_set_new (GPtrArray *events,
                                                      guint      estimated_matches);

G_END_DECLS

#endif /* _HAVE_ZEITGEIST_RESULT_SET_H */
