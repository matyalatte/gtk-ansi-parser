#include "gtk_ansi.h"

static void demo(GtkAnsiParser* parser) {
    gtk_ansi_append(parser, "???this is the first line\n");
    gtk_ansi_append(parser, "\033[31mHello, GTK!\rWhat\033[0m\n");
    gtk_ansi_append(parser, "\033[30;47mblack\033[0m\n");
    gtk_ansi_append(parser, "\033[31;46mred\033[0m\n");
    gtk_ansi_append(parser, "\033[32;45mgreen\033[0m\n");
    gtk_ansi_append(parser, "\033[33;44myellow\033[0m\n");
    gtk_ansi_append(parser, "\033[34;43mblue\033[0m\n");
    gtk_ansi_append(parser, "\033[35;42mmagenta\033[0m\n");
    gtk_ansi_append(parser, "\033[36;41mcyan\033[0m\n");
    gtk_ansi_append(parser, "\033[37;40mwhite\033[0m\n");
    gtk_ansi_append(parser, "\033[30;47mblack\033[0m\n");
    gtk_ansi_append(parser, "\033[91;106mred\033[0m\n");
    gtk_ansi_append(parser, "\033[92;105mgreen\033[0m\n");
    gtk_ansi_append(parser, "\033[93;104myellow\033[0m\n");
    gtk_ansi_append(parser, "\033[94;103mblue\033[0m\n");
    gtk_ansi_append(parser, "\033[95;102mmagenta\033[0m\n");
    gtk_ansi_append(parser, "\033[96;101mcyan\033[0m\n");
    gtk_ansi_append(parser, "\033[97;100mwhite\033[0m\n");
    gtk_ansi_append(parser, "\033[1;3;4;9mdeco\033[0m\n");
    gtk_ansi_append(parser, "test\033[0m\nmessage\nhello\n");
    gtk_ansi_append(parser, "what\rwhy you.\n");
    gtk_ansi_remove_first_bytes(parser, 3);
}

static void activate(GtkApplication *app, gpointer user_data) {
    (void) user_data;  // UNUSED

    GtkWidget *window;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;

    // Create a new window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GtkAnsiParser Demo");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create a scrolled window
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    // Create a TextView widget
    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

    // Use monospace font
    GtkStyleContext* widget = gtk_widget_get_style_context(text_view);
	gboolean has_monospace = gtk_style_context_has_class(widget, "monospace");
	if (!has_monospace)
		gtk_style_context_add_class(widget, "monospace");

    // Make GtkAnsiParser
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkAnsiParser *ansi_parser = gtk_ansi_new(buf);

    // Add TextView to the scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    // Show all widgets
    gtk_widget_show_all(window);

    demo(ansi_parser);
}

int main(int argc, char** argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.GtkAnsiParser", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
