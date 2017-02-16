//
// Created by glavak on 11.02.17.
//

#include <sys/types.h>
#include <stdio.h>
#include <wait.h>
#include <memory.h>
#include <stdlib.h>

#include <unistd.h>

#include "shell.h"
#include "execute.h"

// #define DEBUG

char * infile, * outfile, * appfile;
struct command cmds[MAXCMDS];
char bkgrnd;
int signal_to_stop_process; // -1 - not stop process, signo otherwise

void write_prompt()
{
    char shell_prompt[64];

    getcwd(shell_prompt, sizeof(shell_prompt) - 3);
    strcat(shell_prompt, "$ ");

    write(STDOUT_FILENO, shell_prompt, strlen(shell_prompt));
}

void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGQUIT)
    {
        signal_to_stop_process = signo;
    }
}

int main(int argc, char * argv[])
{
    int i;
    char line[1024];  /*  allow large command lines  */

    if (signal(SIGINT, sig_handler) == SIG_ERR ||
        signal(SIGQUIT, sig_handler) == SIG_ERR)
    {
        printf("Can't catch signal");
    }

    while (1)
    {
        write_prompt();
        size_t read_chars = promptline(line, sizeof(line));
        if (read_chars == 0)
        {
            // Ctrl-D pressed
            break;
        }

        int commands_count = parseline(line);
        if (commands_count <= 0)
        {
            // Empty line entered by user
            continue;
        }

#ifdef DEBUG
        {
            int i, j;
                for (i = 0; i < commands_count; i++) {
                for (j = 0; cmds[i].arguments[j] != (char *) NULL; j++)
                    fprintf(stderr, "cmds[%d].arguments[%d] = %s\n",
                     i, j, cmds[i].arguments[j]);
                fprintf(stderr, "cmds[%d].cmdflag = %o\n", i,
       cmds[i].cmdflag);
            }
        }
#endif

        int pipe_ends[2];

        for (i = 0; i < commands_count; i++)
        {
            char * input_file = NULL;
            char * output_file = NULL;
            char * append_file = NULL;
            if (i == 0)
            {
                input_file = infile;
            }
            if (i == commands_count - 1)
            {
                output_file = outfile;
                append_file = appfile;
            }

            int in_pipe = -1;
            int out_pipe = -1;

            int in_pipe_other_end = -1;
            int out_pipe_other_end = -1;

            if (cmds[i].is_in_piped)
            {
                in_pipe = pipe_ends[0];
                in_pipe_other_end = pipe_ends[1];
            }

            if (cmds[i].is_out_piped)
            {
                pipe(pipe_ends);
                out_pipe = pipe_ends[1];
                out_pipe_other_end = pipe_ends[0];
            }

            int errcode = execute_command_or_builtin(cmds[i].arguments,
                                                     bkgrnd,
                                                     input_file, output_file, append_file,
                                                     in_pipe, out_pipe,
                                                     in_pipe_other_end, out_pipe_other_end);

            if (in_pipe >= 0)
            {
//                printf("DBG: Close %d & %d", in_pipe, in_pipe_other_end);
                close(in_pipe);
                close(in_pipe_other_end);
            }

            if (errcode != 0)
            {
                printf("ash: process \"%s\" exited with error code %d\n", cmds[i].arguments[0], errcode);
            }
        }
    }
}

