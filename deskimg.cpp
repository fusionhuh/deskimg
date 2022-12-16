#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <stack>

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

using namespace std;

void move_window(GtkWindow* window) { // move window to mouse position
    gint root_x;
    gint root_y;

    gint x_dim; // x- and y-dimensions of the window respectively.
    gint y_dim; //

    gtk_window_get_position(GTK_WINDOW (window), &root_x, &root_y);

    gtk_window_get_size(GTK_WINDOW (window), &x_dim, &y_dim);

    gtk_window_begin_move_drag(GTK_WINDOW (window), 1, root_x + x_dim/2, root_y + y_dim/2, 0);
}

void toggle_window_depth(GtkWindow* window) { // toggle whether or not window sits on bottom or top of screen
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

void save_window(GtkWindow* window) {
    printf("Saving... ");

    gint root_x, root_y;

    gtk_window_get_position (GTK_WINDOW (window), &root_x, &root_y);

    std::string user = std::string(getenv("HOME")) + "/";
    std::string savedir = user + ".config/deskimg/";
    std::string config_path = savedir + "config.sh";

    std::string command; command.resize(512,'n');
    snprintf (
        command.data(), 512, 
        "%s -f %s -o %f -x %i %s--y_pos=%i & #%d\n", 
        "deskimg ", filename, opacity, root_x, 
        toggle ? "-t " : "", // Use this format for adding optional arguments in future
        root_y, 
        getpid()
    );
    for (uint16_t i = 0; i < 512; i++) {
        if (command[i] == '\n') {
            command.resize(i + 1);
            command.shrink_to_fit();
            break;
        }
    }
    if (command.back() != '\n') {
        std::cout << command;
        std::cout << "Command size: " + command.size();
        std::cerr << "Something went wrong while creating the command string.\n";
        exit(1);
    }

    mkdir (savedir.c_str(), 0777);
    std::fstream config_file; config_file.open(config_path.c_str(), std::ios::in | std::ios::app);
    if (!config_file) {
        std::cerr << "Config file could not be opened.\n";
    }

//  first check that file is empty or not (no /bin/bash as first line)
    std::string first_line; std::getline(config_file, first_line);
    config_file.clear(); // clear failbit
    if (first_line.compare("#!/bin/bash") != 0) {
        config_file << "#!/bin/bash\n";
        config_file << command;
        std::cout << command;
        config_file.close();
        printf ("saved!\n");
        return;
    }

//  file is not empty, need to look for PID
    std::string new_file_contents = "#!/bin/bash\n";
    bool found_pid = false;
    while(config_file) {
        std::string curr_line; std::getline(config_file, curr_line);
        if (curr_line.find(std::to_string(getpid())) != std::string::npos) { // PID is found in file, need to replace with new line
            new_file_contents += command;
            found_pid = true;
        }
        else {
            if (curr_line.size() > 0) { // avoid re-adding empty lines
                new_file_contents += curr_line + "\n";
            }
        }
    }
    config_file.close(); config_file.open(config_path.c_str(), std::ios::trunc | std::ios::out);
//  open for write mode, create new file
    config_file << new_file_contents;

//  Check that PID existed in file. If it DOESN'T, the PID was never in the file and we need to add the command we created earlier.
    if (!found_pid) {
        config_file << command;
    }

    config_file << std::endl;

    printf ("saved!\n");
}

static gboolean
clicked (GtkWidget* window, GdkEventButton* event, gpointer user_data)
{
    // left click
    if (event->button == 1) // GDK_2BUTTON_PRESS corresponds to double click. See https://docs.gtk.org/gdk3/struct.EventButton.html for more info.
    {
        move_window(GTK_WINDOW(window));
    }
    // double middle click
    if (event->button == 2 && event->type == GDK_2BUTTON_PRESS) // Emergency exit for processes launched on startup
    {
        gdk_window_destroy (GDK_WINDOW(window));
    }
    // right click
    if (event->button == 3)
    {
        toggle_window_depth(GTK_WINDOW(window));
    }
    // double right click
    if (event->button == 3 && event->type == GDK_2BUTTON_PRESS) 
    {
        save_window(GTK_WINDOW(window));
    }
    return true;
}

static void
activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget* window;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");

    g_signal_connect (window, "button-press-event", G_CALLBACK (clicked), NULL);
    g_signal_connect_swapped (window, "destroy", G_CALLBACK (g_application_quit), app);

    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);             // Adds click event to window widget.

    GtkWidget *image;
    image = gtk_image_new_from_file (filename);                                 // Loads the image located at the path specified by the -f option.

    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(window, TRUE);                                 // Makes the window background
    screen = gdk_screen_get_default();                                                    // transparent, allowing view
    visual = gdk_screen_get_rgba_visual(screen);                                // to the desktop.
    if (visual!= NULL && gdk_screen_is_composited(screen))            //
    {                                                                                                                     //
        gtk_widget_set_visual(window, visual);                                        //
    }                                                                                                                     //

    gtk_container_add (GTK_CONTAINER (window), image);

    gtk_window_set_decorated (GTK_WINDOW (window), FALSE);            // Removes the border around the window.

    gtk_widget_set_opacity (window, opacity);                                        // Sets the window to the opacity specified by options.

    gtk_window_set_keep_below (GTK_WINDOW (window), TRUE);    // Keeps the window below or above all other windows on screen.

    gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), TRUE); // Removes the taskbar icon that would normally appear.

    gtk_widget_show_all (window);

    gtk_window_move (GTK_WINDOW (window),
     x_pos, y_pos);    // Sets the coordinates of the top left corner of the window.
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

    g_application_set_flags(G_APPLICATION(app), G_APPLICATION_NON_UNIQUE);

    g_application_add_main_option_entries (G_APPLICATION(app), options);    // Adds the options in the GOptionEntry array to the commmand line parser(?)

    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    g_signal_connect_swapped (app, "handle-local-options", G_CALLBACK (handle_local_options), app);

    status = g_application_run (G_APPLICATION (app), argc, argv);

    g_object_unref (app);

    return status;
}
