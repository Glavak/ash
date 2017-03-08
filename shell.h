//
// Created by glavak on 11.02.17.
//

#ifndef ASH_SHELL_H
#define ASH_SHELL_H

#define MAXARGS 256
#define MAXCMDS 50
#define MAXJOBS 128

struct command
{
    char * arguments[MAXARGS];
    char is_in_piped;
    char is_out_piped;
};

struct job
{
    int index;
    int pid;
    int isStopped;
};

//void print_job(struct job job)
//{
//    if (WIFEXITED(job.status))
//    {
//        printf("Done. Status: %d\n", WEXITSTATUS(job.status));
//    }
//    else if (WIFSTOPPED(job.status))
//    {
//        printf("Stopped.\n");
//        jobs[i].isStopped = 1;
//    }
//}

extern struct command cmds[];
extern char * infile, * outfile, * appfile;
extern char bkgrnd;
extern struct job jobs[MAXJOBS];
extern struct job * fg_job;

int parseline(char *);

size_t promptline(char *, size_t);

#endif //ASH_SHELL_H
