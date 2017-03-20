//
// Created by glavak on 11.02.17.
//

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "shell.h"

static char * blankskip(char * string)
{
    while (isspace(*string) && *string) ++string;
    return string;
}

int parseline(char * line)
{
    int nargs, ncmds;
    char * s;
    char aflg = 0;
    int rval;
    int i;
    int is_in_string_mode = 0;
    static char delim[] = " \t|&<>;\n";

    /* initialize  */
    bkgrnd = nargs = ncmds = rval = 0;
    s = line;
    infile = outfile = appfile = NULL;
    cmds[0].arguments[0] = NULL;
    for (i = 0; i < MAXCMDS; i++)
    {
        cmds[i].is_in_piped = 0;
        cmds[i].is_out_piped = 0;
    }

    while (*s)
    {        /* until line has been parsed */
        s = blankskip(s);       /*  skip white space */
        if (!*s) break; /*  done with line */

        /*  handle <, >, |, &, and ;  */
        switch (*s)
        {
            case '"':cmds[ncmds].arguments[nargs] = s + 1;
                nargs++;
                cmds[ncmds].arguments[nargs] = (char *) NULL;
                do
                {
                    (*s)++;
                }
                while (*s != '"');
                *s = '\0';
                (*s)++;
                break;
            case '&':bkgrnd = 1;
                *s++ = '\0';
                break;
            case '>':
                if (*(s + 1) == '>')
                {
                    aflg = 1;
                    *s++ = '\0';
                }
                *s++ = '\0';
                s = blankskip(s);
                if (!*s)
                {
                    fprintf(stderr, "syntax error\n");
                    return (-1);
                }

                if (aflg)
                {
                    appfile = s;
                }
                else
                {
                    outfile = s;
                }
                s = strpbrk(s, delim);
                if (isspace(*s))
                {
                    *s++ = '\0';
                }
                break;
            case '<':*s++ = '\0';
                s = blankskip(s);
                if (!*s)
                {
                    fprintf(stderr, "syntax error\n");
                    return -1;
                }
                infile = s;
                s = strpbrk(s, delim);
                if (isspace(*s))
                {
                    *s++ = '\0';
                }
                break;
            case '|':
                if (nargs == 0)
                {
                    fprintf(stderr, "syntax error\n");
                    return (-1);
                }
                cmds[ncmds++].is_out_piped = 1;
                cmds[ncmds].is_in_piped = 1;
                *s++ = '\0';
                nargs = 0;
                break;
            case ';':*s++ = '\0';
                ++ncmds;
                nargs = 0;
                break;
            default:
                /*  a command argument  */
                if (nargs == 0)
                { /* next command */
                    rval = ncmds + 1;
                }
                cmds[ncmds].arguments[nargs] = s;
                nargs++;
                cmds[ncmds].arguments[nargs] = (char *) NULL;
                s = strpbrk(s, delim);
                if (isspace(*s))
                {
                    *s++ = '\0';
                }
                break;
        }  /*  close switch  */
    }  /* close while  */

    /*  error check  */

    /*
         *  The only errors that will be checked for are
     *  no command on the right side of a pipe
         *  no command to the left of a pipe is checked above
     */
    if (cmds[ncmds - 1].is_out_piped)
    {
        if (nargs == 0)
        {
            fprintf(stderr, "syntax error\n");
            return -1;
        }
    }

    return rval;
}

