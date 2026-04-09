#ifndef MINIL_SYS_H
#define MINIL_SYS_H

/* process */
int fork(void);
int execve(const char *path, char *const argv[], char *const envp[]);
int waitpid(int pid, int *status, int options);
void _exit(int status);

/* fs */
int close(int fd);
int chdir(const char *path);

/* pipes */
int pipe(int fd[2]);
int dup2(int oldfd, int newfd);

#endif
