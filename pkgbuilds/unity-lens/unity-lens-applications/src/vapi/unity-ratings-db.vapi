/* unity-ratings-db.vapi hand crafted by Mikkel Kamstrup Erlandsen */

[CCode (cprefix = "Unity", lower_case_cprefix = "unity_", cheader_filename = "unity-ratings-db.h")]
namespace Unity {

  [CCode (cprefix = "UnityRatings", lower_case_cprefix = "unity_ratings_")]
  namespace Ratings {
    [CCode (cheader_filename = "unity-ratings-db.h")]
    public struct Result {
      public int32 average_rating;
      public int32 total_rating;
      public int32 dampened_rating;
    }
  
    [Compact]
    [CCode (free_function = "unity_ratings_database_free", cheader_filename = "unity-ratings-db.h")]
    public class Database {
      [CCode (cname = "unity_ratings_database_new")]
      public Database () throws GLib.FileError;
      public bool query (string pkgname, out Result result);
    }
  }
}
