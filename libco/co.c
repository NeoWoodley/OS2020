#include "co.h"
#include "x86.h"
#include <stdlib.h>

#define STACK_SIZE (1<<10)

/*
#if __x86_64__

#else
*/

enum co_status {

    CO_NEW = 1,
	CO_RUNNING,
	CO_WAITING,
	CO_DEAD,

};

struct co {

	char *name;
	void (*func)(void *);
	void *arg;

	enum co_status status;
	struct co *    waiter;
	jmp_buf        context;
	uint8_t        stack[STACK_SIZE];

};

struct co *current;

struct co *co_start(const char *name, void (*func)(void *), void *arg) {

	assert(name != NULL && func != NULL && arg != NULL);
	struct co *new_co = (struct co*)malloc(sizeof(struct co));
	new_co->name = name;
	new_co->func = func;
	new_co->arg = arg;
	new_co->status = CO_NEW;
    
    return new_co;

}

void co_wait(struct co *co) {

	current->status = CO_WAITING;
	struct co *old_current = current;
	co->status = CO_RUNNING;
	current = co;
	current->func(current->arg);
	current->status = CO_DEAD;
	current = old_current;
	free(co);

}

void co_yield() {

	current->status = CO_WAITING;
    int val = setjmo(current->context);
    if (val == 0) {
        	
	}
    else {
	
	}	
}
