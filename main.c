/******************************************************************************
 * Project Name : Mini Shell
 * Author       : YDNS PAVAN
 * Date         : 17-07-2026
 * Batch        : 25036A_105
 *
 * Description:
 * Mini Shell is a simple Linux command-line interpreter developed in C.
 * It supports built-in and external commands, process creation using
 * fork(), command execution using execvp(), piping, signal handling,
 * and basic job control (jobs, fg, bg).
 ******************************************************************************/

#include <stdio.h>
#include "main.h"

char *external_commands[155];
char buf[50];
char prompt_string[100]="minishell";
int status;
int last_status;
char input_string[100];
struct Slist *head;
char fg=0;

int main()
{
    system("clear");
    extract_external_commands();
    scan_input(prompt_string,input_string);
    return 0;
}