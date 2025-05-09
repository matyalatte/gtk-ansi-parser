# Supported Escape Sequences

## Fully Supported Sequences

The following sequences should work as you expected with GtkAnsiParser.

| Code | Reset Code | Effect |
| -- | -- | -- |
| `\b` | | Moves cursor to left |
| `\r` | | Moves cursor to the start of line |
| `\033[0m` | | Reset all styles |
| `\033[1m` | `\033[22m` | Bold |
| `\033[3m` | `\033[23m` | Italic |
| `\033[4m` | `\033[24m` | Underline |
| `\033[9m` | `\033[29m` | Strikethrough |
| `\033[30m` ~ `\033[37m`,<br>`\033[90m` ~ `\033[97m` | `\033[39m` | Foreground preset color |
| `\033[38;2;{r};{g};{b}m` | `\033[39m` | Foreground 24-bit color |
| `\033[38;5;{id}m` | `\033[39m` | Foreground 8-bit color |
| `\033[40m` ~ `\033[47m`,<br>`\033[100m` ~ `\033[107m` | `\033[49m` | Background preset color |
| `\033[48;2;{r};{g};{b}m` | `\033[49m` | Background 24-bit color |
| `\033[48;5;{id}m` | `\033[49m` | Background 8-bit color |

## Partially Supported Sequences

The following sequences might not work as you expected but GtkAnsiParser can detect them.
And some of them require `gtk_ansi_set_default_color()` or `gtk_ansi_set_default_color_with_textview()` to get the default fore and back colors.

| Code | Reset Code | Effect |
| -- | -- | -- |
| `\a` | | Does nothing |
| `\v` | | Line feed |
| `\f` | | Line feed |
| `\033[2m` | `\033[22m` | Reduces intensity by 50% |
| `\033[5m`, `\033[6m` | `\033[25m` | Replaces fore color with default back color once in 1.2 sec |
| `\033[7m` | `\033[27m` | Swaps fore and back colors |
| `\033[8m` | `\033[28m` | Uses back color as fore color |
| Other `\033[*` patterns | | Does nothing |
| `\033` | | Does nothing |
