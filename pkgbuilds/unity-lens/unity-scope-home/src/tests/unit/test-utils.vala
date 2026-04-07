/*
 * Copyright (C) 2011-2013 Canonical Ltd
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
  
using Unity.Protocol;

public static bool run_with_timeout (MainLoop ml, uint timeout_ms = 5000)
{
  bool timeout_reached = false;
  var t_id = Timeout.add (timeout_ms, () =>
  {
    timeout_reached = true;
    debug ("Timeout reached");
    ml.quit ();
    return false;
  });
  
  ml.run ();
  
  if (!timeout_reached) Source.remove (t_id);
  
  return !timeout_reached;
}

/* A bit of magic to get proper-ish fixture support */
public interface Fixture : Object
{
  class DelegateWrapper
  {
    TestDataFunc func;
    public DelegateWrapper (owned TestDataFunc f) { func = (owned) f; }
  }
  
  public virtual void setup () {}
  public virtual void teardown () {}
  
  [CCode (has_target = false)]
  public delegate void Callback<T> (T ptr);
  
  private static List<DelegateWrapper> _tests;
  
  public static unowned TestDataFunc create<F> (Callback<void*> cb)
  requires (typeof (F).is_a (typeof (Fixture)))
  {
    TestDataFunc functor = () =>
    {
      var type = typeof (F);
      var instance = Object.new (type) as Fixture;
      instance.setup ();
      cb (instance);
      instance.teardown ();
    };
    unowned TestDataFunc copy = functor;
    _tests.append (new DelegateWrapper ((owned) functor));
    return copy;
  }
  public static unowned TestDataFunc create_static<F> (Callback<F> cb)
  {
    return create<F> ((Callback<void*>) cb);
  }
}

// this will auto-disconnect signals when it goes out of scope
public class SignalWrapper
{
  unowned Object obj;
  ulong sig_id;
  
  public SignalWrapper (Object o, ulong signal_id)
  {
    obj = o;
    sig_id = signal_id;
  }

  ~SignalWrapper ()
  {
    SignalHandler.disconnect (obj, sig_id);
  }
}

public static ScopeProxy? acquire_test_proxy (string name, string path)
{
  var ml = new MainLoop ();
  ScopeProxy? proxy = null;
  ScopeProxy.new_from_dbus.begin (name, path, null, (obj, res) =>
  {
    try
    {
      proxy = ScopeProxy.new_from_dbus.end (res);
    }
    catch (Error e) {}
    ml.quit ();
  });
  assert (run_with_timeout (ml));
  return proxy;
}

public static void wait_for_synchronization (Dee.Model model)
{
  var shared_model = model as Dee.SharedModel;
  if (shared_model == null) return;
  
  if (shared_model.is_synchronized ()) return;
  SignalWrapper[] signals = {};
  var ml = new MainLoop ();
  
  signals += new SignalWrapper (shared_model, 
                                shared_model.notify["synchronized"].connect (() =>
                                {
                                  ml.quit ();
                                }));
  
  run_with_timeout (ml);
}

public static string open_channel (ScopeProxy proxy,
                                   ChannelType channel_type,
                                   out Dee.SerializableModel model,
                                   bool wait_for_sync = false,
                                   ChannelFlags flags = 0)
{
  string? channel_id = null;
  Dee.Model? real_model = null;
  var ml = new MainLoop ();
  /* Need to use PRIVATE channel, cause standard SharedModel won't
   * synchronize properly when trying to connect to the model
   * from the same process (/bus address) */
  proxy.open_channel.begin (channel_type,
                            flags | ChannelFlags.PRIVATE,
                            null,
                            (obj, res) =>
                            {
                              try
                              {
                                channel_id = proxy.open_channel.end (res, out real_model);
                                if (wait_for_sync)
                                {
                                  wait_for_synchronization (real_model);
                                }
                                ml.quit ();
                              }
                              catch (Error err)
                              {
                                ml.quit ();
                              }
                            });
  
  assert (run_with_timeout (ml));
  assert (channel_id != null);
  model = real_model as Dee.SerializableModel;
  return channel_id;
}

public static HashTable<string, Variant> perform_search (
  ScopeProxy proxy, string channel_id, string query,
  HashTable<string, Variant>? hints = null,
  Dee.SerializableModel? model = null)
{
  var ml = new MainLoop ();
  HashTable<string, Variant>? reply_dict = null;
  proxy.search.begin (channel_id, query,
                      hints ?? new HashTable<string, Variant> (null, null),
                      null,
                      (obj, res) =>
                      {
                        try
                        {
                          reply_dict = proxy.search.end (res);
                        }
                        catch (Error err) {}
                        ml.quit ();
                      });
  
  bool got_search_signal = false;
  
  assert (run_with_timeout (ml, 10000));
  assert (reply_dict != null);
  return reply_dict;
}

public static Variant[] scope_result_to_variant (Unity.ScopeResult result)
{
  var v = new Variant[9];
  v[0] = result.uri;
  v[1] = result.icon_hint;
  v[2] = result.category;
  v[3] = (uint) result.result_type;
  v[4] = result.mimetype;
  v[5] = result.title;
  v[6] = result.comment;
  v[7] = result.dnd_uri;
  v[8] = result.metadata;
  
  return v;
}

public static ActivationReplyRaw? activate (
        ScopeProxy proxy, string channel_id,
        Unity.Protocol.ActionType action_type,
        Unity.ScopeResult result,
        HashTable<string, Variant> hints)
{
  var ml = new MainLoop ();
  var result_arr = scope_result_to_variant (result);
  Unity.Protocol.ActivationReplyRaw? activation_reply = null;
  proxy.activate.begin (channel_id, result_arr,
                        action_type,
                        hints,
                        null,
                        (obj, res) =>
                        {
                          try
                          {
                            activation_reply = proxy.activate.end (res);
                          }
                          catch (Error err) { warning ("%s", err.message); }
                          ml.quit ();
                        });
  
  assert (run_with_timeout (ml));
  return activation_reply;
}
