#include <gtk/gtk.h>

static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  //gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  GtkWidget *image;
  image = gtk_image_new_from_file ("/home/josh/Pictures/missing.gif");

  GdkScreen *screen;
  GdkVisual *visual;

  gtk_widget_set_app_paintable(window, TRUE);
  screen = gdk_screen_get_default();
  visual = gdk_screen_get_rgba_visual(screen);

  if (visual!= NULL && gdk_screen_is_composited(screen))
  {
    gtk_widget_set_visual(window, visual);
  }

  //button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  //gtk_container_add (GTK_CONTAINER (window), button_box);

  gtk_container_add (GTK_CONTAINER (window), image);

  gtk_window_set_decorated (GTK_WINDOW(window), FALSE);

  gtk_widget_set_opacity(window, 0.8);

  //button = gtk_button_new_with_label ("Hello World");
  //g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  //gtk_container_add (GTK_CONTAINER (button_box), button);

  gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  
  g_object_unref (app);

  return status;
}
