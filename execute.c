//
// Created by glavak on 12.02.17.
//

#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include <stdio.h>
#include <wait.h>
#include <fcntl.h>

#include "execute.h"
#include "shell.h"

int execute_command(char ** args,
                    char in_background,
                    char * infile, char * outfile, char * appfile,
                    int in_pipe, int out_pipe,
                    int in_pipe_other_end, int out_pipe_other_end)
{
    // Fork process:
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("ash: forking");
        return 1;
    }
    else
    {
        // Here we have two processes executing this line of code
        if (pid == 0)
        {
            // Child process
            if (infile != NULL)
            {
                int fr = open(infile, O_CREAT);
                dup2(fr, STDIN_FILENO);
            }
            if (outfile != NULL)
            {
                int fw = open(outfile, O_CREAT);
                dup2(fw, STDOUT_FILENO);
            }
            if (appfile != NULL)
            {
                int fw = open(appfile, O_WRONLY | O_APPEND);//FIXME: problem with prompt earlier than output
                dup2(fw, STDOUT_FILENO);
            }

            if (in_pipe >= 0)
            {
                dup2(in_pipe, STDIN_FILENO);
                close(in_pipe);
                close(in_pipe_other_end);
            }
            if (out_pipe >= 0)
            {
                dup2(out_pipe, STDOUT_FILENO);
                close(out_pipe);
                close(out_pipe_other_end);
            }

            if (in_background)
            {
                // Move process to another group, for the child process
                // not to receive signals from terminal sent to shell
                setpgid(0, 0);
            }

            execvp(args[0], args);

            // If we're here, execvp failed
            perror("asd: executing child process");
            exit(1);
        }
        else
        {
            // Parent process
            if (in_background)
            {
                printf("Process started, pid: %d\n", pid);
                return 0;
            }
            else if (out_pipe >= 0 && 0)
            {
                printf("Process started, pid: %d\n", pid);
                // Process has pipe output, so we should return and launch next
                // process in pipe
                return 0;
            }
            else
            {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                {
                    return WEXITSTATUS(status);
                }
                else
                {
                    return 0;
                }
            }
        }
    }
}

int
execute_command_or_builtin(char ** args,
                           char in_background,
                           char * infile, char * outfile, char * appfile,
                           int in_pipe, int out_pipe,
                           int in_pipe_other_end, int out_pipe_other_end)
{
    if (strcmp(args[0], "cd") == 0)
    {
        return chdir(args[1]);
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }
    else
    {
        return execute_command(args, in_background, infile, outfile, appfile, in_pipe, out_pipe, in_pipe_other_end,
                               out_pipe_other_end);
    }
}