#include <gtk/gtk.h>
#include <stdlib.h>


// global option vars START //
gdouble opacity = 1;
gboolean decorate = (gboolean) FALSE;
gchar* filename;
gint x_pos = 0;
gint y_pos = 0;
// global option vars END //

static gboolean
clicked (GtkWidget *window, GdkEventButton *event, gpointer user_data)
{
  if (event->button == 1) // GDK_2BUTTON_PRESS corresponds to double click. See https://docs.gtk.org/gdk3/struct.EventButton.html for more info.
  {
    gint root_x;
    gint root_y;

    gint x_dim; // x- and y-dimensions of the window respectively.
    gint y_dim;

    gtk_window_get_position (GTK_WINDOW (window), &root_x, &root_y);

    gtk_window_get_size (GTK_WINDOW (window), &x_dim, &y_dim);

    gtk_window_begin_move_drag (GTK_WINDOW (window), 1, root_x + x_dim/2, root_y + y_dim/2, 0);
  }
  if (event->button == 2 && event->type == GDK_2BUTTON_PRESS) // Emergency exit in case save command in bash.rc produces no terminal window to give exit command
  {
    exit(1);
  }
  if (event->button == 3 && event->type == GDK_2BUTTON_PRESS)
  {
    gint root_x = 0;
    gint root_y = 0;

    gtk_window_get_position (GTK_WINDOW (window), &root_x, &root_y);

    printf("%i    %i\n", root_x, root_y);
  }
}

static void
activate (GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");

  g_signal_connect (window, "button-press-event", G_CALLBACK (clicked), NULL);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);       // Adds click event to window widget.

  GtkWidget *image;
  image = gtk_image_new_from_file (filename);                 // Loads the image located at the path specified by the -f option.

  GdkScreen *screen;
  GdkVisual *visual;

  gtk_widget_set_app_paintable(window, TRUE);                 // Makes the window background
  screen = gdk_screen_get_default();                          // transparent, allowing view
  visual = gdk_screen_get_rgba_visual(screen);                // to the desktop.
  if (visual!= NULL && gdk_screen_is_composited(screen))      //
  {                                                           //
    gtk_widget_set_visual(window, visual);                    //
  }                                                           //

  gtk_container_add (GTK_CONTAINER (window), image);

  gtk_window_set_decorated (GTK_WINDOW (window), FALSE);      // Removes the border around the window.

  gtk_widget_set_opacity(window, opacity);                    // Sets the window to the opacity specified by options.

  gtk_window_set_keep_below (GTK_WINDOW (window), TRUE);      // Keeps the window below all other windows on screen.

  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), TRUE); // Removes the taskbar icon that would normally appear.

  gtk_widget_show_all (window);

  gint root_x = 0;                                                  // Temporary variables used to store the position of the window
  gint root_y = 0;                                                  // relative to the root window.

  gtk_window_move (GTK_WINDOW (window),
   x_pos - root_x, y_pos - root_y);          // Sets the coordinates of the top left corner of the window.
}

static int
handle_local_options (GtkApplication *app, GVariantDict* options)
{
  g_variant_dict_lookup (options, "file", "s", &filename);
  g_variant_dict_lookup (options, "opacity", "d", &opacity);
  g_variant_dict_lookup (options, "x_pos", "i", &x_pos);
  g_variant_dict_lookup (options, "y_pos", "i", &y_pos);
  
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
    NULL, "Set the opacity of the graphic.", 
    "Decimal value from 0 to 1."
  },

  /// *** X-Position
  {
    "x_pos", 'x', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT,
    NULL, "Set the x-position of the graphic.", 
    "Integer values only."
  },

  /// *** Y-Position
  {
    "y_pos", 'y', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT,
    NULL, "Set the y-position of the graphic.", "Integer values only."
  },

  { NULL }
};

int
main (int argc, char *argv[])
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtkmm.examples.base", G_APPLICATION_FLAGS_NONE);

  g_application_set_flags(app, G_APPLICATION_NON_UNIQUE);

  g_application_add_main_option_entries (app, options);           // Adds the options in the GOptionEntry array to the commmand line parser(?)

  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect_swapped (app, "handle-local-options", G_CALLBACK (handle_local_options), app);

  status = g_application_run (G_APPLICATION (app), argc, argv);

  printf("test\n");

  g_object_unref (app);

  return status;
}
