#include "gtk_ansi.h"

const char* gtk_ansi_get_version() {
    return GTKA_VERSION;
}

int gtk_ansi_get_version_as_int() {
    return GTKA_VERSION_INT;
}

// \e[XXm
typedef int AnsiCode;
enum {
    ANSI_INVALID           = -1,
    ANSI_RESET             = 0,
    ANSI_BOLD              = 1,
    ANSI_FAINT             = 2,
    ANSI_ITALIC            = 3,
    ANSI_UNDERLINE         = 4,
    ANSI_REVERSE           = 7,
    ANSI_CONCEAL           = 8,
    ANSI_STRIKETHROUGH     = 9,
    ANSI_NO_BOLD_FAINT     = 22,
    ANSI_NO_ITALIC         = 23,
    ANSI_NO_UNDERLINE      = 24,
    ANSI_NO_REVERSE        = 27,
    ANSI_NO_CONCEAL        = 28,
    ANSI_NO_STRIKETHROUGH  = 29,
    ANSI_FG_BLACK          = 30,
    /*
    ANSI_FG_RED            = 31,
    ANSI_FG_GREEN          = 32,
    ANSI_FG_YELLOW         = 33,
    ANSI_FG_BLUE           = 34,
    ANSI_FG_MAGENTA        = 35,
    ANSI_FG_CYAN           = 36,
    */
    ANSI_FG_WHITE          = 37,
    ANSI_FG_CUSTOM         = 38,
    ANSI_FG_RESET          = 39,
    ANSI_BG_BLACK          = 40,
    /*
    ANSI_BG_RED            = 41,
    ANSI_BG_GREEN          = 42,
    ANSI_BG_YELLOW         = 43,
    ANSI_BG_BLUE           = 44,
    ANSI_BG_MAGENTA        = 45,
    ANSI_BG_CYAN           = 46,
    */
    ANSI_BG_WHITE          = 47,
    ANSI_BG_CUSTOM         = 48,
    ANSI_BG_RESET          = 49,
    ANSI_FG_BRIGHT_BLACK   = 90,
    /*
    ANSI_FG_BRIGHT_RED     = 91,
    ANSI_FG_BRIGHT_GREEN   = 92,
    ANSI_FG_BRIGHT_YELLOW  = 93,
    ANSI_FG_BRIGHT_BLUE    = 94,
    ANSI_FG_BRIGHT_MAGENTA = 95,
    ANSI_FG_BRIGHT_CYAN    = 96,
    */
    ANSI_FG_BRIGHT_WHITE   = 97,
    ANSI_BG_BRIGHT_BLACK   = 100,
    /*
    ANSI_BG_BRIGHT_RED     = 101,
    ANSI_BG_BRIGHT_GREEN   = 102,
    ANSI_BG_BRIGHT_YELLOW  = 103,
    ANSI_BG_BRIGHT_BLUE    = 104,
    ANSI_BG_BRIGHT_MAGENTA = 105,
    ANSI_BG_BRIGHT_CYAN    = 106,
    */
    ANSI_BG_BRIGHT_WHITE   = 107,
};

typedef int AnsiColor;
enum {
    ANSI_COLOR_DEFAULT = -1,
    ANSI_COLOR_BLACK = 0,
    ANSI_COLOR_RED,
    ANSI_COLOR_GREEN,
    ANSI_COLOR_YELLOW,
    ANSI_COLOR_BLUE,
    ANSI_COLOR_MAGENTA,
    ANSI_COLOR_CYAN,
    ANSI_COLOR_WHITE,
    ANSI_COLOR_BRIGHT_BLACK,
    ANSI_COLOR_BRIGHT_RED,
    ANSI_COLOR_BRIGHT_GREEN,
    ANSI_COLOR_BRIGHT_YELLOW,
    ANSI_COLOR_BRIGHT_BLUE,
    ANSI_COLOR_BRIGHT_MAGENTA,
    ANSI_COLOR_BRIGHT_CYAN,
    ANSI_COLOR_BRIGHT_WHITE,
    ANSI_COLOR_MAX,
};

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

struct GtkAnsiParser {
    GtkTextBuffer* Buffer;
    GtkTextTagTable* TagTable;
    int Length;
    int MaxLength;
    int CursorPos;
    int LineStartPos;

    // Style flags
    unsigned int UseBold : 1;
    unsigned int UseReverse : 1;
    unsigned int UseItalic : 1;
    unsigned int UseUnderline : 1;
    unsigned int UseStrikethrough : 1;
    unsigned int UseConceal : 1;

    // Some color name buffers ("#rrggbb")
    const char* FgColor;     // Current fg color
    const char* BgColor;     // Current bg color
    char FgColorDefault[8];  // Default fg color
    char BgColorDefault[8];  // Default bg color
    char FgColorCustom[8];   // Buffer for \033[38m
    char BgColorCustom[8];   // Buffer for \033[48m
};

GtkAnsiParser* gtk_ansi_new(GtkTextBuffer* buffer) {
    if (!buffer)
        return NULL;

    GtkAnsiParser* parser = g_malloc0(sizeof(GtkAnsiParser));
    if (!parser)
        return NULL;

    parser->Buffer = buffer;
    parser->TagTable = gtk_text_buffer_get_tag_table(buffer);
    gtk_ansi_init(parser);
    return parser;
}

void gtk_ansi_free(GtkAnsiParser* parser) {
    g_free(parser);
}

void gtk_ansi_reset_tags(GtkAnsiParser* parser) {
    if (!parser)
        return;
    parser->UseBold = FALSE;
    parser->UseReverse = FALSE;
    parser->UseItalic = FALSE;
    parser->UseUnderline = FALSE;
    parser->UseStrikethrough = FALSE;
    parser->UseConceal = FALSE;
    parser->FgColor = NULL;
    parser->BgColor = NULL;
}

void gtk_ansi_init(GtkAnsiParser* parser) {
    if (!parser)
        return;
    gtk_text_buffer_set_text(parser->Buffer, "", 0);
    parser->Length = 0;
    parser->CursorPos = 0;
    parser->LineStartPos = 0;
    parser->MaxLength = 1024 * 1024;
    strcpy(parser->FgColorDefault, COLORS[ANSI_COLOR_BLACK]);
    strcpy(parser->BgColorDefault, COLORS[ANSI_COLOR_BRIGHT_WHITE]);
    strcpy(parser->FgColorCustom, "#ffffff");
    strcpy(parser->BgColorCustom, "#ffffff");
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
    char* color = is_bg ? parser->BgColorCustom : parser->FgColorCustom;
    sprintf(color, "#%02x%02x%02x", r, g, b);
}

static int str_to_uint8_unsafe(const char* str) {
    int n = 0;
    for (int i = 0; i < 2; i++) {
        char c = *str++;
        if ('0' <= c && c <= '9')
            n += c - 48;
        else if ('A' <= c && c <= 'F')
            n += c - 55;
        else if ('a' <= c && c <= 'f')
            n += c - 85;
        n *= 16;
    }
    return n;
}

void gtk_ansi_get_default_color(
    GtkAnsiParser* parser, int is_bg, int* r, int* g, int* b) {
    if (!parser)
        return;
    const char* color = is_bg ? parser->BgColorCustom : parser->FgColorCustom;
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

static GtkTextTag* create_tag(GtkTextBuffer* buffer, const char* name, const char* color) {
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
    } else if (c == 's') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "strikethrough", TRUE, NULL);
    } else if (c == 'f') {
        new_tag = gtk_text_buffer_create_tag(
            buffer, name, "foreground", color, NULL);
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
        memcpy(color_tag + 2, color, 7);
        color_tag[9] = 0;
        name = color_tag;
    }
    GtkTextTag* tag = gtk_text_tag_table_lookup(parser->TagTable, name);
    if (!tag)
        tag = create_tag(parser->Buffer, name, color);
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
        gtk_ansi_apply_tag_base(parser, "ul", NULL, start, end);
    if (parser->UseStrikethrough)
        gtk_ansi_apply_tag_base(parser, "st", NULL, start, end);

    const char* fgColor = parser->FgColor;
    const char* bgColor = parser->BgColor;
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
    return 0;
}

// Overite the buffer with some bytes
static int gtk_ansi_overwrite_text(GtkAnsiParser* parser, const char* text, int len) {
    if (parser->Length - parser->CursorPos < len)
        len = parser->Length - parser->CursorPos;

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
    if (len == 0)
        return;
    if (parser->Length + len > parser->MaxLength) {
        int removed_len = parser->Length - parser->MaxLength;
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
    if (code == ANSI_RESET)
        gtk_ansi_reset_tags(parser);
    else if (code == ANSI_BOLD)
        parser->UseBold = TRUE;
    else if (code == ANSI_FAINT || code == ANSI_NO_BOLD_FAINT)
        parser->UseBold = FALSE;
    else if (code == ANSI_ITALIC)
        parser->UseItalic = TRUE;
    else if (code == ANSI_NO_ITALIC)
        parser->UseItalic = FALSE;
    else if (code == ANSI_UNDERLINE)
        parser->UseUnderline = TRUE;
    else if (code == ANSI_NO_UNDERLINE)
        parser->UseUnderline = FALSE;
    else if (code == ANSI_STRIKETHROUGH)
        parser->UseStrikethrough = TRUE;
    else if (code == ANSI_NO_STRIKETHROUGH)
        parser->UseStrikethrough = FALSE;
    else if (code == ANSI_REVERSE)
        parser->UseReverse = TRUE;
    else if (code == ANSI_NO_REVERSE)
        parser->UseReverse = FALSE;
    else if (code == ANSI_CONCEAL)
        parser->UseConceal = TRUE;
    else if (code == ANSI_NO_CONCEAL)
        parser->UseConceal = FALSE;
    else if (ANSI_FG_BLACK <= code && code <= ANSI_FG_WHITE)
        parser->FgColor = COLORS[code - ANSI_FG_BLACK];
    else if (ANSI_BG_BLACK <= code && code <= ANSI_BG_WHITE)
        parser->BgColor = COLORS[code - ANSI_BG_BLACK];
    else if (ANSI_FG_BRIGHT_BLACK <= code && code <= ANSI_FG_BRIGHT_WHITE)
        parser->FgColor = COLORS[code - ANSI_FG_BRIGHT_BLACK + ANSI_COLOR_BRIGHT_BLACK];
    else if (code == ANSI_FG_CUSTOM)
        parser->FgColor = parser->FgColorCustom;
    else if (code == ANSI_FG_RESET)
        parser->FgColor = NULL;
    else if (ANSI_BG_BRIGHT_BLACK <= code && code <= ANSI_BG_BRIGHT_WHITE)
        parser->BgColor = COLORS[code - ANSI_BG_BRIGHT_BLACK + ANSI_COLOR_BRIGHT_BLACK];
    else if (code == ANSI_BG_CUSTOM)
        parser->BgColor = parser->BgColorCustom;
    else if (code == ANSI_BG_RESET)
        parser->BgColor = NULL;
    else
        return ANSI_INVALID;
    return code;
}

static int check_ansi_code(const char* text, int* len) {
    const char* p = text;
    int unsupported = 0;
    while (*p) {
        if (*p == '"') {
            // Skip a string
            unsupported = 1;
            while (*p && *p != '"')
                p++;
            if (!*p)
                break;
            p++;
            continue;
        }
        if (*p < '0' || '?' < *p) {
            // End of sequence
            if (*p != 'm')
                unsupported = 1;
            p++;
            break;
        } else if (*p == ':' || '<' <= *p) {
            unsupported = 1;
        }
        p++;
    }
    *len = (int)(p - text);
    return unsupported;
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
    sprintf(color, "#%02x%02x%02x", r, g, b);
}

void gtk_ansi_append(GtkAnsiParser* parser, const char* text) {
    if (!parser || !text)
        return;

    const char *start = text;
    const char *p = start;

    while (*p) {
        if (*p == '\033' && *(p + 1) == '[') {  // Found an ANSI escape sequence
            // NOLINTNEXTLINE(readability/casting)
            gtk_ansi_append_text_base(parser, start, (int)(p - start));

            const char *q = p + 2;  // Skip "\033["

            int len = 0;
            int unsupported = check_ansi_code(q, &len);
            p = q + len;
            start = p;
            if (unsupported)
                continue;

            while (*q) {
                AnsiCode code = read_ansi_code(q, &len);
                q += len;

                if (code == ANSI_FG_CUSTOM || code == ANSI_BG_CUSTOM) {
                    if (*q++ == 'm')
                        break;
                    int r, g, b;
                    int mode = read_ansi_code(q, &len);
                    q += len;
                    if (mode == 2) {
                        // \033[38;2;{r};{g};{b}m
                        if (*q++ == 'm')
                            break;
                        r = read_ansi_code(q, &len);
                        q += len;
                        if (*q++ == 'm')
                            break;
                        g = read_ansi_code(q, &len);
                        q += len;
                        if (*q++ == 'm')
                            break;
                        b = read_ansi_code(q, &len);
                        q += len;
                        set_custom_rgb(parser, code, r, g, b);
                    } else if (mode == 5) {
                        // \033[38;5;{c}m
                        if (*q++ == 'm')
                            break;
                        int color_code = read_ansi_code(q, &len);
                        q += len;
                        if (color_code < 16) {
                            // color_code is a preset id
                            if (code == ANSI_FG_CUSTOM)
                                strcpy(parser->FgColorCustom, COLORS[color_code]);
                            else
                                strcpy(parser->BgColorCustom, COLORS[color_code]);
                        } else if (color_code < 232) {
                            // 0 <= r, g, b < 6
                            // color_code = 16 + 36 * r + 6 * g + b
                            color_code -= 16;
                            r = (color_code / 36) * 256 / 6;
                            g = ((color_code % 36) / 6) * 256 / 6;
                            b = (color_code % 6) * 256 / 6;
                            set_custom_rgb(parser, code, r, g, b);
                        } else {
                            // 0 <= g < 24
                            // color_code = 232 + 24 * g
                            r = g = b = 256 * (color_code - 232) / 24;
                            set_custom_rgb(parser, code, r, g, b);
                        }
                    }
                }
                gtk_ansi_enable_tag_by_ansi(parser, code);

                if (*q++ == 'm')
                    break;
            }
            continue;
        } else if (*p == '\r' || *p == '\n' || *p == '\a') {
            // NOLINTNEXTLINE(readability/casting)
            gtk_ansi_append_text_base(parser, start, (int)(p - start));
            if (*p == '\r')
                parser->CursorPos = parser->LineStartPos;
            else if (*p == '\n')
                gtk_ansi_append_line_feed(parser);
            start = p + 1;
        }
        p++;
    }
    // NOLINTNEXTLINE(readability/casting)
    gtk_ansi_append_text_base(parser, start, (int)(p - start));
}
