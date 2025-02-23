#include "gtk_ansi.h"

static void demo(GtkAnsiParser* parser) {
    // Shows the same ansi strings as demo.sh
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            char str[18];
            snprintf(str, sizeof(str), "\033[48;5;%dm  \033[0m", y * 16 + x);
            str[17] = 0;
            gtk_ansi_append(parser, str);
        }
        gtk_ansi_append_line_feed(parser);
    }
    gtk_ansi_append(parser, "\033[1mBold\033[0m\033[3mItalic\033[0m");
    gtk_ansi_append(parser, "\033[4mUnderline\033[0m\n");
    gtk_ansi_append(parser, "\033[9mStrikethrough\033[0m");
    gtk_ansi_append(parser, "\033[7mReverse\033[0m\033[8mConceal\033[0m\n");
    gtk_ansi_append(parser, "0123456789abcdefghjklmn\rCarriage returns!\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
    (void) user_data;  // UNUSED

    GtkWidget *window;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;

    // Create a new window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GtkAnsiParser Demo");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 360);

    // Create a scrolled window
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    // Create a TextView widget
    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

    // Use monospace font
    GtkStyleContext* widget = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_class(widget, "monospace");

    // Make GtkAnsiParser
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkAnsiParser *ansi_parser = gtk_ansi_new(buf);
    gtk_ansi_set_default_color_with_textview(ansi_parser, GTK_TEXT_VIEW(text_view));

    // Add TextView to the scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    // Show all widgets
    gtk_widget_show_all(window);

    demo(ansi_parser);
}

int main(int argc, char** argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.GtkAnsiParser", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
