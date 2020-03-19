#include "co.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <setjmp.h>
#include <string.h>

#define STACK_SIZE (1<<10)

static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg) {

  asm volatile (
#if __x86_64__
    "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
      : : "b"((uintptr_t)sp),     "d"(entry), "a"(arg)
#else
    "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
      : : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg)
#endif
  );

}

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

	struct co *    next;
	enum co_status status;
	struct co *    waiter;
	jmp_buf        context;
	uint8_t        stack[STACK_SIZE];

};

void list_append(struct co* head, struct co* new_co) {
	if(head == NULL) {
	    head = new_co;
	}
    else {
    	struct co* temp = head;
		while(temp->next != NULL) {
	    	temp = temp->next;
		}
		temp->next = new_co;
	}
}

struct co *current = NULL;

struct co *co_list = NULL;

struct co *co_start(const char *name, void (*func)(void *), void *arg) {

	assert(name != NULL && func != NULL && arg != NULL);
	struct co *new_co = (struct co*)malloc(sizeof(struct co));
//	new_co->name = name;
    strcpy(new_co->name, name);
	new_co->func = func;
	new_co->arg = arg;
	new_co->status = CO_NEW;
	new_co->next = NULL;
    
	list_append(co_list, new_co);

    return new_co;
}

void co_wait(struct co *co) {

	if(current == NULL) {
		co->status = CO_RUNNING;
        current = co;	
		assert(current != NULL);
	}
	else {
	    current->status = CO_WAITING;
	    struct co *old_current = current;
	    co->status = CO_RUNNING;
	    current = co;
	    current->func(current->arg);
	    current->status = CO_DEAD;
	    current = old_current;
		current->status = CO_RUNNING;
	    free(co);
		assert(co == NULL);
	}
}

void co_yield() {
    
	if(current == NULL) {
	    exit(0);
	}
	else {
	    current->status = CO_WAITING;
        int val = setjmp(current->context);
        if (val == 0) {
            
	    }
        else {
           return;	
	    }	
	}
}
