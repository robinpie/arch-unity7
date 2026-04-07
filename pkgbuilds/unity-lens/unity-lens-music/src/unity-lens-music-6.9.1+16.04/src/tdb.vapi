/* tdb.vapi
 *
 * Copyright (C) 2012 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 * Author:
 * 	Michal Hruby <michal.hruby@canonical.com>
 */

[CCode (lower_case_cprefix = "tdb_", cheader_filename = "tdb.h")]
namespace TDB {
	/* Database Connection Handle */
	[Compact]
	[CCode (free_function = "tdb_close", cname = "TDB_CONTEXT", cprefix = "tdb_")]
	public class Database {
		[CCode (cname = "tdb_open")]
		public Database (string name, int hash_size, TDB.OpenFlags tdb_flags, int open_flags, Posix.mode_t mode);
		[CCode (cname = "tdb_open_ex")]
		public Database.open_ex (string name, int hash_size, int tdb_flags, int open_flags, Posix.mode_t mode, TDB.LogFunc log_fn);

		public int reopen ();
		public static int reopen_all ();

		public TDB.Error error ();
		public unowned string errorstr ();

		public TDB.Data fetch (TDB.Data key);
		public int @delete (TDB.Data key);
		public int store (TDB.Data key, TDB.Data dbuf, TDB.StoreType type_flag);
		public TDB.Data firstkey ();
		public TDB.Data nextkey (TDB.Data key);
		public int traverse (TDB.TraverseFunc traverse_func);
		public int exists (TDB.Data key);
		public int lockkeys (TDB.Data[] keys);
		public void unlockkeys ();
		public int lockall ();
		public void unlockall ();

		public int chainlock (TDB.Data key);
		public void chainunlock (TDB.Data key);
	}

	[CCode (cname = "TDB_DATA")]
	[SimpleType]
	public struct Data {
		[CCode (array_length_cname = "dsize", array_length_type = "size_t", cname = "dptr")]
		public unowned uint8[] data;
		[CCode (cname = "dsize")]
		public size_t data_size;
	}

	[CCode (cname = "tdb_null")]
	public const TDB.Data NULL_DATA;

	[CCode (has_target = false)]
	public delegate void LogFunc (TDB.Database db, TDB.DebugLevel debug_level, string format, ...);

	public delegate int TraverseFunc (TDB.Database db, TDB.Data key, TDB.Data @value);

	[CCode (cname = "SQLITE_ANY")]
	public const int ANY;

	[CCode (cname = "enum tdb_debug_level", cprefix = "TDB_DEBUG_")]
	public enum DebugLevel {
		FATAL,
		ERROR,
		WARNING,
		TRACE
	}

	[CCode (cname = "int", cprefix = "TDB_")]
	public enum StoreType {
		REPLACE,
		INSERT,
		MODIFY
	}

	[CCode (cname = "int", cprefix = "TDB_")]
	public enum OpenFlags {
		DEFAULT,
		CLEAR_IF_FIRST,
		INTERNAL,
		NOLOCK,
		NOMMAP,
		CONVERT,
		BIGENDIAN,
		NOSYNC,
		SEQNUM,
		VOLATILE,
		ALLOW_NESTING,
		DISALLOW_NESTING,
		INCOMPATIBLE_HASH
	}

	[CCode (cname = "enum TDB_ERROR", cprefix = "TDB_ERR_")]
	public enum Error {
		[CCode (cname = "TDB_SUCCESS")]
		SUCCESS,
		CORRUPT,
		IO,
		LOCK,
		OOM,
		EXISTS,
		NOLOCK,
		LOCK_TIMEOUT,
		NOEXISTS,
		EINVAL,
		RDONLY,
		NESTING
	}
}

