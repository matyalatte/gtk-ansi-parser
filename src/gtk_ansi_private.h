#ifndef __GTK_ANSI_PARSER_SRC_GTK_ANSI_PRIVATE_H__
#define __GTK_ANSI_PARSER_SRC_GTK_ANSI_PRIVATE_H__

#include "gtk_ansi.h"

#ifdef __cplusplus
extern "C" {
#endif

// \e[XXm
typedef int AnsiCode;
enum {
    ANSI_INVALID           = -1,
    ANSI_RESET             = 0,
    ANSI_BOLD              = 1,
    ANSI_FAINT             = 2,
    ANSI_ITALIC            = 3,
    ANSI_UNDERLINE         = 4,
    ANSI_BLINK             = 5,
    ANSI_RAPID_BLINK       = 6,
    ANSI_REVERSE           = 7,
    ANSI_CONCEAL           = 8,
    ANSI_STRIKETHROUGH     = 9,
    ANSI_NO_BOLD_FAINT     = 22,
    ANSI_NO_ITALIC         = 23,
    ANSI_NO_UNDERLINE      = 24,
    ANSI_NO_BLINK          = 25,
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

struct GtkAnsiParser {
    GtkTextBuffer* Buffer;
    GtkTextTagTable* TagTable;
    int Length;
    int MaxLength;
    int CursorPos;
    int LineStartPos;

    // Style flags
    unsigned int UseBold : 1;
    unsigned int UseFaint : 1;
    unsigned int UseBlink : 1;
    unsigned int UseReverse : 1;
    unsigned int UseItalic : 1;
    unsigned int UseUnderline : 1;
    unsigned int UseStrikethrough : 1;
    unsigned int UseConceal : 1;

    // Parameters for blinking
    guint BlinkFuncId;
    GtkTextTag* BlinkTag;
    unsigned int BlinkVisible : 1;

    // Some color name buffers ("#rrggbb")
    const char* FgColor;     // Current fg color
    const char* BgColor;     // Current bg color
    char FgColorDefault[8];  // Default fg color
    char BgColorDefault[8];  // Default bg color
    char FgColorCustom[8];   // Buffer for \033[38m
    char BgColorCustom[8];   // Buffer for \033[48m
};

#ifdef __cplusplus
}
#endif

#endif  // __GTK_ANSI_PARSER_SRC_GTK_ANSI_PRIVATE_H__
