/**
 * @file geis_group.c
 * @brief Geis gesture group module implementation
 *
 * Copyright 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_config.h"
#include "geis_group.h"

#include "geis_logging.h"
#include <stdlib.h>


struct _GeisGroup
{
  GeisGroup    next;
  GeisInteger  id;
  GeisFrameSet frameset;
};

struct _GeisGroupSet
{
  GeisSize  count;
  GeisGroup first;
};


/*
 * Creates a new, empty group set.
 */
GeisGroupSet
geis_groupset_new()
{
  GeisGroupSet groupset = calloc(1, sizeof(struct _GeisGroupSet));
  if (!groupset)
  {
    geis_error("error allocating gesture group set");
    goto final_exit;
  }

final_exit:
  return groupset;
}


/*
 * Destroys a group set and all groups contained in it.
 */
void
geis_groupset_delete(GeisGroupSet groupset)
{
  GeisGroup p = groupset->first;
  while (p)
  {
    GeisGroup tmp = p->next;
    geis_group_delete(p);
    p = tmp;
  }
  free(groupset);
}


/*
 * Inserts a gesture group into a group set.
 */
GeisStatus
geis_groupset_insert(GeisGroupSet groupset, GeisGroup group)
{
  if (groupset->count == 0)
  {
    groupset->first = group;
  }
  else
  {
    GeisGroup p = groupset->first;
    while (p->next)
      p = p->next;
    p->next = group;
  }
  ++groupset->count;
  return GEIS_STATUS_SUCCESS;
}


GeisSize
geis_groupset_group_count(GeisGroupSet groupset)
{
  return groupset->count;
}


GeisGroup
geis_groupset_group(GeisGroupSet groupset, GeisSize index)
{
  GeisGroup group = NULL;
  if (index >= groupset->count)
  {
    geis_warning("gesture group set index out of range");
  }
  else
  {
    GeisSize i;
    group = groupset->first;
    for (i = 0; i < index; ++i)
    {
      group = group->next;
    }
  }
  return group;
}


/*
 * Creates a new gesture group.
 */
GeisGroup
geis_group_new(GeisInteger id)
{
  GeisGroup group = calloc(1, sizeof(struct _GeisGroup));
  if (!group)
  {
    geis_error("error allocating gesture group");
    goto final_exit;
  }

  group->frameset = geis_frameset_new();
  if (!group->frameset)
  {
    geis_error("error allocating gesture group frame set");
    goto unwind_group;
  }

  group->id = id;
  goto final_exit;

unwind_group:
  free(group);
  group = NULL;
final_exit:
  return group;
}


/*
 * Destroys a gesture group and all gesture frames contained in it.
 */
void
geis_group_delete(GeisGroup group)
{
  geis_frameset_delete(group->frameset);
  free(group);
}


/*
 * Gets the identifier of a gesture group.
 */
GeisInteger
geis_group_id(GeisGroup group)
{
  return group->id;
}


/*
 * Adds a gesture frame to a gesture group.
 */
GeisStatus
geis_group_insert_frame(GeisGroup group, GeisFrame frame)
{
  return geis_frameset_insert(group->frameset, frame);
}


/*
 * Gets the number of gesture frames in a gesture group.
 */
GeisSize
geis_group_frame_count(GeisGroup group)
{
  return geis_frameset_frame_count(group->frameset);
}


/*
 * Gets an indicated gesture frame from a gesture group.
 */
GeisFrame
geis_group_frame(GeisGroup group, GeisSize index)
{
  return geis_frameset_frame(group->frameset, index);
}


/*
 * Marks a gesture group as rejected.
 */
void
geis_group_reject(GeisGroup group GEIS_UNUSED)
{
}


