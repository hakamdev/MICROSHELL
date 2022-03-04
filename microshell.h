#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_cmd
{
    int count;
    char *args[1024];
}               t_cmd;

typedef struct s_shell
{
    int count;
    int split_count;
    int index;
    int cmd_index;
    char *split[1024];
    t_cmd   cmds[1024];
}               t_shell;

t_shell shell;

#endif