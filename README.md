# gtk-ansi-parser
ANSI string parser for GtkTextBuffer

## Example

```c
    // Allocate memory
    GtkTextBuffer* buffer = gtk_text_buffer_new(nullptr);
    GtkAnsiParser* parser = gtk_ansi_new(buffer);

    // Remove the first bytes when the buffer size exceeds 512KiB.
    gtk_ansi_set_max_length(parser, 512 * 1024);

    // Append ANSI strings.
    gtk_ansi_append("\033[1mBold\033[0m\n");
    gtk_ansi_append("\033[31mRed\033[32mBlue\033[0m\n");
    gtk_ansi_append("\033[38;2;0;255;0mGreen\033[0m\n");

    // Free memory
    g_object_unref(buffer);
    gtk_ansi_free(parser);
```
