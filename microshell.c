#include "microshell.h"

// DEBUG
void    printARGS(int num, char **av) {
    printf("SPLIT NUMBER (%d)\n", num);
    printf("[");
    for (int i = 0; av[i]; i++) {
        printf(" %s,", av[i]);
    }
    printf("]\n");
}

// 1st call
void    initShell() {
    shell.index = 0;
    shell.cmd_index = 0;
    shell.count = 0;
    shell.split_count = 0;
}

// on each ;
void    getNextSplit(int ac, char **av) {
    shell.split_count = 0;
    if (ac < 2)
        return ;
    if (shell.index >= ac - 1)
        return ;
    int prev_index = ++shell.index;
    if (shell.index == 1 && strcmp(av[shell.index], ";") == 0)
        prev_index = ++shell.index;
    while (shell.index < ac && strcmp(av[shell.index], ";") != 0)
        ++shell.index;
    if (shell.index == prev_index)
        return ;
    int i = 0;
    while (prev_index < shell.index) {
        shell.split[i++] = av[prev_index++];
    }
    shell.split[i] = NULL;
    shell.split_count = i;
}

void    getNextCmd(int ac, char **av) {
    if (ac < 2)
        return NULL;
    if (shell.index >= ac - 1)
        return NULL;
    char    **args = (char **)malloc(sizeof(char *) * ac);
    if (!args)
        return NULL;
    int prev_index = ++shell.index;
    if (shell.index == 1 && strcmp(av[shell.index], ";") == 0)
        prev_index = ++shell.index;
    while (shell.index < ac && strcmp(av[shell.index], ";") != 0)
        ++shell.index;
    if (shell.index == prev_index)
        return NULL;
    int i = 0;
    while (prev_index < shell.index) {
        args[i++] = av[prev_index++];
    }
    args[i] = NULL;
    return (args);
}

int main(int ac, char **av, char **env) {
    char **args = NULL;
    int i = 1;
    initShell();
    getNextSplit(ac, av);
    while (shell.split_count > 0) {
        printARGS(i++, shell.split);
        getNextSplit(ac, av);
    }
    return (0);
}