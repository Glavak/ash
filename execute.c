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

int foreground_process(struct job * job);

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
                int fr = open(infile, O_RDONLY);
                dup2(fr, STDIN_FILENO);
            }
            if (outfile != NULL)
            {
                int access_mode =
                        S_IRUSR | S_IRGRP | S_IROTH |
                        S_IWUSR | S_IWGRP;
                int fw = open(outfile, O_WRONLY | O_CREAT, access_mode);
                dup2(fw, STDOUT_FILENO);
            }
            if (appfile != NULL)
            {
                int fw = open(appfile, O_WRONLY | O_APPEND);
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

            pid = getpid();
            setpgid(pid, pid);

            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            /*if (isatty(0)) tcsetpgrp(0, pid);
            if (isatty(1)) tcsetpgrp(1, pid);
            if (isatty(2)) */
            //tcsetpgrp(STDIN_FILENO, pid);

            execvp(args[0], args);

            // If we're here, execvp failed
            perror("asd: executing child process");
            exit(1);
        }
        else
        {
            // Parent process
            setpgid(pid, 0);

            int jobNum = -1;
            for (int i = 0; i < MAXJOBS; ++i)
            {
                if (jobs[i].pid < 0)
                {
                    jobNum = i;
                    break;
                }
            }

            jobs[jobNum].pid = pid;
            jobs[jobNum].isStopped = 0;
            if (in_background)
            {
                printf("[%d] %d\n", jobNum, pid);
                return 0;
            }
            else
            {
                return foreground_process(&jobs[jobNum]);
            }
        }
    }
}

void print_jobs()
{
    for (int i = 0; i < MAXJOBS; ++i)
    {
        if (jobs[i].pid > 0)
        {
            printf("[%d] %d ", i, jobs[i].pid);

            int status;
            int ret = waitpid(jobs[i].pid, &status, WNOHANG | WUNTRACED);
            if (ret == 0)
            {
                if (jobs[i].isStopped)
                {
                    printf("Stopped.\n");
                }
                else
                {
                    printf("Running.\n");
                }
            }
            else if (ret != jobs[i].pid)
            {
                perror("waitpid");
            }
            else if (WIFEXITED(status))
            {
                printf("Done. Status: %d\n", WEXITSTATUS(status));
                jobs[i].pid = -1;
            }
            else if (jobs[i].isStopped || WIFSTOPPED(status))
            {
                printf("Stopped.\n");
                jobs[i].isStopped = 1;
            }
            else if (WIFSIGNALED(status))
            {
                printf("Signaled. Signal: %d\n", WTERMSIG(status));
                jobs[i].pid = -1;
            }
            else
            {
                printf("Running.\n");
            }
        }
    }
}

void background_process(struct job * job)
{
    job->isStopped = 0;
    kill(job->pid, SIGCONT);
}

int foreground_process(struct job * job)
{
    /*
    printf("\n\nErrno = %d\n\n", errno);
    printf("\n\nNew pgid = %d\n\n", getpgid(pid));
    printf("\n\nMy pid = %d\n\n", getpid());*/

    fg_job = job;

    if (job->isStopped)
    {
        job->isStopped = 0;
        kill(job->pid, SIGCONT);
    }

    int status;
    waitpid(job->pid, &status, WUNTRACED);

    if (WIFEXITED(status))
    {
        printf("WIFEXITED triggered\n");
        job->pid = -1;
        return WEXITSTATUS(status);
    }
    else if (WIFSTOPPED(status))
    {
        // Child process suspended with Ctrl-Z
        printf("WIFSTOPPED triggered\n");
        job->isStopped = 1;
        fg_job = NULL;
        return 0;
    }
    else if (WIFSIGNALED(status))
    {
        printf("WIFSIGNALED triggered\n");
        job->pid = -1;
        fg_job = NULL;
        return 0;
    }
    else
    {
        printf("smth weird triggered\n");
        job->pid = -1;
        fg_job = NULL;
        return 0;
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
    else if (strcmp(args[0], "jobs") == 0)
    {
        print_jobs();
        return 0;
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        int job_id;
        sscanf(args[1], "%d", &job_id);
        foreground_process(&jobs[job_id]);
        return 0;
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        int job_id;
        sscanf(args[1], "%d", &job_id);
        background_process(&jobs[job_id]);
        return 0;
    }
    else
    {
        return execute_command(args, in_background,
                               infile, outfile, appfile,
                               in_pipe, out_pipe,
                               in_pipe_other_end, out_pipe_other_end);
    }
}