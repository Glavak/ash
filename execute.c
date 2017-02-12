//
// Created by glavak on 12.02.17.
//

#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include <stdio.h>
#include <wait.h>

#include "execute.h"

int execute_command(char ** args, char in_background)
{
    // Fork process:
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("ash: forking");
    }
    else
    {
        // Here we have two processes executing this line of code
        if (pid == 0)
        {
            // Child process

            execvp(args[0], args);

            // If we're here, execvp failed
            perror("asd: executing child process");
            return 1;
        }
        else
        {
            // Parent process

            if (in_background)
            {
                printf("Process started, pid: %d\n", pid);
            }
            else
            {
                int status;
                waitpid(pid, &status, WUNTRACED);
                return status;
            }

            /*do
            {
                wait_pid = waitpid(pid, &status, WUNTRACED);
            }
            while (!WIFEXITED(status) && !WIFSIGNALED(status));*/
        }
    }

    return 1;
}

int execute_command_or_builtin(char ** args, char in_background)
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
        return execute_command(args, in_background);
    }
}