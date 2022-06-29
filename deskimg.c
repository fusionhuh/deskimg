#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

// global option vars START //
gdouble opacity = 1;
gboolean decorate = FALSE;
gchar* filename;
gint x_pos = 0;
gint y_pos = 0;
gboolean save = FALSE;
gboolean toggle = FALSE;
gboolean top = FALSE;
gboolean bottom = TRUE;
// global option vars END //

static gboolean
clicked (GtkWidget* window, GdkEventButton* event, gpointer user_data)
{
  if (event->button == 1) // GDK_2BUTTON_PRESS corresponds to double click. See https://docs.gtk.org/gdk3/struct.EventButton.html for more info.
  {
    gint root_x;
    gint root_y;

    gint x_dim; // x- and y-dimensions of the window respectively.
    gint y_dim; //

    gtk_window_get_position (GTK_WINDOW (window), &root_x, &root_y);

    gtk_window_get_size (GTK_WINDOW (window), &x_dim, &y_dim);

    gtk_window_begin_move_drag (GTK_WINDOW (window), 1, root_x + x_dim/2, root_y + y_dim/2, 0);
  }
  if (event->button == 2 && event->type == GDK_2BUTTON_PRESS) // Emergency exit for processes launched on startup
  {
    gdk_window_destroy (GTK_WINDOW (window));
  }
  if (event->button == 3)
  {
    if (toggle)
    {
      if (bottom)
      {
        gtk_window_set_keep_above (GTK_WINDOW (window), TRUE);
        bottom = FALSE;
        top = TRUE;
      }
      else if (top)
      {
        gtk_window_set_keep_below (GTK_WINDOW (window), TRUE);
        top = FALSE;
        bottom = TRUE;
      }
    }
  }
  if (event->button == 3 && event->type == GDK_2BUTTON_PRESS) // Double right-click
  {
    if (save)
    {
      printf("Saving... ");

      gint root_x = 0;
      gint root_y = 0;

      gtk_window_get_position (GTK_WINDOW (window), &root_x, &root_y);

      const char* user = strcat (getenv("HOME"), "/");

      char* savedir = (char*) calloc (strlen (user) + strlen (".config/deskimg/") + 1, sizeof (char));
      strcat (savedir, user);
      strcat (savedir, ".config/deskimg/");
      
      char* config_path = (char*) calloc (strlen (savedir) + strlen ("config.sh") + 1, sizeof (char));
      strcat (config_path, savedir);
      strcat (config_path, "config.sh");

      // Get the PID of the process. Will be used to check if we've already written to the config.sh file from this process.
      // If we have written to it, we don't add a new line and simply modify the line that currently exists.
      char pid[7] = {0}; // 7 characters is the max length of a PID
      snprintf(pid, 7, "%d", getpid()); 

      const char* deskimg = "deskimg"; // should update this to be argv[0] later

      char command_buf[256] = {0};
      snprintf (
        &command_buf, 256, 
        "%s -f %s -o %f -x %i %s--y_pos=%i & #%s\n", 
        deskimg, filename, opacity, root_x, 
        toggle ? "-t " : "", // Use this format for adding optional arguments in future
        root_y, pid
      );

      int command_size = 0;
      while (command_buf[command_size] != 10)
      {
        command_size++;
      }
      command_size++;

      char* command = (char*) calloc(command_size, sizeof (char));
      strncat (command, &command_buf, command_size);

      FILE* config;

      mkdir (savedir, 0777);
      if (!(config = fopen (config_path, "r+"))) // If the file does not open with read flag, we must create the file using the write flag
      {
        config = fopen (config_path, "w+");
      }

      fseek (config, 0, SEEK_END);         // Determine the size in bytes of the file.
      int config_size = ftell (config);    //
      rewind (config);                     //

      if (config_size > 0) // Config file is not empty, need to parse to see if PID is in file.
      {
        char* buffer = (char*) malloc (sizeof (char) * config_size);
        fread (buffer, 1, config_size, config);

        if (strstr (buffer, pid) != NULL) // PID is found in file, need to find line on which it occurs to update it.
        {
          int pointer = 0;

          while (pointer < config_size)
          {
            if (buffer[pointer] == '#') // Comment found, need to check if PID is the same
            {
              pointer++; // Increment pointer to start on character after '#'

              char new_pid[1] = "";

              while (buffer[pointer] != 10 && buffer[pointer] != 32 && pointer < config_size) // 10 is the newline char in ASCII; need to also check for space char bc comment might not be PID 
              {
                strncat (new_pid, &buffer[pointer], 1);
                pointer++;
              }

              if (strcmp (pid, new_pid) == 0) // PID matches! Find the left and right bounds and eliminate the line.
              {
                int left_pointer = pointer - 1; // Decrement left pointer to start before newline or EOF

                while (buffer[left_pointer] != 10 && left_pointer > 0)
                {
                  left_pointer--;
                }

                pointer++; // Increment right pointer so we start cloning AFTER the right line break (i.e. leave it out of the buffer)

                char* left_buffer = (char*) malloc (sizeof (char) * (left_pointer+1));
                char* right_buffer = (char*) malloc (sizeof (char) * (config_size - pointer));

                fseek (config, 0, SEEK_SET); // Set file pointer to the beginning of the file.
                fread (left_buffer, 1, left_pointer + 1, config);
                fseek (config, 0, pointer); // Set file pointer to start reading after the line we are replacing.
                fread (right_buffer, 1, config_size - pointer, config); // Right buffer contains all of the text AFTER the line.

                fclose(config);
                config = fopen(config_path, "w+");

                fwrite (left_buffer, 1, left_pointer+1, config);
                fwrite (command, 1, command_size, config);
                fwrite (right_buffer, 1, config_size - pointer, config);

                free(left_buffer);
                free(right_buffer);

                pointer = config_size; // Break outermost loop
              }
              // PID doesn't match; increment pointer and start searching again
            }
            pointer++;
          }
        }
        else // PID is not found, can simply write command at the end of the file.
        {
          fseek (config, 0, SEEK_END);
          fwrite ((char*) "\n", 1, 1, config); // Writing newline char to file in case it isn't at the end.
          fwrite (command, 1, command_size, config);
        }

        free (buffer);
      }
      else // Config file is empty.
      {
        const char* bash = "#!/bin/bash\n";
        fwrite (bash, 1, strlen (bash), config);
        fwrite (command, 1, command_size, config);
      }

      free (command);
      free (savedir);
      free (config_path);

      fclose (config);

      printf ("saved!\n");
    }
  }
}

static void
activate (GtkApplication* app, gpointer user_data)
{
  GtkWidget* window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");

  g_signal_connect (window, "button-press-event", G_CALLBACK (clicked), NULL);
  g_signal_connect_swapped (window, "destroy", G_CALLBACK (g_application_quit), app);

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

  gtk_widget_set_opacity (window, opacity);                    // Sets the window to the opacity specified by options.

  gtk_window_set_keep_below (GTK_WINDOW (window), TRUE);  // Keeps the window below or above all other windows on screen.

  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), TRUE); // Removes the taskbar icon that would normally appear.

  gtk_widget_show_all (window);

  gtk_window_move (GTK_WINDOW (window),
   x_pos, y_pos);  // Sets the coordinates of the top left corner of the window.
}

static int
handle_local_options (GtkApplication *app, GVariantDict* options)
{
  g_variant_dict_lookup (options, "file", "s", &filename);
  g_variant_dict_lookup (options, "opacity", "d", &opacity);
  g_variant_dict_lookup (options, "x_pos", "i", &x_pos);
  g_variant_dict_lookup (options, "y_pos", "i", &y_pos);
  g_variant_dict_lookup (options, "save", "b", &save);
  g_variant_dict_lookup (options, "toggle", "b", &toggle);
  gboolean reset = FALSE;
  g_variant_dict_lookup (options, "reset", "b", &reset);

  if (reset)
  {
    char* user = strcat (getenv("HOME"), "/");

    char* savedir = (char*) malloc(sizeof (char) * strlen (user));
    strcpy (savedir, user);
    strcat (savedir, ".config/deskimg/");
      
    char* config_path = (char*) malloc(sizeof (char) * strlen (savedir));
    strcpy (config_path, savedir);
    strcat (config_path, "config.sh");

    FILE* config = fopen (config_path, "w+");
    fclose (config);

    printf ("Launched program to reset, exiting...\n");
    exit (1);
  }
  return -1;
}

static const 
GOptionEntry options[] =
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
    NULL, "Set the x-position of the graphic on the screen.", 
    "Integer values only."
  },

  /// *** Y-Position
  {
    "y_pos", 'y', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT,
    NULL, "Set the y-position of the graphic on the screen.", "Integer values only."
  },

  /// *** Save
  {
    "save", 's', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE,
    NULL, "Allow the program to save the current graphic and its position to the config file located in ~/.config/deskimg/config.sh.", NULL
  },

  /// *** Toggle
  {
    "toggle", 't', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE,
    NULL, "Allows you to toggle between keeping the image on top of all other windows or below." 
    " Toggle by right clicking on the graphic.", NULL
  },

  /// *** Reset
  {
    "reset", 'r', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE,
    NULL, "Resets the config file. Will immediately exit after config file is reset.", NULL
  },

  { NULL }
};

int
main (int argc, char *argv[])
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtkmm.examples.base", G_APPLICATION_HANDLES_OPEN);

  g_application_set_flags(app, G_APPLICATION_NON_UNIQUE);

  g_application_add_main_option_entries (app, options);  // Adds the options in the GOptionEntry array to the commmand line parser(?)

  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect_swapped (app, "handle-local-options", G_CALLBACK (handle_local_options), app);

  status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return status;
}
