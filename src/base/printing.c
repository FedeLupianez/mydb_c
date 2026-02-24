#include "printing.h"

void print_error(char* message)
{
    printf("\x1b[38;5;160m%s\e[0m\n", message);
}

void print_ok(char* message)
{
    printf("\x1b[38;5;35m%s\e[0m\n", message);
}

void print_info(char* message)
{
    printf("\x1b[38;5;243m%s\e[0m\n", message);
}

void print_warning(char* message)
{
    printf("\x1b[38;5;178m%s\e[0m\n", message);
}

void print_debug(char* message)
{
    printf("\x1b[38;5;97m%s\e[0m\n", message);
}

void print_trace(char* message)
{
    printf("\x1b[38;5;75m%s\e[0m\n", message);
}
