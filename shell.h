//
// Created by glavak on 11.02.17.
//

#ifndef ASH_SHELL_H
#define ASH_SHELL_H

#define MAXARGS 256
#define MAXCMDS 50
#define MAXJOBS 128

#include <wait.h>
#include <fcntl.h>
#include <termio.h>

struct command
{
    char * arguments[MAXARGS];
    char is_in_piped;
    char is_out_piped;
};

struct job
{
    int index;
    int pids[128];
    int status;
    struct termios tmodes;
};

void print_job_status(struct job * job);

extern struct command cmds[];
extern char * infile, * outfile, * appfile;
extern char bkgrnd;
extern struct job jobs[MAXJOBS];
extern struct job * fg_job;
extern int shell_terminal;
extern struct termios shell_tmodes;

int parseline(char *);
void kill_job(struct job * job, int sig);

size_t promptline(char *, size_t);

#endif //ASH_SHELL_H
