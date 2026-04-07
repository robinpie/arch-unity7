[CCode (cheader_filename = "uuid.h", lower_case_cprefix = "")]
namespace UUID
{
  private static void uuid_generate_time ([CCode (array_length=false)] uint8[] data);
  private static void uuid_generate_random ([CCode (array_length=false)] uint8[] data);
  private static void uuid_unparse ([CCode (array_length=false)] uint8[] uuid, [CCode (array_length=false)] char[] str);

  /**
   Generate Time-UUID with node (MAC) bytes replaced with random bytes, so that it doesn't contain any sensitive data.
   */
  public static string randomized_time_uuid ()
  {
    char[] res = new char[37];

    uint8[] data1 = new uint8[16];
    uuid_generate_time (data1);

    uint8[] data2 = new uint8[16];
    uuid_generate_random (data2);

    // last 6 bytes are MAC, overwrite with random bytes and set multicast bit, see section 4.5 of RFC 4122.
    for (int i = 10; i<16; i++)
      data1[i] = data2[i];
    data1[10] |= 1;

    uuid_unparse (data1, res);
    string uuid_str = (string)res;
    return uuid_str;
  }
}

