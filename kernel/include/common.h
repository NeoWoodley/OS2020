#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

#define MAX_CPU 8
#define STACK_SIZE 4096

//intptr_t locked = 0;

//uintptr_t page_brk = 0;

int ntask,ncpu;

typedef struct task {
	union {
    struct {
	    const char *name;
		struct task *next;
		_Context *context;
	};
	uint8_t stack[4096];
	};
} task_t;

struct cpu_local {
    task_t *current;
} cpu_local[MAX_CPU];

#define current cpu_local[_cpu()].current

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

