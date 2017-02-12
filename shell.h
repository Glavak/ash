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
    char cmdflag;
};

/*  cmdflag's  */
#define OUTPIP  01
#define INPIP   02

extern struct command cmds[];
extern char * infile, * outfile, * appfile;
extern char bkgrnd;

int parseline(char *);

size_t promptline(char *, char *, size_t);

#endif //ASH_SHELL_H
