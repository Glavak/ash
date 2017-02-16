//
// Created by glavak on 11.02.17.
//

#ifndef ASH_SHELL_H
#define ASH_SHELL_H

#define MAXARGS 256
#define MAXCMDS 50

struct command
{
    char * arguments[MAXARGS];
    char is_in_piped;
    char is_out_piped;
};

extern struct command cmds[];
extern char * infile, * outfile, * appfile;
extern char bkgrnd;
extern int signal_to_stop_process;

int parseline(char *);

size_t promptline(char *, size_t);

#endif //ASH_SHELL_H
