// gpl v2
// (C) Neil Jagdish Patel <neil.patel@canonical.com>
#include <gtk/gtk.h>
#include <unity-misc/na-tray-manager.h>
#include <unity-misc/na-tray-child.h>
#include <unity-misc/na-tray.h>

static gboolean
on_window_expose (GtkWidget *window, cairo_t *cr)
{
  GtkAllocation alloc;

  gtk_widget_get_allocation (window, &alloc);

  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);

  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  cairo_set_source_rgba (cr, 0.0f, 0.0f, 0.0f, 0.0f);
  cairo_rectangle (cr, 0, 0, alloc.width, alloc.height);
  cairo_fill (cr);

  gtk_container_propagate_draw (GTK_CONTAINER (window),
                                  gtk_bin_get_child (GTK_BIN (window)),
                                  cr);

  return FALSE;
}

int
main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *tray;

  gtk_init (&argc, &argv);

  if (na_tray_manager_check_running (gdk_screen_get_default ()))
    g_error ("Another tray manager is running");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_resize (GTK_WINDOW (window), 100, 24);
  gtk_widget_set_name (window, "UnityPanelApplet");
  gtk_widget_set_visual (window, gdk_screen_get_rgba_visual (gdk_screen_get_default ()));
  gtk_widget_realize (window);
  gtk_widget_set_app_paintable (window, TRUE);

  g_signal_connect (window, "draw",
                    G_CALLBACK (on_window_expose), NULL);

  tray = (GtkWidget *)na_tray_new_for_screen (gdk_screen_get_default (),
                                              GTK_ORIENTATION_HORIZONTAL,
                                              NULL, NULL);
  gtk_container_add (GTK_CONTAINER (window), tray);
  gtk_widget_show (tray);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
