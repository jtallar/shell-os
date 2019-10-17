/***************************************************
    Scheduler.h
****************************************************/

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include <process.h>

typedef union node {
    struct {
        int used;
        int times;
        Process process;
        union node * next;
    } n;
    long x; // Here happens the align 
} Node;

/* Creates the scheduler */
void initScheduler();

/* Chooses next process to give the CPU */
uint64_t scheduler(uint64_t sp);

/* Adds a new process to the scheduler */
uint8_t add(Process p);

/* Deletes a process given PID */
uint64_t kill(uint64_t pid);

/* Sets priority of the process given its PID */
void setPriority(uint64_t pid, uint8_t n);

/* Sets the state of the process given its PID */
void setState(uint64_t pid, states state);

/* Prints all processes */
void listAll();

/* Returns current process pid */
uint64_t getPid();

#endif /* _SCHEDULER_H_ */