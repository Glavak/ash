//
// Created by glavak on 11.02.17.
//

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <memory.h>
#include <stdlib.h>
#include <asm/errno.h>
#include <errno.h>

#include "shell.h"
#include "execute.h"

//#define DEBUG

char * infile, * outfile, * appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

int main(int argc, char * argv[])
{
    int i;
    char line[1024];      /*  allow large command lines  */
    int comands_count;
    char * prompt = "> ";      /* shell prompt */

    /* PLACE SIGNAL CODE HERE */

    //sprintf(prompt, "[%s] ", argv[0]);

    while (promptline(prompt, line, sizeof(line)) > 0)
    {
        if ((comands_count = parseline(line)) <= 0)
        {
            continue;
        }
#ifdef DEBUG
        {
            int i, j;
                for (i = 0; i < comands_count; i++) {
                for (j = 0; cmds[i].arguments[j] != (char *) NULL; j++)
                    fprintf(stderr, "cmd[%d].arguments[%d] = %s\n",
                     i, j, cmds[i].arguments[j]);
                fprintf(stderr, "cmds[%d].cmdflag = %o\n", i,
       cmds[i].cmdflag);
            }
        }
#endif

        for (i = 0; i < comands_count; i++)
        {
            int errcode = execute_command_or_builtin(cmds[i].arguments, bkgrnd);

            if (errcode != 0)
            {
                printf("asd: process exited with error code %d", errcode);
            }
        }

    }  /* close while */
}
/* PLACE SIGNAL CODE HERE */