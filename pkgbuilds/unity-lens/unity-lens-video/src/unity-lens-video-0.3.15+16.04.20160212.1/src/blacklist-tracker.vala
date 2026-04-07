/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Authored by Michal Hruby <michal.hruby@canonical.com>
 *
 */
using Zeitgeist;
using Config;
using Gee;

namespace Unity.VideoLens
{
  /* Libzeitgeist currently doesn't expose any blacklist API, so we need to
   * use direct dbus calls */
  [DBus (name = "org.gnome.zeitgeist.Blacklist")]
  public interface Blacklist : Object
  {
    [DBus (signature = "a{s(asaasay)}")]
    public abstract async Variant get_templates () throws Error;
    public signal void template_added (string template_id,
        [DBus (signature = "(asassay)")] Variant template);
    public signal void template_removed (string template_id,
        [DBus (signature = "(asassay)")] Variant template);
  }

  public class BlacklistTracker : Object
  {
    private HashTable<string, Event> event_templates;
    private Blacklist blacklist_proxy;

    private Set<string> blacklisted_uris;
    private bool cached_uris_dirty;

    construct
    {
      event_templates = new HashTable<string, Event> (str_hash, str_equal);
      blacklisted_uris = new HashSet<string> ();

      Bus.get_proxy<Blacklist> (BusType.SESSION, "org.gnome.zeitgeist.Engine",
                     "/org/gnome/zeitgeist/blacklist", 0, null, (src, res) =>
      {
        try
        {
          blacklist_proxy = (src as DBusConnection).get_proxy<Blacklist>.end (res);
          fetch_blacklists ();
        }
        catch (GLib.Error err)
        {
          warning ("%s", err.message);
        }
      });
    }

    private async void fetch_blacklists ()
    {
      blacklist_proxy.template_added.connect (this.template_added);
      blacklist_proxy.template_removed.connect (this.template_removed);
      Variant all_templates = yield blacklist_proxy.get_templates ();

      VariantIter iter = new VariantIter (all_templates);
      string template_id;
      Variant event_variant;
      while (iter.next ("{s@(asaasay)}", out template_id, out event_variant))
      {
        Event e = new Event.from_variant (event_variant);
        event_templates[template_id] = e;
      }
      cached_uris_dirty = true;
    }

    private void template_added (string id, Variant template)
    {
      Event e = new Event.from_variant (template);
      event_templates[id] = e;
      cached_uris_dirty = true;
    }

    private void template_removed (string id, Variant template)
    {
      event_templates.remove (id);
      cached_uris_dirty = true;
    }

    private void update_uris ()
    {
      var iter = HashTableIter<string, Event> (event_templates);
      unowned Event e;
      while (iter.next (null, out e))
      {
        if (e.num_subjects () > 0)
        {
          unowned Subject s = e.get_subject (0);
          unowned string uri = s.get_uri ();
          if (uri == null || uri == "") continue;

          if (uri.has_suffix ("*"))
          {
            blacklisted_uris.add (uri.substring (0, uri.length - 1));
          }
        }
      }
    }

    public unowned Set<string> get_blacklisted_uris ()
    {
      if (cached_uris_dirty)
      {
        blacklisted_uris.clear ();
        update_uris ();
        cached_uris_dirty = false;
      }
      return blacklisted_uris;
    }
  }

} /* namespace */
