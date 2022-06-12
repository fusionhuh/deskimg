#include <gtk/gtk.h>
#include <argp.h>
#include <stdlib.h>


// global option vars START //
gdouble opacity = 1;
gboolean decorate = (gboolean) FALSE;
gchar* filename;
// global option vars END //

static gboolean
clicked (GtkWidget *window, GdkEventButton *event, gpointer user_data)
{
  if (event->button == 1 && event->type == GDK_2BUTTON_PRESS)
  {
    gint root_x;
    gint root_y;

    gtk_window_get_position (GTK_WINDOW (window), &root_x, &root_y);

    gtk_window_begin_move_drag (GTK_WINDOW (window), 1, root_x, root_y, 0);
  }
}


static void
activate (GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");

  g_signal_connect (window, "button-press-event", G_CALLBACK (clicked), NULL);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  GtkWidget *image;
  image = gtk_image_new_from_file (filename);

  GdkScreen *screen;
  GdkVisual *visual;

  gtk_widget_set_app_paintable(window, TRUE);
  screen = gdk_screen_get_default();
  visual = gdk_screen_get_rgba_visual(screen);

  if (visual!= NULL && gdk_screen_is_composited(screen))
  {
    gtk_widget_set_visual(window, visual);
  }

  gtk_container_add (GTK_CONTAINER (window), image);

  gtk_window_set_decorated (GTK_WINDOW (window), FALSE);

  gtk_widget_set_opacity(window, opacity);

  gtk_widget_show_all (window);
}

static void
command_line (GtkApplication* app, GApplicationCommandLine* cmdline)
{
  int p = 0;
}

static int
handle_local_options (GtkApplication *app, GVariantDict* options)
{
  g_variant_dict_lookup (options, "file", "s", &filename);
  g_variant_dict_lookup (options, "opacity", "d", &opacity);
  
  return -1;
}

static const GOptionEntry options[] =
{
  /// *** File
  {
    "file", 'f', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING,
    NULL, "Select the image/animation file to be displayed.", NULL
  },

  /// *** Opacity
  {
    "opacity", 'o', G_OPTION_FLAG_NONE, G_OPTION_ARG_DOUBLE,
    NULL, "Set the opacity of the graphic.", "Decimal value from 0 to 1."
  },

  { NULL }
};

int
main (int argc, char *argv[])
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtkmm.examples.base", G_APPLICATION_HANDLES_COMMAND_LINE);

  //add main options here
  g_application_add_main_option_entries (app, options);

  g_signal_connect (app, "startup", G_CALLBACK (activate), NULL);
  g_signal_connect_swapped (app, "command-line", G_CALLBACK (command_line), app);
  g_signal_connect_swapped (app, "handle-local-options", G_CALLBACK (handle_local_options), app);

  status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return status;
}
