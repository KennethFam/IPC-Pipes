#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

#define BUFFER_SIZE 100

int main() {
    int pipefd_p[2], pipefd_c[2], result;
    char read_msg[BUFFER_SIZE];
    char write_msg[BUFFER_SIZE];
    if (pipe(pipefd_p) == -1) {
        fprintf(stderr, "Parent pipe creation failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pipe(pipefd_c) == -1) {
        fprintf(stderr, "Child pipe creation failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    else if (pid == 0) {
        if (read(pipefd_c[0], read_msg, BUFFER_SIZE) == -1) {
            fprintf(stderr, "Child pipe read failed! Error #%d: %s\n", errno, strerror(errno));
            return 1;
        }
        fprintf(stdout, "%s", read_msg);
        sprintf(write_msg, "Daddy, my name is %d\n", getpid());
        if (write(pipefd_p[1], write_msg, BUFFER_SIZE) == -1) {
            fprintf(stderr, "Write to parent pipe failed! Error #%d: %s\n", errno, strerror(errno));
            return 1;
        }
        exit(0);
    }
    else {
        sprintf(write_msg, "I am your daddy! and my name is %d\n", getpid());
        if (write(pipefd_c[1], write_msg, BUFFER_SIZE) == -1) {
            fprintf(stderr, "Write to child pipe failed! Error #%d: %s\n", errno, strerror(errno));
            return 1;
        }
        if (close(pipefd_c[0]) == -1) {
            fprintf(stderr, "Read end of child pipe failed to close! Error #%d: %s\n", errno, strerror(errno));
            return 1;
        }
        if (read(pipefd_p[0], read_msg, BUFFER_SIZE) == -1) {
            fprintf(stderr, "Parent pipe read failed! Error #%d: %s\n", errno, strerror(errno));
            return 1;
        }
        fprintf(stdout, "%s", read_msg);
    }
    if (wait(&result) < 0) {
        fprintf(stderr, "An error occured with the wait function call! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (result < 0) {
        fprintf(stderr, "The child process did not exit properly!");
        return 1;
    }
    if (close(pipefd_p[0]) == -1) {
        fprintf(stderr, "Read end of parent pipe failed to close! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (close(pipefd_p[1]) == -1) {
        fprintf(stderr, "Write end of parent pipe failed to close! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (close(pipefd_c[1]) == -1) {
        fprintf(stderr, "Write end of child pipe failed to close! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    return 0;
}