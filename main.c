#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

//定义一个给clone用的栈，大小1M
#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];

char* const container_args[] = {
    "/bin/bash",
    "-l",
    NULL
};

int container_main(void* arg)
{
    //查看子进程的pid
    printf("Container [%5d] - inside the container!\n", getpid());
    printf("Container - inside the container!\n");
    //直接执行一个shell，以便我们观察这个进程空间里的资源是否被隔离
    system("mount -t proc proc /proc");
    execv(container_args[0], container_args);
    printf("Something's wrong!\n");
    return 1;
}

int main() {
    printf("Parent - start a container!\n");
    //调用clone函数，其中传出一个函数，还有一个栈空间的（为什么传尾指针？因为栈是反的
    int container_pid = clone(container_main, container_stack + STACK_SIZE, 
        CLONE_NEWUTS|SIGCHLD|CLONE_NEWIPC|CLONE_NEWPID|CLONE_NEWNS, NULL);
    waitpid(container_pid, NULL, 0);
    printf("Parent - container stopped!\n");
    return 0;
}
