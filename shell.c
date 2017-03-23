//
// Created by glavak on 11.02.17.
//

#include <sys/types.h>
#include <stdio.h>
#include <wait.h>
#include <memory.h>
#include <stdlib.h>

#include <unistd.h>
#include <termio.h>

#include "shell.h"
#include "execute.h"

// #define DEBUG

char * infile, * outfile, * appfile;
struct job jobs[MAXJOBS];
struct command cmds[MAXCMDS];
char bkgrnd;
struct job * fg_job;
int shell_terminal;
struct termios shell_tmodes;

void print_job_status(struct job * job)
{
    printf("[%d] %d ", job->index, job->pids[0]);

    if (WIFEXITED(job->status))
    {
        printf("Done. Exit code: %d\n", WEXITSTATUS(job->status));
        job->pids[0] = -1;
    }
    else if (WIFSTOPPED(job->status))
    {
        printf("Stopped.\n");
    }
    else if (WIFSIGNALED(job->status))
    {
        printf("Signaled. Signal: %d\n", WTERMSIG(job->status));
        job->pids[0] = -1;
    }
    else
    {
        printf("Running.\n");
    }
}

void write_prompt()
{
    char shell_prompt[64];
    char path[64];

    getcwd(path, sizeof(path));

    sprintf(shell_prompt, "\x1b[01;36m%s\x1b[01;31m$\x1b[0m ", path);

    write(STDOUT_FILENO, shell_prompt, strlen(shell_prompt));
}

void kill_job(struct job * job, int sig)
{
    int pid_index = 0;
    while (job->pids[pid_index] > 0)
    {
        kill(job->pids[pid_index], sig);
        pid_index++;
    }
}

void sig_handler(int signo)
{
    if (fg_job == NULL) return;

    if (signo == SIGTSTP)
    {
        kill_job(fg_job, SIGSTOP);
    }
    else
    {
        kill_job(fg_job, signo);
    }
}

int main(int argc, char * argv[])
{
    char line[1024];  /*  allow large command lines  */

    if (signal(SIGINT, sig_handler) == SIG_ERR ||
        signal(SIGQUIT, sig_handler) == SIG_ERR ||
        signal(SIGTSTP, sig_handler) == SIG_ERR ||
        signal(SIGTTIN, SIG_IGN) == SIG_ERR ||
        signal(SIGTTOU, SIG_IGN) == SIG_ERR)
    {
        printf("Can't catch signal");
    }

    for (int i = 0; i < MAXJOBS; ++i)
    {
        jobs[i].index = i;
        jobs[i].pids[0] = -1;
        jobs[i].status = 0;
    }

    fg_job = NULL;
    shell_terminal = STDIN_FILENO;

    int my_pid = getpid();
    setpgid(my_pid, my_pid);

    // Grab control of the terminal
    tcsetpgrp(shell_terminal, getpgrp());

    // Save default terminal attributes for shell
    tcgetattr(shell_terminal, &shell_tmodes);

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

        // Find free job slot
        int jobNum = -1;
        for (int i = 0; i < MAXJOBS; ++i)
        {
            if (jobs[i].pids[0] < 0)
            {
                jobNum = i;
                break;
            }
        }
        jobs[jobNum].status = 0;
        jobs[jobNum].tmodes = shell_tmodes;

        int pipe_ends[2];
        for (int i = 0; i < commands_count; i++)
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
                                                     in_pipe_other_end, out_pipe_other_end,
                                                     jobNum);
        }
    }
}

