#include <gtk/gtk.h>
#include <argp.h>
#include <stdlib.h>


// argp details START //
const char *argp_program_version = "deskimg-pre-p-rep-realpha";
const char *argp_program_bug_address = "no@no.com";
static char doc[] = "later";
static char args_doc[] = "later 2";
static struct
argp_option options[] = {
  { 0, 0, 0, 0, "General Options:", 1 },
  { "opacity", 'o', "opacity_amount", 0, "Set the opacity of the graphic." },
  { "decoratewindow", 'd', 0, 0, "Decorate the window." }
};
// argp details END //

// global option vars START //
double opacity;
gboolean decorate = (gboolean) FALSE;
char* filename = "/home/josh/Pictures/giphy.gif";
// global option vars END //


static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key) 
  {
    case 'o':
      opacity = strtod(arg, NULL);
      break;
    case 'd':
      printf("decorate\n");
      decorate = (gboolean) TRUE;
      break;
    case ARGP_KEY_ARG:
      printf("arg\n");
      filename = (char*) malloc(sizeof(arg));
      strcpy(filename, arg);
      break;
  }
}

static void
activate (GtkApplication *app, gpointer user_data)
{
  printf("starting app\n");
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");

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

  gtk_window_set_decorated (GTK_WINDOW(window), FALSE);

  gtk_widget_set_opacity(window, 0.8);

  gtk_widget_show_all (window);

}

static void
open (GtkApplication* app, GFile** files, gint n_files, const gchar *hint)
{
  gint i;
  n_files = 0;
}

static void
command_line (GtkApplication* app, GApplicationCommandLine* cmdline)
{
  int p = 0;
}

static void
handle_local_options (GtkApplication *app, GVariantDict* options)
{
  int p = 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int
main (int argc, char **argv)
{
  printf("before\n");
  argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, 0);
  printf("after\n");

  GtkApplication *app;
  int status;

  printf("%s\n", filename);

  app = gtk_application_new ("org.gtkmm.examples.base", (G_APPLICATION_HANDLES_OPEN & G_APPLICATION_HANDLES_COMMAND_LINE));

  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  printf("1\n");
  g_signal_connect_swapped (app, "open", G_CALLBACK (open), NULL);
  printf("2\n");
  g_signal_connect_swapped (app, "command-line", G_CALLBACK (command_line), app);
  printf("3\n");
  g_signal_connect_swapped (app, "handle-local-options", G_CALLBACK (handle_local_options), app);

  printf("here baby\n");

  status = g_application_run (G_APPLICATION (app), argc, argv);

  
  g_object_unref (app);



  return status;
}
