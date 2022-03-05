#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <err.h>

typedef struct s_cmd
{
    int     count;
    int     fd[2];
    char    *args[1024];
}               t_cmd;

typedef struct s_shell
{
    int     cmd_count;
    int     cmd_index;
    int     cmd_split_index;
    int     split_count;
    int     split_index;
    char    *split[1024];
    t_cmd   cmds[1024];
    int     status;
}               t_shell;

t_shell     shell;

#endif