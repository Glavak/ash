//
// Created by glavak on 12.02.17.
//

#ifndef ASH_EXECUTE_H
#define ASH_EXECUTE_H

int execute_command(char ** args,
                    char in_background,
                    char * infile, char * outfile, char * appfile,
                    int in_pipe, int out_pipe,
                    int in_pipe_other_end, int out_pipe_other_end);

int execute_command_or_builtin(char ** args,
                               char in_background,
                               char * infile, char * outfile, char * appfile,
                               int in_pipe, int out_pipe,
                               int in_pipe_other_end, int out_pipe_other_end);

#endif //ASH_EXECUTE_H
