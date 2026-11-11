#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand((unsigned int)(time(NULL) ^ GetCurrentProcessId()));

    HANDLE sem = CreateSemaphoreA(NULL, 1, 1, "Local\\lab6_sem");
    if (sem == NULL) {
        printf("Eroare semafor: %lu\n", GetLastError());
        return 1;
    }

    WaitForSingleObject(sem, INFINITE);
    HANDLE hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), "Local\\lab6_mem");
    if (hMap == NULL) {
        printf("Eroare memorie: %lu\n", GetLastError());
        ReleaseSemaphore(sem, 1, NULL);
        CloseHandle(sem);
        return 1;
    }

    int *nr = (int *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));
    if (nr == NULL) {
        printf("Eroare mapare: %lu\n", GetLastError());
        CloseHandle(hMap);
        ReleaseSemaphore(sem, 1, NULL);
        CloseHandle(sem);
        return 1;
    }
    ReleaseSemaphore(sem, 1, NULL);

    while (1) {
        WaitForSingleObject(sem, INFINITE);
        int v = *nr;

        if (v >= 1000) {
            ReleaseSemaphore(sem, 1, NULL);
            break;
        }

        printf("[PID %lu] citit %d\n", GetCurrentProcessId(), v);

        while (v < 1000 && (rand() % 2 + 1) == 2) {
            v++;
            *nr = v;
            printf("[PID %lu] a picat 2 -> scris %d\n", GetCurrentProcessId(), v);
        }

        fflush(stdout);
        ReleaseSemaphore(sem, 1, NULL);
        Sleep(1);
    }

    UnmapViewOfFile(nr);
    CloseHandle(hMap);
    CloseHandle(sem);
    return 0;
}