#include "co.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <setjmp.h>
#include <string.h>
#include <time.h>

#define STACK_SIZE (1<<10)

//#define random(x) (rand()%x)

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

	enum co_status status;
	struct co *    waiter;
	struct co *    next; // to connect members in list
	struct co *    brother; // to connect members in rand_pool
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

void rand_pool_append(struct co* head, struct co* new_co) {
	if(head == NULL) {
	    head = new_co;
	}
    else {
    	struct co* temp = head;
		while(temp->brother != NULL) {
	    	temp = temp->brother;
		}
		temp->brother = new_co;
	}
}

void waiter_append(struct co* prev, struct co* current) {
	assert(prev->waiter == NULL);
	prev->waiter = current;
	assert(prev->waiter != NULL);
}

struct co* rand_pool = NULL;

void rand_choose(struct co* head, struct co* candidate) {
    int count = 0;

    struct co* temp = head;
    while(temp != NULL) {
        if(temp->status == CO_NEW || temp->status == CO_WAITING) {
	        rand_pool_append(rand_pool, temp);
		    count ++;
	    }
		temp = temp->next;
    }
    
	srand((unsigned)time(NULL));
	index = rand();
	index %= count;
	struct co* pool = rand_pool;
	for(int i=0; i < index; i ++) {
	    pool = pool->brother;
	}
	candidate = pool;

	rand_pool = NULL;

}

struct co *current = NULL;

struct co *co_list = NULL;

struct co *co_start(const char *name, void (*func)(void *), void *arg) {

	assert(name != NULL && func != NULL && arg != NULL);
	struct co *new_co = (struct co*)malloc(sizeof(struct co));
    strcpy(new_co->name, name);
	new_co->func = func;
	new_co->arg = arg;
	new_co->status = CO_NEW;
	new_co->next = NULL;
	new_co->brother = NULL;
	for(int i = 0; i < STACK_SIZE; i ++) {
	    new_co->stack[i] = 0;
	}
    
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
		waiter_append(co, current);
	    current = co;
	    current->func(current->arg);
	    current->status = CO_DEAD;
	    current = old_current;
		current->status = CO_RUNNING;
	    free(co);
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
            struct co* new_co = NULL;
			rand_choose(co_list, new_co);
			assert(new_co->status == CO_NEW || new_co->status == CO_WAITING);
			
			if (new_co->status == CO_NEW) {
			    stack_switch_call(&new_co->status, new_co->func, new_co->name);
			}
			else {
			   longjmp(new_co->context, 2); 
			}
            
	    }
        else {
			current->status = CO_RUNNING;
            return;	
	    }	
	}
}
