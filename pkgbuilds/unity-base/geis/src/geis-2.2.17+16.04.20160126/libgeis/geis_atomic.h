/**
 * @file libgeis/geis_atomic.c
 * @brief Atomic operation helpers
 *
 * Copyright 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef GEIS_ATOMIC_H_
#define GEIS_ATOMIC_H_

/**
 * An atomic value used for reference counting.
 */
typedef unsigned int GeisRefCount;

/**
 * Atomically increments a refcount.
 *
 * @param[in] refcount  A pointer to a refcount.
 */
static inline void
geis_atomic_ref(GeisRefCount *refcount)
{
  __sync_fetch_and_add(refcount, 1);
}

/**
 * Atomically decrements a refcount.
 *
 * @param[in] refcount  A pointer to a refcount.
 *
 * @returns the new reccount value.
 */
static inline GeisRefCount
geis_atomic_unref(GeisRefCount *refcount)
{
  return __sync_sub_and_fetch(refcount, 1);
}

#endif /* GEIS_ATOMIC_H_ */
