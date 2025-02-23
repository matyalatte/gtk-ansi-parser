#ifndef __GTK_ANSI_PARSER_INCLUDE_GTK_ANSI_H__
#define __GTK_ANSI_PARSER_INCLUDE_GTK_ANSI_H__

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

// Export APIs when shared build
#ifndef _GTKA_EXTERN
#ifdef _TSM_STATIC
#define _GTKA_EXTERN extern
#else  // _TSM_STATIC
#ifdef _WIN32
#define _GTKA_EXTERN __declspec(dllexport) extern
#else  // _WIN32
#define _GTKA_EXTERN __attribute__((visibility("default"))) extern
#endif  // _WIN32
#endif  // _TSM_STATIC
#endif  // _GTKA_EXTERN

// Version info
#define GTKA_VERSION "0.1.0"
#define GTKA_VERSION_INT 100

/**
 * Gets the version of gtk-ansi-parser.
 * It should be of the form `x.x.x`.
 *
 * @returns A string that represents the version.
 */
_GTKA_EXTERN const char* gtk_ansi_get_version();

/**
 * Gets the version of gtk-ansi-parser as an integer.
 * The value should be `major * 10000 + minor * 100 + patch`.
 * If `gtk_ansi_get_version() == "1.2.3"` then `gtk_ansi_get_version_as_int() == 10203`.
 *
 * @returns An integer that represents the version.
 */
_GTKA_EXTERN int gtk_ansi_get_version_as_int();

/**
 * Class for an ansi text buffer.
 *
 * @struct GtkAnsiParser
 */
typedef struct GtkAnsiParser GtkAnsiParser;

/**
 * Creates a new object for GtkAnsiParser instance.
 *
 * @param buffer GtkTextBuffer instance.
 * @returns GtkAnsiParser instance.
 *          Caller is responsible for freeing the data with `gtk_ansi_free()`
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN GtkAnsiParser* gtk_ansi_new(GtkTextBuffer* buffer);

/**
 * Frees the memory of a GtkAnsiParser object.
 *
 * @param parser GtkAnsiParser instance.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_free(GtkAnsiParser* parser);

/**
 * Initializes GtkAnsiParser.
 * @note `gtk_ansi_new()` calls it internally.
 *
 * @param parser GtkAnsiParser instance.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_init(GtkAnsiParser* parser);

/**
 * Sets the max length of the text buffer. The default value is 1 MiB (1048576).
 * @note GtkAnsiParser removes the first bytes when the buffer size exceeds max_len.
 *
 * @param parser GtkAnsiParser instance.
 * @param max_len Max length of the text buffer.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_set_max_length(GtkAnsiParser* parser, int max_len);

/**
 * Gets the max length of the text buffer.
 *
 * @param parser GtkAnsiParser instance.
 * @returns Max length of the text buffer.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN int gtk_ansi_get_max_length(GtkAnsiParser* parser);

/**
 * Gets the current length of the text buffer.
 *
 * @param parser GtkAnsiParser instance.
 * @returns Current length of the text buffer.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN int gtk_ansi_get_length(GtkAnsiParser* parser);

/**
 * Sets the default foreground or background color.
 * Reversed or hidden characters (with `\033[7m` or `\033[8m`]) use them.
 *
 * @param parser GtkAnsiParser instance.
 * @param is_bg 0 for foreground, non-zero for background.
 * @param r Red (0 ~ 255)
 * @param g Green (0 ~ 255)
 * @param b Blue (0 ~ 255)
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_set_default_color(
    GtkAnsiParser* parser, int is_bg, int r, int g, int b);

/**
 * Copies the default fore and back colors from a text view.
 * Reversed or hidden characters (with `\033[7m` or `\033[8m`]) use them.
 *
 * @param parser GtkAnsiParser instance.
 * @param view GtkTextView instance.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_set_default_color_with_textview(
    GtkAnsiParser* parser, GtkTextView* view);

/**
 * Gets the default foreground or background color.
 * You can use NULL if you don't need one of them.
 *
 * @param parser GtkAnsiParser instance.
 * @param is_bg 0 for foreground, non-zero for background.
 * @param r Red (0 ~ 255)
 * @param g Green (0 ~ 255)
 * @param b Blue (0 ~ 255)
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_get_default_color(
    GtkAnsiParser* parser, int is_bg, int* r, int* g, int* b);

/**
 * Gets the text buffer as a new char string.
 *
 * @param parser GtkAnsiParser instance.
 * @returns Text data.
 *          Caller is responsible for freeing the data with `gtk_ansi_free_text()`.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN char* gtk_ansi_get_text(GtkAnsiParser* parser);

/**
 * Frees the memory of a string allocated by `gtk_ansi_get_text()`.
 *
 * @param text A pointer to a string.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_free_text(char* text);

/**
 * Gets GtkTextBuffer from GtkAnsiParser
 *
 * @param parser GtkAnsiParser instance.
 * @returns GtkTextBuffer instance.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN GtkTextBuffer* gtk_ansi_get_text_buffer(GtkAnsiParser* parser);

/**
 * Appends a string to buffer.
 * It can handle ansi escape sequences and carriage returns.
 *
 * @param parser GtkAnsiParser instance.
 * @param text A pointer to a string
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_append(GtkAnsiParser* parser, const char* text);

/**
 * Appends a line feed to buffer.
 * It's a little bit faster than `gtk_ansi_append(parser, "\n")`
 *
 * @param parser GtkAnsiParser instance.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_append_line_feed(GtkAnsiParser *parser);

/**
 * Removes the first bytes from the text buffer.
 *
 * @param parser GtkAnsiParser instance.
 * @param n Byte size that you want to remove.
 * @returns The size of removed bytes.
 *          It can be smaller than n when the buffer becomes empty.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN int gtk_ansi_remove_first_bytes(GtkAnsiParser* parser, int n);

/**
 * Disables applied tags. (Applies `\033[0m`.)
 *
 * @param parser GtkAnsiParser instance.
 * @memberof GtkAnsiParser
 */
_GTKA_EXTERN void gtk_ansi_reset_tags(GtkAnsiParser *parser);

#ifdef __cplusplus
}
#endif

#endif  // __GTK_ANSI_PARSER_INCLUDE_GTK_ANSI_H__
