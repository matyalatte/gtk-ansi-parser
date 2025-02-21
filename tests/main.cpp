#include <gtest/gtest.h>
#include "gtk_ansi.h"

struct AnsiCase {
    const char *ansi;
    const char *plain;
};

// Test Fixture for GtkAnsiParser
class AnsiTest : public ::testing::TestWithParam<AnsiCase> {
protected:
    GtkAnsiParser* parser;
    GtkTextBuffer* text_buffer;

    void SetUp() override {
        gtk_init(nullptr, nullptr);  // Initialize GTK

        // Create a GtkTextBuffer to pass to gtk_ansi_new
        text_buffer = gtk_text_buffer_new(nullptr);
        parser = gtk_ansi_new(text_buffer);
    }

    void TearDown() override {
        if (text_buffer) g_object_unref(text_buffer);
        gtk_ansi_free(parser);
    }
};

TEST_F(AnsiTest, GetTextBufferNotNull) {
    GtkTextBuffer* text_buffer = gtk_ansi_get_text_buffer(parser);
    ASSERT_NE(text_buffer, nullptr);
}

const AnsiCase ansi_cases_plain[] = {
    { "Hello, World", "Hello, World" },
    { "Hello, World\nWhat", "Hello, World\nWhat" },
    { "Hello, World\rWhat", "Whato, World" },
    { "Hello, World\rWhat\n", "Whato, World\n" },
    { "Hello, World\aWhat", "Hello, WorldWhat" },
    { NULL, "" },
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationPlain,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_plain));

const AnsiCase ansi_cases_style[] = {
    { "\033[1mBold", "Bold" },
    { "\033[31mRed", "Red" },
    { "\033[42mBlueBack", "BlueBack" },
    { "\033[38;2;0;128;255mCustomRGB", "CustomRGB" },
    { "\033[48;5;34mCustomRGB", "CustomRGB" },
    { NULL, "" },
};

INSTANTIATE_TEST_SUITE_P(AnsiTestInstantiationStyle,
    AnsiTest,
    ::testing::ValuesIn(ansi_cases_style));

TEST_P(AnsiTest, AppendText) {
    const AnsiCase test_case = GetParam();
    gtk_ansi_append(parser, test_case.ansi);

    char* text = gtk_ansi_get_text(parser);

    ASSERT_STREQ(text, test_case.plain);
    gtk_ansi_free_text(text);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
