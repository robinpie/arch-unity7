[CCode (cprefix = "Soup", gir_namespace = "SoupGNOME", gir_version = "2.4", lower_case_cprefix = "soup_")]
namespace SoupGNOME {
	[CCode (cheader_filename = "libsoup/soup-gnome.h", type_id = "soup_proxy_resolver_gnome_get_type ()")]
	public class ProxyResolverGNOME : Soup.ProxyResolverDefault, Soup.ProxyURIResolver, Soup.SessionFeature {
		[CCode (has_construct_function = false)]
		protected ProxyResolverGNOME ();
	}
}
