#include "test_util.h"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST_F(AnsiTest, GetTextBuffer) {
    GtkTextBuffer* buf = gtk_ansi_get_text_buffer(parser);
    ASSERT_EQ(buf, text_buffer);
}

TEST(AnsiNullTest, GetTextBufferNull) {
    GtkTextBuffer* buf = gtk_ansi_get_text_buffer(NULL);
    ASSERT_EQ(buf, nullptr);
}

TEST(AnsiVerTest, GetVersion) {
    ASSERT_STREQ(gtk_ansi_get_version(), GTKA_VERSION);
}

TEST(AnsiVerTest, GetVersionAsInt) {
    ASSERT_EQ(gtk_ansi_get_version_as_int(), GTKA_VERSION_INT);
}

TEST_F(AnsiTest, Init) {
    ASSERT_EQ(parser->UseUnderline, 0);
    ASSERT_EQ(parser->FgColor, nullptr);
    CheckText("");
    gtk_ansi_append(parser, "\033[4mUnderline");
    gtk_ansi_append(parser, "\033[31mRed");
    ASSERT_EQ(parser->UseUnderline, 1);
    ASSERT_NE(parser->FgColor, nullptr);
    CheckText("UnderlineRed");
    CheckTagSize(2);
    gtk_ansi_init(parser);
    ASSERT_EQ(parser->UseUnderline, 0);
    ASSERT_EQ(parser->FgColor, nullptr);
    CheckText("");
}

TEST(AnsiNullTest, InitNull) {
    gtk_ansi_init(NULL);
}

TEST_F(AnsiTest, MaxLength) {
    ASSERT_EQ(gtk_ansi_get_max_length(parser), 1024 * 1024);
    gtk_ansi_set_max_length(parser, 256);
    ASSERT_EQ(gtk_ansi_get_max_length(parser), 256);
}

TEST(AnsiNullTest, MaxLengthNull) {
    gtk_ansi_set_max_length(NULL, 0);
    int len = gtk_ansi_get_max_length(NULL);
    ASSERT_EQ(len, 0);
}

TEST_F(AnsiTest, DefaultFgColor) {
    int r, g, b;
    gtk_ansi_get_default_color(parser, 0, &r, &g, &b);
    ASSERT_EQ(r, 0);
    ASSERT_EQ(g, 0);
    ASSERT_EQ(b, 0);
    gtk_ansi_get_default_color(parser, 1, &r, &g, &b);
    ASSERT_EQ(r, 255);
    ASSERT_EQ(g, 255);
    ASSERT_EQ(b, 255);
    gtk_ansi_set_default_color(parser, 0, 34, 280, 111);
    gtk_ansi_get_default_color(parser, 0, &r, &g, &b);
    ASSERT_EQ(r, 34);
    ASSERT_EQ(g, 24);
    ASSERT_EQ(b, 111);
    gtk_ansi_get_default_color(parser, 1, &r, &g, &b);
    ASSERT_EQ(r, 255);
    ASSERT_EQ(g, 255);
    ASSERT_EQ(b, 255);
}

TEST_F(AnsiTest, DefaultBgColor) {
    int r, g, b;
    gtk_ansi_get_default_color(parser, 0, &r, &g, &b);
    ASSERT_EQ(r, 0);
    ASSERT_EQ(g, 0);
    ASSERT_EQ(b, 0);
    gtk_ansi_get_default_color(parser, 1, &r, &g, &b);
    ASSERT_EQ(r, 255);
    ASSERT_EQ(g, 255);
    ASSERT_EQ(b, 255);
    gtk_ansi_set_default_color(parser, 1, 34, 280, 111);
    gtk_ansi_get_default_color(parser, 0, &r, &g, &b);
    ASSERT_EQ(r, 0);
    ASSERT_EQ(g, 0);
    ASSERT_EQ(b, 0);
    gtk_ansi_get_default_color(parser, 1, &r, &g, &b);
    ASSERT_EQ(r, 34);
    ASSERT_EQ(g, 24);
    ASSERT_EQ(b, 111);
    gtk_ansi_append(parser, "\033[7mReversed");
    CheckTagSize(2);
    CheckTag("fg#22186f");
    CheckTag("bg#000000");
}

TEST_F(AnsiTest, DefaultColorNullLayer) {
    int c;
    gtk_ansi_get_default_color(parser, 1, &c, NULL, NULL);
    ASSERT_EQ(c, 255);
    gtk_ansi_get_default_color(parser, 0, NULL, &c, NULL);
    ASSERT_EQ(c, 0);
    gtk_ansi_get_default_color(parser, 1, NULL, NULL, &c);
    ASSERT_EQ(c, 255);
}

TEST(AnsiNullTest, DefaultColorNull) {
    gtk_ansi_set_default_color(NULL, 0, 0, 0, 0);
    int r, g, b;
    gtk_ansi_get_default_color(NULL, 0, &r, &g, &b);
}

TEST_F(AnsiTest, ResetTags) {
    ASSERT_EQ(parser->UseUnderline, 0);
    ASSERT_EQ(parser->FgColor, nullptr);
    CheckText("");
    gtk_ansi_append(parser, "\033[4mUnderline");
    gtk_ansi_append(parser, "\033[31mRed");
    ASSERT_EQ(parser->UseUnderline, 1);
    ASSERT_NE(parser->FgColor, nullptr);
    CheckText("UnderlineRed");
    CheckTagSize(2);
    gtk_ansi_reset_tags(parser);
    ASSERT_EQ(parser->UseUnderline, 0);
    ASSERT_EQ(parser->FgColor, nullptr);
    CheckText("UnderlineRed");
}

TEST(AnsiNullTest, ResetTagsNull) {
    gtk_ansi_reset_tags(NULL);
}

TEST_F(AnsiTest, RemvoeBytes) {
    gtk_ansi_append(parser, "Hello, World!");
    int removed = gtk_ansi_remove_first_bytes(parser, 4);
    ASSERT_EQ(removed, 4);
    CheckText("o, World!");
    removed = gtk_ansi_remove_first_bytes(parser, 100);
    ASSERT_EQ(removed, 9);
    CheckText("");
}

TEST_F(AnsiTest, RemvoeBytesWthCR) {
    gtk_ansi_append(parser, "Hello, World!\rfoobar");
    gtk_ansi_remove_first_bytes(parser, 2);
    CheckText("obar World!");
    gtk_ansi_append(parser, "???\n");
    CheckText("obar???rld!\n");
    gtk_ansi_append(parser, "foo");
    CheckText("obar???rld!\nfoo");
}

TEST(AnsiNullTest, RemoveBytesNull) {
    int removed = gtk_ansi_remove_first_bytes(NULL, 0);
    ASSERT_EQ(removed, 0);
}
