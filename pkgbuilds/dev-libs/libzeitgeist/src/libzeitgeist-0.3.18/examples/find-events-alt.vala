using Zeitgeist;

namespace ZeitgeistExample
{

	public async void find_events (Zeitgeist.Log zg)
	{
     	Event ev = new Event.full (Zeitgeist.ZG_ACCESS_EVENT,
    	                           Zeitgeist.ZG_USER_ACTIVITY,
    	                           "");
    	
    	PtrArray events = new PtrArray();
    	events.add ((ev as Object).ref());
    	
    	var results = yield zg.find_events (new TimeRange.anytime(),
    	                                    (owned)events,
    	                                    StorageState.ANY,
    	                                    10,
    	                                    ResultType.MOST_POPULAR_SUBJECTS,
    	                                    null);
    	foreach (var e in results)
    		{
    			print (" * %s\n", e.get_subject(0).get_uri());
    		}
	}

	public static int main (string[] args)
	{
		var zg = new Zeitgeist.Log();
		find_events.begin (zg);
		
		MainLoop mainloop = new MainLoop(null, false);
    	mainloop.run();
    	
    	return 0;
	}

}
