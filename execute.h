//
// Created by glavak on 12.02.17.
//

#ifndef ASH_EXECUTE_H
#define ASH_EXECUTE_H

int execute_command(char ** args, char in_background);

int execute_command_or_builtin(char ** args, char in_background);

#endif //ASH_EXECUTE_H
