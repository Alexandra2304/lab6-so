#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int main() {
    srand(time(NULL) ^ getpid());
    sem_t *sem = sem_open("/lab6_sem", O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) { perror("sem_open"); return 1; }
    sem_wait(sem);
    int fd = shm_open("/lab6_mem", O_CREAT | O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return 1; }
    ftruncate(fd, sizeof(int));
    int *nr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (nr == MAP_FAILED) { perror("mmap"); return 1; }
    sem_post(sem);
    while (1) {
        sem_wait(sem);
        int v = *nr;
        if (v >= 1000) { sem_post(sem); break; }
        printf("[PID %d] citit %d\n", getpid(), v);
        while (v < 1000 && (rand() % 2 + 1) == 2) {
            v++;
            *nr = v;
            printf("[PID %d] a picat 2 -> scris %d\n", getpid(), v);
        }
        fflush(stdout);
        sem_post(sem);
        usleep(1000);
    }
    munmap(nr, sizeof(int));
    close(fd);
    sem_close(sem);
    return 0;
}
