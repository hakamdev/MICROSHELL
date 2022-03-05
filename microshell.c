#include "microshell.h"

void initShell() {
    shell.split_index = 1;
    shell.cmd_split_index = 0;
    shell.cmd_index = 0;
    shell.cmd_count = 0;
    shell.split_count = 0;
}

void getNextSplit(int ac, char **av) {
    // init split count to 0 at each call
    shell.split_count = 0;
    // check if we reached the end
    if (shell.split_index > ac - 1) return ;
    // remember beginning of args
    int prev_index = shell.split_index;
    // skip 1st arg if it's a ;
    if (strcmp(av[shell.split_index], ";") == 0)
        prev_index = ++shell.split_index;
    // find the next ; to know where args end
    while (shell.split_index < ac && strcmp(av[shell.split_index], ";") != 0)
        ++shell.split_index;
    // check if no args were found and return
    if (shell.split_index == prev_index) return ;
    // copy all args to a new split
    int i = 0;
    while (prev_index < shell.split_index)
        shell.split[i++] = av[prev_index++];
    shell.split[i] = NULL;
    // remember how many args found to pass as ac to getNextCmd
    shell.split_count = i;
    // remember how many cmds are in current split
    i = 0;
    shell.cmd_count = 1;
    while (shell.split[i])
        if (strcmp(shell.split[i++], "|") == 0)
            ++shell.cmd_count;
    // reset cmd related variables
    shell.cmd_index = 0;
    shell.cmd_split_index = 0;
}

void getNextCmd(int ac, char **av) {
    // cmd1 arg1 arg2 | cmd2 arg1 | cmd3
    if (shell.cmd_split_index >= ac) return;
    int prev_index = shell.cmd_split_index;
    if (strcmp(av[shell.cmd_split_index], "|") == 0)
        prev_index = ++shell.cmd_split_index;
    //printf("bfr: prev %d, indx %d, ac %d\n", prev_index, shell.cmd_split_index, ac);
    while (shell.cmd_split_index < ac
        && strcmp(av[shell.cmd_split_index], "|") != 0)
        ++shell.cmd_split_index;
    if (shell.cmd_split_index == prev_index) return;
    int i = 0;
    while (prev_index < shell.cmd_split_index)
        shell.cmds[shell.cmd_index].args[i++] = av[prev_index++];
    //printf("aftr: prev %d, indx %d, ac %d\n", prev_index, shell.cmd_split_index, ac);
    shell.cmds[shell.cmd_index].args[i] = NULL;
    shell.cmds[shell.cmd_index].count = i;
}

int handleNextSplit(int ac, char **av) {
    getNextSplit(ac, av);
    if (shell.split_count <= 0) return 1;
     while (shell.cmd_index < shell.cmd_count)
    {
        getNextCmd(shell.split_count, shell.split);
        shell.cmd_index++;
    }
    return 0;
}

void closePipes(int index) {
    int prev = index - 1;
    close(shell.cmds[index].fd[0]);
    close(shell.cmds[index].fd[1]);
    if (prev > -1) {
        close(shell.cmds[prev].fd[0]);
        close(shell.cmds[prev].fd[1]);
    }
}

void closePipesParent(int index)
{
    int prev = index - 1;
    if (shell.cmd_count > 1 && index == 0) {
        close(shell.cmds[index].fd[1]);
    } else if (shell.cmd_count > 1 && index == shell.cmd_count - 1) {
        close(shell.cmds[prev].fd[0]);
        close(shell.cmds[index].fd[0]);
        close(shell.cmds[index].fd[1]);
    } else if (shell.cmd_count > 1) {
        close(shell.cmds[prev].fd[0]);
        close(shell.cmds[index].fd[1]);
    }
}

void setupPipe(int index) {
    int prev = index - 1;
    if (shell.cmd_count > 1 && index == 0) {
        dup2(shell.cmds[index].fd[1], 1);
    } else if (shell.cmd_count > 1 && index == shell.cmd_count - 1) {
        dup2(shell.cmds[prev].fd[0], 0);
    } else if (shell.cmd_count > 1) {
        dup2(shell.cmds[prev].fd[0], 0);
        dup2(shell.cmds[index].fd[1], 1);
    }
    closePipes(index);
}

int runCommands(int index, char **env) {
    if (shell.cmd_count == 1 && strcmp(shell.cmds[0].args[0], "cd") == 0) {
        // RUN CD
        return 0;
    }
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid == 0) {
        setupPipe(index);
        execve(shell.cmds[index].args[0], shell.cmds[index].args, env);
        perror("microshell");
        exit(1);
    } else {
        closePipesParent(index);
        waitpid(pid, &shell.status, 0);
    }
    return (0);
}

void startMicroshell(char **env) {
    int i = 0;
    while(i < shell.cmd_count) {
        if (pipe(shell.cmds[i].fd) == -1) exit(1);
        runCommands(i, env);
        ++i;
    }
}

int main(int ac, char **av, char **env) {
    char **args = NULL;
    
    initShell();
    while (handleNextSplit(ac, av) == 0)
        startMicroshell(env);
    return (0);
}
