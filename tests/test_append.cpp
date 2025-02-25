#include "test_util.h"

TEST(AnsiNullTest, AppendToNull) {
    gtk_ansi_append(NULL, NULL);
}

const AnsiCase ansi_cases_plain[] = {
    { NULL, "", 0, nullptr },
    { "Hello, World", "Hello, World", 0, nullptr },
    { "Hello, World\nWhat", "Hello, World\nWhat", 0, nullptr },
    { "Hello, World\rWhat", "Whato, World", 0, nullptr },
    { "Hello, World\rWhat\n", "Whato, World\n", 0, nullptr },
    { "Hello, World\rWhat is this?\n", "What is this?\n", 0, nullptr },
    { "Hello, World\aWhat", "Hello, WorldWhat", 0, nullptr },
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationPlain,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_plain));

const AnsiCase ansi_cases_style[] = {
    // Bold
    { "\033[1mBold", "Bold", 1, "bo" },
    { "\033[1;0mBold", "Bold", 0, nullptr },
    { "\033[1;22mBold", "Bold", 0, nullptr },
    { "\033[1;23mBold", "Bold", 1, "bo" },

    // Faint
    { "\033[2mFaint", "Faint", 1, "fg#000000" },
    { "\033[38;2;128;64;32;2mFaint", "Faint", 1, "fg#402010" },
    { "\033[2;0mFaint", "Faint", 0, nullptr },
    { "\033[2;22mFaint", "Faint", 0, nullptr },
    { "\033[2;23mFaint", "Faint", 1, "fg#000000" },
    { "\033[1;2;22mFaint", "Faint", 0, nullptr },
    { "\033[1;2mBoldFaint", "BoldFaint", 2, "bo" },

    // Italic
    { "\033[3mItalic", "Italic", 1, "it" },
    { "\033[3;22mItalic", "Italic", 1, "it" },
    { "\033[3;23mItalic", "Italic", 0, nullptr },
    { "\033[3;0mItalic", "Italic", 0, nullptr },

    // Underline
    { "\033[4mUnderline", "Underline", 1, "un" },
    { "\033[4;23mUnderline", "Underline", 1, "un" },
    { "\033[4;24mUnderline", "Underline", 0, nullptr },
    { "\033[4;0mUnderline", "Underline", 0, nullptr },

    // Blink
    { "\033[5mBlink", "Blink", 1, "bl" },
    { "\033[6mRapidBlink", "RapidBlink", 1, "bl" },
    { "\033[5;25mBlink", "Blink", 0, nullptr },
    { "\033[5;0mBlink", "Blink", 0, nullptr },
    { "\033[6;25mRapidBlink", "RapidBlink", 0, nullptr },

    // Reverse
    { "\033[7mReverse", "Reverse", 2, "fg#ffffff" },
    { "\033[7;28mReverse", "Reverse", 2, "fg#ffffff" },
    { "\033[7;27mReverse", "Reverse", 0, nullptr },
    { "\033[7;0mReverse", "Reverse", 0, nullptr },

    // Conseal
    { "\033[8mConseal", "Conseal", 1, "fg#ffffff" },
    { "\033[8;27mConseal", "Conseal", 1, "fg#ffffff" },
    { "\033[8;28mConseal", "Conseal", 0, nullptr },
    { "\033[8;0mConseal", "Conseal", 0, nullptr },

    // Strikethrough
    { "\033[9mStrikethrough", "Strikethrough", 1, "st" },
    { "\033[9;22mStrikethrough", "Strikethrough", 1, "st" },
    { "\033[9;29mStrikethrough", "Strikethrough", 0, nullptr },
    { "\033[9;0mStrikethrough", "Strikethrough", 0, nullptr },
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationStyle,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_style));

const AnsiCase ansi_cases_color[] = {
    // FG clors
    { "\033[30mBlack", "Black", 1, "fg#000000" },
    { "\033[31mRed", "Red", 1, "fg#cd0000" },
    { "\033[32mGreen", "Green", 1, "fg#00cd00" },
    { "\033[33mYellow", "Yellow", 1, "fg#cdcd00" },
    { "\033[34mBlue", "Blue", 1, "fg#0000ee" },
    { "\033[35mMagenta", "Magenta", 1, "fg#cd00cd" },
    { "\033[36mCyan", "Cyan", 1, "fg#00cdcd" },
    { "\033[37mWhite", "White", 1, "fg#e5e5e5" },
    { "\033[37;49mWhite", "White", 1, "fg#e5e5e5" },
    { "\033[37;39mWhite", "White", 0, nullptr },
    { "\033[37;0mWhite", "White", 0, nullptr },

    // FG bright clors
    { "\033[90mBlack", "Black", 1, "fg#7f7f7f" },
    { "\033[91mRed", "Red", 1, "fg#ff0000" },
    { "\033[92mGreen", "Green", 1, "fg#00ff00" },
    { "\033[93mYellow", "Yellow", 1, "fg#ffff00" },
    { "\033[94mBlue", "Blue", 1, "fg#5c5cff" },
    { "\033[95mMagenta", "Magenta", 1, "fg#ff00ff" },
    { "\033[96mCyan", "Cyan", 1, "fg#00ffff" },
    { "\033[97mWhite", "White", 1, "fg#ffffff" },
    { "\033[97;49mWhite", "White", 1, "fg#ffffff" },
    { "\033[97;39mWhite", "White", 0, nullptr },
    { "\033[97;0mWhite", "White", 0, nullptr },

    // BG clors
    { "\033[40mBlack", "Black", 1, "bg#000000" },
    { "\033[41mRed", "Red", 1, "bg#cd0000" },
    { "\033[42mGreen", "Green", 1, "bg#00cd00" },
    { "\033[43mYellow", "Yellow", 1, "bg#cdcd00" },
    { "\033[44mBlue", "Blue", 1, "bg#0000ee" },
    { "\033[45mMagenta", "Magenta", 1, "bg#cd00cd" },
    { "\033[46mCyan", "Cyan", 1, "bg#00cdcd" },
    { "\033[47mWhite", "White", 1, "bg#e5e5e5" },
    { "\033[47;39mWhite", "White", 1, "bg#e5e5e5" },
    { "\033[47;49mWhite", "White", 0, nullptr },
    { "\033[47;0mWhite", "White", 0, nullptr },

    // BG bright clors
    { "\033[100mBlack", "Black", 1, "bg#7f7f7f" },
    { "\033[101mRed", "Red", 1, "bg#ff0000" },
    { "\033[102mGreen", "Green", 1, "bg#00ff00" },
    { "\033[103mYellow", "Yellow", 1, "bg#ffff00" },
    { "\033[104mBlue", "Blue", 1, "bg#5c5cff" },
    { "\033[105mMagenta", "Magenta", 1, "bg#ff00ff" },
    { "\033[106mCyan", "Cyan", 1, "bg#00ffff" },
    { "\033[107mWhite", "White", 1, "bg#ffffff" },
    { "\033[107;39mWhite", "White", 1, "bg#ffffff" },
    { "\033[107;49mWhite", "White", 0, nullptr },
    { "\033[107;0mWhite", "White", 0, nullptr },
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationColor,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_color));

const AnsiCase ansi_cases_custom_color[] = {
    // custom rgb
    { "\033[38;2;0;128;255mCustomRGB", "CustomRGB", 1, "fg#0080ff" },
    { "\033[38;2;0;128;0mCustomRGB", "CustomRGB", 1, "fg#008000" },
    { "\033[38;2;0;128;257mCustomRGB", "CustomRGB", 1, "fg#008001" },

    // preset colors
    { "\033[48;5;0mCustomRGB", "CustomRGB", 1, "bg#000000" },
    { "\033[48;5;1mCustomRGB", "CustomRGB", 1, "bg#cd0000" },
    { "\033[48;5;15mCustomRGB", "CustomRGB", 1, "bg#ffffff" },

    // 6 × 6 × 6 cube colors
    { "\033[48;5;16mCustomRGB", "CustomRGB", 1, "bg#000000" },
    { "\033[48;5;34mCustomRGB", "CustomRGB", 1, "bg#009900" },
    { "\033[48;5;231mCustomRGB", "CustomRGB", 1, "bg#ffffff" },

    // gray scale colors
    { "\033[48;5;232mCustomRGB", "CustomRGB", 1, "bg#000000" },
    { "\033[48;5;234mCustomRGB", "CustomRGB", 1, "bg#161616" },
    { "\033[48;5;255mCustomRGB", "CustomRGB", 1, "bg#ffffff" },

    // reset
    { "\033[38;2;0;128;255;49mCustomRGB", "CustomRGB", 1, "fg#0080ff" },
    { "\033[38;2;0;128;255;39mCustomRGB", "CustomRGB", 0, nullptr },
    { "\033[38;2;0;128;255;0mCustomRGB", "CustomRGB", 0, nullptr },
    { "\033[48;5;34;39mCustomRGB", "CustomRGB", 1, "bg#009900" },
    { "\033[48;5;34;49mCustomRGB", "CustomRGB", 0, nullptr },
    { "\033[48;5;34;0mCustomRGB", "CustomRGB", 0, nullptr },

    // etc
    { "\033[38;2;0;128;mCustomRGB", "CustomRGB", 1, "fg#008000" },
    { "\033[38;2;;;mCustomRGB", "CustomRGB", 1, "fg#000000" },
    { "\033[38;2;mCustomRGB", "CustomRGB", 0, nullptr },
    { "\033[38;;mCustomRGB", "CustomRGB", 0, nullptr },
    { "\033[38mCustomRGB", "CustomRGB", 0, nullptr },
    { "\033[38;5;3mCustomRGB", "CustomRGB", 1, "fg#cdcd00" },
    { "\033[38;5;mCustomRGB", "CustomRGB", 1, "fg#000000" },
    { "\033[38;;mCustomRGB", "CustomRGB", 0, nullptr },
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationCustomColor,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_custom_color));

const AnsiCase ansi_cases_unsupported[] = {
    { "\033[8ACursorUp", "CursorUp", 0, nullptr },
    { "\033[2BCursorDown", "CursorDown", 0, nullptr },
    { "\033[33CCursorForward", "CursorForward", 0, nullptr },
    { "\033[DCursorBack", "CursorBack", 0, nullptr },
    { "\033[4ECursorNextLine", "CursorNextLine", 0, nullptr },
    { "\033[42FCursorPrevLine", "CursorPrevLine", 0, nullptr },
    { "\033[8GCursorHorizAbs", "CursorHorizAbs", 0, nullptr },
    { "\033[32;4HCursorPos", "CursorPos", 0, nullptr },
    { "\033[32;4fCursorPos", "CursorPos", 0, nullptr },
    { "\033[?25hShowsCursor", "ShowsCursor", 0, nullptr },
    { "\0337SaveCursor", "7SaveCursor", 0, nullptr },
    { "\0338RestoreCursor", "8RestoreCursor", 0, nullptr },
    { "\033 F", " F", 0, nullptr },
    { "\033 G", " G", 0, nullptr },
    { "\033#3", "#3", 0, nullptr },
    { "\033[1~Home", "Home", 0, nullptr },
    { "\033[=19hSetMode", "SetMode", 0, nullptr },
    { "\033[M", "", 0, nullptr },
    { "\033[0;68;\"DIR\";13pKey", "Key", 0, nullptr},
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationUnsupported,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_unsupported));

const AnsiCase ansi_cases_control_chars[] = {
    { "\a", "", 0, nullptr },
    { "aaa\tbbb", "aaa\tbbb", 0, nullptr },
    { "aaa\rc\nbbb", "caa\nbbb", 0, nullptr },
    { "aaa\rc\vbbb", "caa\nbbb", 0, nullptr },
    { "aaa\rc\fbbb", "caa\nbbb", 0, nullptr },
    { "abc\rd", "dbc", 0, nullptr },
    { "\ba", "a", 0, nullptr },
    { "a\bb", "b", 0, nullptr },
    { "abc\bde", "abde", 0, nullptr },
    { "a\b\nb", "a\nb", 0, nullptr },
    { "a\n\bb", "a\nb", 0, nullptr },
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationControlChars,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_control_chars));

TEST_P(AnsiTest, AppendText) {
    const AnsiCase test_case = GetParam();
    const char* rest = gtk_ansi_append(parser, test_case.ansi);

    ASSERT_STREQ(rest, "");
    CheckText(test_case.plain);

    CheckTagSize(test_case.tag_size);
    CheckTag(test_case.tag_name);
}

TEST_F(AnsiTest, AppendTextIncomplete) {
    const char* rest;
    rest = gtk_ansi_append(parser, "\033[0;68;\"DIR\";13");
    EXPECT_STREQ(rest, "\033[0;68;\"DIR\";13");
    CheckText("");
    rest = gtk_ansi_append(parser, "\033[0;68;\"DIR");
    EXPECT_STREQ(rest, "\033[0;68;\"DIR");
    CheckText("");
    rest = gtk_ansi_append(parser, "\033[0;32");
    EXPECT_STREQ(rest, "\033[0;32");
    CheckText("");
    CheckTagSize(0);
    rest = gtk_ansi_append(parser, "\033[;");
    ASSERT_STREQ(rest, "\033[;");
    CheckText("");
    CheckTagSize(0);
    rest = gtk_ansi_append(parser, "\033[");
    ASSERT_STREQ(rest, "\033[");
    CheckText("");
    CheckTagSize(0);
    rest = gtk_ansi_append(parser, "\033");
    ASSERT_STREQ(rest, "\033");
    CheckText("");
    CheckTagSize(0);
    rest = gtk_ansi_append(parser, "aaa\033[");
    ASSERT_STREQ(rest, "\033[");
    CheckText("aaa");
    CheckTagSize(0);
}

TEST_F(AnsiTest, AppendTextTagDup) {
    gtk_ansi_append(parser, "\033[1mBold");
    CheckTagSize(1);
    CheckTag("bo");
    gtk_ansi_append(parser, "\033[1mBold");
    CheckTagSize(1);
    gtk_ansi_append(parser, "\033[31mRed");
    CheckTagSize(2);
    CheckTag("fg#cd0000");
    gtk_ansi_append(parser, "\033[31mRed");
    CheckTagSize(2);
}

TEST_F(AnsiTest, AppendTextReset) {
    CheckText("");
    CheckTagSize(0);
    ASSERT_EQ(parser->UseBold, 0);
    ASSERT_EQ(parser->FgColor, nullptr);
    gtk_ansi_append(parser, "\033[1mBold");
    CheckText("Bold");
    CheckTagSize(1);
    ASSERT_EQ(parser->UseBold, 1);
    ASSERT_EQ(parser->FgColor, nullptr);
    gtk_ansi_append(parser, "\033[31mRed");
    CheckText("BoldRed");
    CheckTagSize(2);
    ASSERT_EQ(parser->UseBold, 1);
    ASSERT_NE(parser->FgColor, nullptr);
    gtk_ansi_append(parser, "\033[0mPlain");
    CheckText("BoldRedPlain");
    CheckTagSize(2);
    ASSERT_EQ(parser->UseBold, 0);
    ASSERT_EQ(parser->FgColor, nullptr);
}

TEST_F(AnsiTest, AppendLF) {
    gtk_ansi_append_line_feed(parser);
    CheckText("\n");
}

TEST_F(AnsiTest, AppendLFWithCR) {
    gtk_ansi_append(parser, "aaaa\rb");
    gtk_ansi_append_line_feed(parser);
    gtk_ansi_append(parser, "cc");
    gtk_ansi_append_line_feed(parser);
    CheckText("baaa\ncc\n");
}

TEST(AnsiNullTest, AppendLFNull) {
    gtk_ansi_append_line_feed(NULL);
}

TEST_F(AnsiTest, AppendLongText) {
    gtk_ansi_set_max_length(parser, 32);
    gtk_ansi_append(parser, "test");
    gtk_ansi_append(parser, "\033[1m01234567\033[0m89abcdef0123456\033[3;2;2m789abcdef0123456789");
    CheckText("abcdef0123456789abcdef0123456789");
}

TEST_F(AnsiTest, AppendToLongText) {
    gtk_ansi_set_max_length(parser, 32);
    gtk_ansi_append(parser, "\033[1m01234567\033[0m89abcdef0123456\033[3;2;2m789abcdef");
    gtk_ansi_append(parser, "0123456789");
    CheckText("abcdef0123456789abcdef0123456789");
}

TEST_F(AnsiTest, AppendLFToLongText) {
    gtk_ansi_set_max_length(parser, 32);
    gtk_ansi_append(parser, "\033[1m01234567\033[0m89abcdef0123456\033[3;2;2m789abcdef");
    gtk_ansi_append_line_feed(parser);
    CheckText("123456789abcdef0123456789abcdef\n");
}
