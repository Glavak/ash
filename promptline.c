//
// Created by glavak on 11.02.17.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>

size_t promptline(char * line, size_t sizline)
{
    size_t n = 0;

    while (1)
    {
        n += read(0, (line + n), sizline - n);
        line[n] = '\0';

        if (line[n - 2] == '\\' && line[n - 1] == '\n')
        {
            // Command ends with "\", replace it with spaces and continue reading next line
            // TODO: cut them instead of replacing
            line[n] = ' ';
            line[n - 1] = ' ';
            line[n - 2] = ' ';
            continue;
        }
        return n;
    }
}

