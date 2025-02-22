# Supported Escape Sequences

## Fully Supported Sequences

The following sequences should work as you expected with GtkAnsiParser.

| Code | Reset Code | Effect |
| -- | -- | -- |
| `\r` | | Carriage return |
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
Also, some of them requires `gtk_ansi_set_default_color()` to get the default fore and back colors.

| Code | Reset Code | Effect |
| -- | -- | -- |
| `\033[2m` | `\033[22m` | Disables bold fonts |
| `\033[7m` | `\033[27m` | Swaps fore and back colors |
| `\033[8m` | `\033[28m` | Uses back color as fore color |
| Other `\033[*` patterns | | Ignores itself |
| `\a` | | Ignores itself |
| `\033` | | Ignores itself |
