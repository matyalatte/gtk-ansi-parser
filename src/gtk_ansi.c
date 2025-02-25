#include "gtk_ansi.h"
#include "gtk_ansi_private.h"

// Color names for GtkTextTag
static const char* COLORS[16] = {
    "#000000",  // ANSI_COLOR_BLACK
    "#cd0000",  // ANSI_COLOR_RED
    "#00cd00",  // ANSI_COLOR_GREEN
    "#cdcd00",  // ANSI_COLOR_YELLOW
    "#0000ee",  // ANSI_COLOR_BLUE
    "#cd00cd",  // ANSI_COLOR_MAGENTA
    "#00cdcd",  // ANSI_COLOR_CYAN
    "#e5e5e5",  // ANSI_COLOR_WHITE
    "#7f7f7f",  // ANSI_COLOR_BRIGHT_BLACK
    "#ff0000",  // ANSI_COLOR_BRIGHT_RED
    "#00ff00",  // ANSI_COLOR_BRIGHT_GREEN
    "#ffff00",  // ANSI_COLOR_BRIGHT_YELLOW
    "#5c5cff",  // ANSI_COLOR_BRIGHT_BLUE
    "#ff00ff",  // ANSI_COLOR_BRIGHT_MAGENTA
    "#00ffff",  // ANSI_COLOR_BRIGHT_CYAN
    "#ffffff",  // ANSI_COLOR_BRIGHT_WHITE
};

const char* gtk_ansi_get_version() {
    return GTKA_VERSION;
}

int gtk_ansi_get_version_as_int() {
    return GTKA_VERSION_INT;
}

GtkAnsiParser* gtk_ansi_new(GtkTextBuffer* buffer) {
    if (!buffer)
        return NULL;

    GtkAnsiParser* parser = g_malloc0(sizeof(GtkAnsiParser));
    if (!parser)
        return NULL;

    parser->Buffer = buffer;
    parser->TagTable = gtk_text_buffer_get_tag_table(buffer);
    parser->BlinkFuncId = 0;
    parser->BlinkTag = NULL;
    parser->BlinkVisible = 0;
    gtk_ansi_init(parser);
    return parser;
}

void gtk_ansi_free(GtkAnsiParser* parser) {
    if (parser->BlinkFuncId)
        g_source_remove(parser->BlinkFuncId);
    g_free(parser);
}

void gtk_ansi_reset_tags(GtkAnsiParser* parser) {
    if (!parser)
        return;
    parser->UseBold = 0;
    parser->UseFaint = 0;
    parser->UseBlink = 0;
    parser->UseReverse = 0;
    parser->UseItalic = 0;
    parser->UseUnderline = 0;
    parser->UseStrikethrough = 0;
    parser->UseConceal = 0;
    parser->FgColor = NULL;
    parser->BgColor = NULL;
}

static void copy_color(char* dest, const char* src) {
    // Copy "#rrggbb" from src to dest
    memcpy(dest, src, 7);
    dest[7] = 0;
}

void gtk_ansi_init(GtkAnsiParser* parser) {
    if (!parser)
        return;
    gtk_text_buffer_set_text(parser->Buffer, "", 0);
    parser->Length = 0;
    parser->CursorPos = 0;
    parser->LineStartPos = 0;
    parser->MaxLength = 1024 * 1024;
    copy_color(parser->FgColorDefault, COLORS[ANSI_COLOR_BLACK]);
    copy_color(parser->BgColorDefault, COLORS[ANSI_COLOR_BRIGHT_WHITE]);
    copy_color(parser->FgColorCustom, "#ffffff");
    copy_color(parser->BgColorCustom, "#ffffff");
    gtk_ansi_reset_tags(parser);
}

void gtk_ansi_set_max_length(GtkAnsiParser* parser, int max_len) {
    if (!parser)
        return;
    parser->MaxLength = max_len;
}

int gtk_ansi_get_max_length(GtkAnsiParser* parser) {
    if (!parser)
        return 0;
    return parser->MaxLength;
}

int gtk_ansi_get_length(GtkAnsiParser* parser) {
    if (!parser)
        return 0;
    return parser->Length;
}

void gtk_ansi_set_default_color(
        GtkAnsiParser* parser, int is_bg, int r, int g, int b) {
    if (!parser)
        return;
    char* color = is_bg ? parser->BgColorDefault : parser->FgColorDefault;
    snprintf(color, sizeof(parser->BgColorDefault),
             "#%02x%02x%02x", r % 256, g % 256, b % 256);
}

#define D2I(n) (int)(n * 255)

void gtk_ansi_set_default_color_with_textview(
        GtkAnsiParser* parser, GtkTextView* view) {
    if (!parser || !view)
        return;
    GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(view));
    if (!context)
        return;

    // Get foreground color
    GdkRGBA color;
    gtk_style_context_get_color(context, GTK_STATE_FLAG_NORMAL, &color);
    gtk_ansi_set_default_color(parser, 0, D2I(color.red), D2I(color.green), D2I(color.blue));

    // Get background color
    // Note: Using gtk_style_context_* does not work with some gtk themes.
    //       So, we render 1x1 window and read a pixel.
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
    cairo_t *cr = cairo_create(surface);
    gtk_render_background(context, cr, 0, 0, 1, 1);
    unsigned char *data = cairo_image_surface_get_data(surface);
    unsigned char blue = data[0];
    unsigned char green = data[1];
    unsigned char red = data[2];
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    gtk_ansi_set_default_color(parser, 1, red, green, blue);
}

static int str_to_uint8_unsafe(const char* str) {
    int n = 0;
    for (int i = 0; i < 2; i++) {
        n *= 16;
        char c = *str++;
        if ('0' <= c && c <= '9')
            n += c - 48;
        else if ('A' <= c && c <= 'F')
            n += c - 55;
        else if ('a' <= c && c <= 'f')
            n += c - 87;
    }
    return n;
}

void gtk_ansi_get_default_color(
    GtkAnsiParser* parser, int is_bg, int* r, int* g, int* b) {
    if (!parser)
        return;
    const char* color = is_bg ? parser->BgColorDefault : parser->FgColorDefault;
    if (r)
        *r = str_to_uint8_unsafe(color + 1);
    if (g)
        *g = str_to_uint8_unsafe(color + 3);
    if (b)
        *b = str_to_uint8_unsafe(color + 5);
}

char* gtk_ansi_get_text(GtkAnsiParser* parser) {
    if (!parser)
        return NULL;
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(parser->Buffer, &start);
    gtk_text_buffer_get_end_iter(parser->Buffer, &end);
    return gtk_text_buffer_get_text(parser->Buffer, &start, &end, TRUE);
}

void gtk_ansi_free_text(char* text) {
    g_free(text);
}

GtkTextBuffer* gtk_ansi_get_text_buffer(GtkAnsiParser* parser) {
    if (!parser)
        return NULL;
    return parser->Buffer;
}

gboolean toggle_blink(gpointer data) {
    GtkAnsiParser* parser = (GtkAnsiParser *)data;

    // Change foreground color for blinking
    parser->BlinkVisible = !parser->BlinkVisible;
    if (!parser->BlinkTag)
        return TRUE;
    if (parser->BlinkVisible)
        g_object_set(parser->BlinkTag, "foreground",
                     parser->BgColorDefault, "foreground-set", TRUE, NULL);
    else
        g_object_set(parser->BlinkTag, "foreground-set", FALSE, NULL);

    return TRUE;
}

static GtkTextTag* create_tag(GtkAnsiParser* parser, const char* name, const char* color) {
    GtkTextBuffer* buffer = parser->Buffer;
    GtkTextTag *new_tag = NULL;
    char c = name[0];
    char c2 = name[1];
    if (c == 'b' && c2 == 'o') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "weight", PANGO_WEIGHT_BOLD, NULL);
    } else if (c == 'i') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "style", PANGO_STYLE_ITALIC, NULL);
    } else if (c == 'u') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "underline", PANGO_UNDERLINE_SINGLE, NULL);
    } else if (c == 'b' && c2 == 'l') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, NULL);
        parser->BlinkTag = new_tag;
        if (!parser->BlinkFuncId)
            parser->BlinkFuncId = g_timeout_add(600, toggle_blink, parser);
    } else if (c == 's') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "strikethrough", TRUE, NULL);
    } else if (c == 'f') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "foreground", color, NULL);
        // Use the lowest priority to prioritize the blinking tag
        gtk_text_tag_set_priority(new_tag, 0);
    } else if (c == 'b' && c2 == 'g') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "background", color, NULL);
    }
    return new_tag;
}

// Applies a text tag
static void gtk_ansi_apply_tag_base(GtkAnsiParser* parser,
                                    const char* name, const char* color,
                                    GtkTextIter* start, GtkTextIter* end) {
    char color_tag[10];
    if (color) {
        // ".g#rrggbb"
        memcpy(color_tag, name, 2);
        copy_color(color_tag + 2, color);
        name = color_tag;
    }
    GtkTextTag* tag = gtk_text_tag_table_lookup(parser->TagTable, name);
    if (!tag)
        tag = create_tag(parser, name, color);
    if (!tag)
        return;
    gtk_text_buffer_apply_tag(parser->Buffer, tag, start, end);
}

// Applies enabled text tags
static void gtk_ansi_apply_tags(GtkAnsiParser* parser, GtkTextIter* start, GtkTextIter* end) {
    if (parser->UseBold)
        gtk_ansi_apply_tag_base(parser, "bo", NULL, start, end);
    if (parser->UseItalic)
        gtk_ansi_apply_tag_base(parser, "it", NULL, start, end);
    if (parser->UseUnderline)
        gtk_ansi_apply_tag_base(parser, "un", NULL, start, end);
    if (parser->UseBlink)
        gtk_ansi_apply_tag_base(parser, "bl", NULL, start, end);
    if (parser->UseStrikethrough)
        gtk_ansi_apply_tag_base(parser, "st", NULL, start, end);

    const char* fgColor = parser->FgColor;
    const char* bgColor = parser->BgColor;
    char fgColorFainted[8];
    if (parser->UseFaint) {
        if (!fgColor)
            fgColor = parser->FgColorDefault;
        int r, g, b;
        r = str_to_uint8_unsafe(fgColor + 1);
        g = str_to_uint8_unsafe(fgColor + 3);
        b = str_to_uint8_unsafe(fgColor + 5);
        snprintf(fgColorFainted, sizeof(fgColorFainted),
            "#%02x%02x%02x", r / 2, g / 2, b / 2);
        fgColor = fgColorFainted;
    }
    if (parser->UseReverse) {
        // swap fg and bg colors
        if (!fgColor)
            fgColor = parser->FgColorDefault;
        if (!bgColor)
            bgColor = parser->BgColorDefault;
        const char* fdColor_ = fgColor;
        fgColor = bgColor;
        bgColor = fdColor_;
    }
    if (parser->UseConceal) {
        fgColor = bgColor;
        if (!fgColor)
            fgColor = parser->BgColorDefault;
    }
    if (fgColor)
        gtk_ansi_apply_tag_base(parser, "fg", fgColor, start, end);
    if (bgColor)
        gtk_ansi_apply_tag_base(parser, "bg", bgColor, start, end);
}

int gtk_ansi_remove_first_bytes(GtkAnsiParser* parser, int n) {
    if (!parser || n <= 0)
        return 0;
    if (n > parser->Length)
        n = parser->Length;
    GtkTextIter start, end;

    // Get the start iterator
    gtk_text_buffer_get_start_iter(parser->Buffer, &start);

    // Copy the start iterator and move it forward by 'n' characters
    end = start;
    gtk_text_iter_forward_chars(&end, n);

    // Delete text from start to the new position
    gtk_text_buffer_delete(parser->Buffer, &start, &end);

    parser->Length -= n;
    parser->CursorPos -= n;
    if (parser->CursorPos < 0)
        parser->CursorPos = 0;
    parser->LineStartPos -= n;
    if (parser->LineStartPos < 0)
        parser->LineStartPos = 0;
    return n;
}

// Overite the buffer with some bytes
static int gtk_ansi_overwrite_text(GtkAnsiParser* parser, const char* text, int len) {
    if (parser->Length - parser->CursorPos < len)
        len = parser->Length - parser->CursorPos;
    if (len <= 0)
        return 0;

    GtkTextIter start, end;

    // Get iterators at the calculated position
    gtk_text_buffer_get_iter_at_offset(parser->Buffer, &start, parser->CursorPos);
    gtk_text_buffer_get_iter_at_offset(parser->Buffer, &end, parser->CursorPos + len);

    // Delete last `byte_count` characters
    gtk_text_buffer_delete(parser->Buffer, &start, &end);

    // Insert new text at the same position
    gtk_text_buffer_insert(parser->Buffer, &start, text, len);
    end = start;
    gtk_text_iter_backward_chars(&end, len);
    gtk_ansi_apply_tags(parser, &start, &end);

    parser->CursorPos += len;
    return len;
}

// Append a plain text to the buffer.
static void gtk_ansi_append_text_base(GtkAnsiParser* parser, const char* text, int len) {
    if (parser->CursorPos != parser->Length) {
        int overwrite_len = gtk_ansi_overwrite_text(parser, text, len);
        text += overwrite_len;
        len -= overwrite_len;
    }
    if (len <= 0)
        return;

    if (parser->Length + len > parser->MaxLength) {
        int removed_len = parser->Length + len - parser->MaxLength;
        if (len > parser->MaxLength) {
            text += len - parser->MaxLength;
            len = parser->MaxLength;
            removed_len = parser->MaxLength;
        }
        gtk_ansi_remove_first_bytes(parser, removed_len);
    }

    GtkTextIter start, end;
    gtk_text_buffer_get_end_iter(parser->Buffer, &start);
    gtk_text_buffer_insert(parser->Buffer, &start, text, len);
    end = start;
    gtk_text_iter_backward_chars(&end, len);
    gtk_ansi_apply_tags(parser, &start, &end);
    parser->Length += len;
    parser->CursorPos = parser->Length;
}

void gtk_ansi_append_line_feed(GtkAnsiParser* parser) {
    if (!parser)
        return;
    if (parser->Length == parser->MaxLength) {
        gtk_ansi_remove_first_bytes(parser, 1);
    }
    GtkTextIter start;
    gtk_text_buffer_get_end_iter(parser->Buffer, &start);
    gtk_text_buffer_insert(parser->Buffer, &start, "\n", 1);
    parser->Length++;
    parser->CursorPos = parser->Length;
    parser->LineStartPos = parser->Length;
}

// Enables an ANSI code for the next appended characters.
// Returns ANSI_INVALID if you use an unsupported code.
static AnsiCode gtk_ansi_enable_tag_by_ansi(GtkAnsiParser* parser, AnsiCode code) {
    if (code == ANSI_RESET) {
        gtk_ansi_reset_tags(parser);
    } else if (code == ANSI_BOLD) {
        parser->UseBold = 1;
    } else if (code == ANSI_FAINT) {
        parser->UseFaint = 1;
    } else if (code == ANSI_NO_BOLD_FAINT) {
        parser->UseBold = 0;
        parser->UseFaint = 0;
    } else if (code == ANSI_ITALIC) {
        parser->UseItalic = 1;
    } else if (code == ANSI_NO_ITALIC) {
        parser->UseItalic = 0;
    } else if (code == ANSI_UNDERLINE) {
        parser->UseUnderline = 1;
    } else if (code == ANSI_NO_UNDERLINE) {
        parser->UseUnderline = 0;
    } else if (code == ANSI_BLINK || code == ANSI_RAPID_BLINK) {
        parser->UseBlink = 1;
    } else if (code == ANSI_NO_BLINK) {
        parser->UseBlink = 0;
    } else if (code == ANSI_STRIKETHROUGH) {
        parser->UseStrikethrough = 1;
    } else if (code == ANSI_NO_STRIKETHROUGH) {
        parser->UseStrikethrough = 0;
    } else if (code == ANSI_REVERSE) {
        parser->UseReverse = 1;
    } else if (code == ANSI_NO_REVERSE) {
        parser->UseReverse = 0;
    } else if (code == ANSI_CONCEAL) {
        parser->UseConceal = 1;
    } else if (code == ANSI_NO_CONCEAL) {
        parser->UseConceal = 0;
    } else if (ANSI_FG_BLACK <= code && code <= ANSI_FG_WHITE) {
        parser->FgColor = COLORS[code - ANSI_FG_BLACK];
    } else if (ANSI_BG_BLACK <= code && code <= ANSI_BG_WHITE) {
        parser->BgColor = COLORS[code - ANSI_BG_BLACK];
    } else if (ANSI_FG_BRIGHT_BLACK <= code && code <= ANSI_FG_BRIGHT_WHITE) {
        parser->FgColor = COLORS[code - ANSI_FG_BRIGHT_BLACK + ANSI_COLOR_BRIGHT_BLACK];
    } else if (code == ANSI_FG_CUSTOM) {
        parser->FgColor = parser->FgColorCustom;
    } else if (code == ANSI_FG_RESET) {
        parser->FgColor = NULL;
    } else if (ANSI_BG_BRIGHT_BLACK <= code && code <= ANSI_BG_BRIGHT_WHITE) {
        parser->BgColor = COLORS[code - ANSI_BG_BRIGHT_BLACK + ANSI_COLOR_BRIGHT_BLACK];
    } else if (code == ANSI_BG_CUSTOM) {
        parser->BgColor = parser->BgColorCustom;
    } else if (code == ANSI_BG_RESET) {
        parser->BgColor = NULL;
    } else {
        return ANSI_INVALID;
    }
    return code;
}

typedef int CheckAnsiResult;
#define ANSIRES_UNSUPPORTED 1
#define ANSIRES_UNFINISHED 2


int is_unsupported(CheckAnsiResult res) {
    return (res & ANSIRES_UNSUPPORTED) == ANSIRES_UNSUPPORTED;
}

int is_unfinished(CheckAnsiResult res) {
    return (res & ANSIRES_UNFINISHED) == ANSIRES_UNFINISHED;
}

CheckAnsiResult check_ansi_code(const char* text, int* len) {
    const char* p = text + 1;
    if (!*p) {
        // End with \033
        *len = 1;
        return ANSIRES_UNFINISHED;
    } else if (*p != '[') {
        // Unsupported sequence
        *len = 1;
        return ANSIRES_UNSUPPORTED;
    }

    p++;
    int res = 0;
    while (*p) {
        if (*p == '"') {
            // Skip a string
            res |= ANSIRES_UNSUPPORTED;
            const char* start = p;
            p++;
            while (*p && *p != '"' && (p - start < 64))
                p++;
            if (!*p) {
                break;
            }
            p++;
            continue;
        }
        if (*p < '0' || '?' < *p) {
            // End of sequence
            if (*p != 'm')
                res |= ANSIRES_UNSUPPORTED;
            break;
        } else if (*p == ':' || '<' <= *p) {
            res |= ANSIRES_UNSUPPORTED;
        }
        p++;
    }
    if (*p)
        p++;
    else
        res |= ANSIRES_UNFINISHED;
    *len = (int)(p - text);
    return res;
}

static AnsiCode read_ansi_code(const char* text, int* len) {
    AnsiCode code = 0;
    const char* p = text;
    while (*p >= '0' && *p <= '9') {
        code = code * 10 + (*p - '0');
        p++;
    }
    *len = (int)(p - text);
    return code;
}

static void set_custom_rgb(GtkAnsiParser* parser, AnsiCode code, int r, int g, int b) {
    char* color = code == ANSI_FG_CUSTOM ? parser->FgColorCustom : parser->BgColorCustom;
    snprintf(color, sizeof(parser->FgColorCustom),
             "#%02x%02x%02x", r % 256, g % 256, b % 256);
}

static void set_custom_color(GtkAnsiParser* parser, AnsiCode code, const char* new_color) {
    char* color = code == ANSI_FG_CUSTOM ? parser->FgColorCustom : parser->BgColorCustom;
    copy_color(color, new_color);
}

static int read_custom_color(
        GtkAnsiParser* parser, AnsiCode code,
        const char* p, int* len) {
    const char* start = p;
    if (*p++ == 'm') {
        *len = (int)(p - start);
        return 1;
    }
    int l;
    int r, g, b;
    int mode = read_ansi_code(p, &l);
    p += l;
    if (mode == 2) {
        // \033[38;2;{r};{g};{b}m
        if (*p++ == 'm') {
            *len = (int)(p - start);
            return 1;
        }
        r = read_ansi_code(p, &l);
        p += l;
        if (*p++ == 'm') {
            *len = (int)(p - start);
            return 1;
        }
        g = read_ansi_code(p, &l);
        p += l;
        if (*p++ == 'm') {
            *len = (int)(p - start);
            return 1;
        }
        b = read_ansi_code(p, &l);
        p += l;
        set_custom_rgb(parser, code, r, g, b);
    } else if (mode == 5) {
        // \033[38;5;{c}m
        if (*p++ == 'm') {
            *len = (int)(p - start);
            return 1;
        }
        int color_code = read_ansi_code(p, &l);
        p += l;
        if (color_code < 16) {
            // color_code is a preset id
            set_custom_color(parser, code, COLORS[color_code]);
        } else if (color_code < 232) {
            // 0 <= r, g, b < 6
            // color_code = 16 + 36 * r + 6 * g + b
            color_code -= 16;
            r = 255 * (color_code / 36) / 5;
            g = 255 * (color_code / 6 % 6) / 5;
            b = 255 * (color_code % 6) / 5;
            set_custom_rgb(parser, code, r, g, b);
        } else {
            // 0 <= g < 24
            // color_code = 232 + g
            r = g = b = 255 * (color_code - 232) / 23;
            set_custom_rgb(parser, code, r, g, b);
        }
    }
    *len = (int)(p - start);
    return 0;
}

const char* gtk_ansi_append(GtkAnsiParser* parser, const char* text) {
    if (!parser || !text)
        return "";

    // Block signals for GtkTextBuffer
    gtk_text_buffer_begin_user_action(parser->Buffer);

    const char *start = text;
    const char *p = start;
    const char *unread = NULL;

    while (*p) {
        if (*p == '\033') {  // Found an ANSI escape sequence
            gtk_ansi_append_text_base(parser, start, (int)(p - start));

            int len = 0;
            CheckAnsiResult res = check_ansi_code(p, &len);
            if (is_unfinished(res))
                unread = p;
            const char *q = p + 2;  // Skip "\033["
            p += len;
            start = p;
            if (is_unsupported(res) || is_unfinished(res))
                continue;

            while (*q) {
                AnsiCode code = read_ansi_code(q, &len);
                q += len;

                if (code == ANSI_FG_CUSTOM || code == ANSI_BG_CUSTOM) {
                    // Read 2;{r};{g};{b} or 5;{id}
                    int ret = read_custom_color(parser, code, q, &len);
                    q += len;
                    if (ret)  // failed to parse the custom color
                        break;
                }
                gtk_ansi_enable_tag_by_ansi(parser, code);

                if (*q++ == 'm')
                    break;
            }
            continue;
        } else if ('\a' <= *p && *p <= '\r' && *p != '\t') {
            gtk_ansi_append_text_base(parser, start, (int)(p - start));
            if (*p == '\b')
                parser->CursorPos -= parser->CursorPos > parser->LineStartPos;
            else if ('\n' <= *p && *p <= '\f')  // \n \v \f
                gtk_ansi_append_line_feed(parser);
            else if (*p == '\r')
                parser->CursorPos = parser->LineStartPos;
            start = p + 1;
        }
        p++;
    }
    gtk_ansi_append_text_base(parser, start, (int)(p - start));

    gtk_text_buffer_end_user_action(parser->Buffer);

    if (unread)
        return unread;
    return p;
}
