#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

#define MAX_CPU 4

//intptr_t locked = 0;

//uintptr_t page_brk = 0;

int ntask,ncpu;

typedef struct task {
    struct {
	    const char *name;
		int id, cpu;
		struct task *next;
		_Context *context;
	};
	uint8_t stack[4096];
} task_t;

struct cpu_local {
    task_t *current, idle;
	int i;
} cpu_local[MAX_CPU];

#define CPU (&cpu_local[_cpu()])
#define current (CPU->current)
#define IDLE (&CPU->idle)

extern task_t tasks[];

struct spinlock {
    char name[64];
	volatile intptr_t locked;
};

struct spinlock *glk;

struct semaphore {
    int value;
	int cond;
	intptr_t locked;
};

