#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

//intptr_t locked = 0;

//uintptr_t page_brk = 0;

typedef struct task {
    struct {
	    const char *name;
		struct task *next;
		_Context *context;
	};
	uint8_t stack[4096];
} task_t;

extern task_t *current;
extern task_t tasks[];

struct spinlock {
    char name[64];
	volatile intptr_t locked;
};

struct semaphore {
    int value;
	int cond;
	intptr_t locked;
};

