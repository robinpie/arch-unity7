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
 * Modified by Angelo Compagnucci <angelo.compagnucci@gmail.com>
 *
 */

namespace Unity.FilesLens {
  public enum UrlType {
   UNKNOWN,
   WEB,
   MOUNTABLE;
  }

  public class UrlChecker : Object
  {
    /* A string serialized GIcon */
    public string web_icon { get; private set; }
    public string mountable_icon { get; private set; }
    
    /* Regexes URLs must match */
    private Regex web_regex;
    private Regex mountable_regex;
    private const string TLD_REGEX = "(ac|ad|ae|aero|af|ag|ai|al|am|an|ao|aq|ar|arpa|"    +
      "as|asia|at|au|aw|ax|az|ba|bb|bd|be|bf|bg|bh|bi|biz|bj|bm|bn|bo|br|bs|bt|bv|bw|"    +
      "by|bz|ca|cat|cc|cd|cf|cg|ch|ci|ck|cl|cm|cn|co|com|coop|cr|cu|cv|cw|cx|cy|cz|de|"   +
      "dj|dk|dm|do|dz|ec|edu|ee|eg|er|es|et|eu|fi|fj|fk|fm|fo|fr|ga|gb|gd|ge|gf|gg|gh|"   +
      "gi|gl|gm|gn|gov|gp|gq|gr|gs|gt|gu|gw|gy|hk|hm|hn|hr|ht|hu|id|ie|il|im|in|info|"    +
      "int|io|iq|ir|is|it|je|jm|jo|jobs|jp|ke|kg|kh|ki|km|kn|kp|kr|kw|ky|kz|la|lb|lc|"    +
      "li|lk|lr|ls|lt|lu|lv|ly|ma|mc|md|me|mg|mh|mil|mk|ml|mm|mn|mo|mobi|mp|mq|mr|ms|mt|" +
      "mu|museum|mv|mw|mx|my|mz|na|name|nc|ne|net|nf|ng|ni|nl|no|np|nr|nu|nz|om|org|pa|"  +
      "pe|pf|pg|ph|pk|pl|pm|pn|pr|pro|ps|pt|pw|py|qa|re|ro|rs|ru|rw|sa|sb|sc|sd|se|sg|"   +
      "sh|si|sj|sk|sl|sm|sn|so|sr|st|su|sv|sx|sy|sz|tc|td|tel|tf|tg|th|tj|tk|tl|tm|tn|"   +
      "to|tp|tr|travel|tt|tv|tw|tz|ua|ug|uk|us|uy|uz|va|vc|ve|vg|vi|vn|vu|wf|ws|xn|xxx|"  +
      "ye|yt|za|zm|zw|local|[0-9]{1,3})";
    
    public UrlChecker ()
    {
      web_icon = new ThemedIcon ("web-browser").to_string ();
      mountable_icon = new ThemedIcon ("folder-remote").to_string ();
      
      try {
        web_regex = new Regex ("(http[s]?://.+)|" +
                                "([a-zA-Z0-9\\-\\.]+\\." +TLD_REGEX+ "\\b.*)",
                                RegexCompileFlags.OPTIMIZE);
        mountable_regex = new Regex ("(\\\\|(ftp|ssh|sftp|smb|dav)://).+");
      } catch (RegexError e) {
        warning ("Error compiling regular expressions for URL matching. URL launching will not work: %s", e.message);
      }
    }
    
    /* Returns a valid URL if the input looks like it or null otherwise,
     * returns also url type which can after be used
     * to retrive corresponding icon
     */
    public string? check_url (string sample, out UrlType url_type)
    {
      url_type = UrlType.UNKNOWN;
      if (sample.strip () == "") return null;
      
      if (mountable_regex.match (sample))
      {
        url_type = UrlType.MOUNTABLE;
        return sample.replace("\\\\","smb://");
      }
      else if (web_regex.match (sample))
      {
        url_type = UrlType.WEB;
        return sample.has_prefix ("http") ? sample : ("http://" + sample);
      }
      return null;
    }
    
    public string get_icon_for_type (UrlType url_type)
    {
      switch (url_type)
      {
        case UrlType.WEB:
          return web_icon;
        case UrlType.MOUNTABLE:
          return mountable_icon;
      }
      return web_icon;
  }
    
  } /* end: class UrlChecker */

} /* end: namespace */
