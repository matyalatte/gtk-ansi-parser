#pragma once

#include <gtest/gtest.h>
#include "gtk_ansi.h"
#include "gtk_ansi_private.h"

struct AnsiCase {
    const char *ansi;
    const char *plain;
    int tag_size;
    const char* tag_name;
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

    void CheckText(const char* plain) {
        char* text = gtk_ansi_get_text(parser);
        ASSERT_STREQ(text, plain);
        gtk_ansi_free_text(text);
        ASSERT_EQ(parser->Length, strlen(plain));
    }

    void CheckTagSize(int size) {
        int tag_size = gtk_text_tag_table_get_size(parser->TagTable);
        ASSERT_EQ(tag_size, size);
    }

    void CheckTag(const char* name) {
        if (!name) return;
        GtkTextTag* tag = gtk_text_tag_table_lookup(parser->TagTable, name);
        ASSERT_NE(tag, nullptr);
    }
};
