# gtk-ansi-parser

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

ANSI string parser for GtkTextBuffer

![gtk_ansi_parser](https://github.com/user-attachments/assets/2ebf26af-c9f8-453f-88f9-e4932328efe6)

## About

gtk-ansi-parser is a small C library to support ANSI strings for `GtkTextBuffer`.
You can use most of the graphics modes (`\033[*m`) and carriage returns to display messages with `GtkTextView`.

## Example

```c
// Allocate memory
GtkTextBuffer* buffer = gtk_text_buffer_new(nullptr);
GtkAnsiParser* parser = gtk_ansi_new(buffer);

// GtkAnsiParser removes the first bytes when the buffer size exceeds 512KiB.
gtk_ansi_set_max_length(parser, 512 * 1024);

// Append ANSI strings.
gtk_ansi_append(parser, "\033[1mBold\033[0m\n");
gtk_ansi_append(parser, "\033[31mRed\033[32mBlue\033[0m\n");
gtk_ansi_append(parser, "\033[38;2;0;255;0mGreen\033[0m\n");
gtk_ansi_append(parser, "Carriage return\rOverwrite\n");

// Free memory
g_object_unref(buffer);
gtk_ansi_free(parser);
```

## Building

### Requirements

- [Meson](https://mesonbuild.com/) (0.54.0 or later) for building
- GTK 3.10+

### Build whole project

```bash
meson setup build
meson compile -C build
meson test -C build
```

### Build library only

```bash
meson setup build -Dtests=false -Ddemo=false
meson compile -C build
```

### Build as subproject

You don't need to clone the git repo if you build your project with meson.  
Save the following text as `subprojects/gtk_ansi_parser.wrap`.  

```ini
[wrap-git]
url = https://github.com/matyalatte/gtk-ansi-parser.git
revision = head
depth = 1

[provide]
gtk_ansi_parser = gtkansi_dep
```

Then, you can use gtk-ansi-parser in your meson project.

```python
gtkansi_dep = dependency('gtk_ansi_parser', fallback : ['gtk_ansi_parser', 'gtkansi_dep'])
executable('your_exe_name', ['your_code.cpp'], dependencies : [gtkansi_dep])
```

```bash
meson setup build -Dgtk_ansi_parser:tests=false -Dgtk_ansi_parser:demo=false
meson compile -C build
```
