#include "co.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <setjmp.h>
#include <string.h>
#include <time.h>

#define STACK_SIZE (1<<10)

#define DEBUG

static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg) {
#ifdef DEBUG
	printf("In function stack_switch_call!\n");
#endif
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

struct co *current = NULL;

struct co co_list;
struct co *co_list_head = &co_list;

struct co rand_pool;
struct co *rand_pool_head = &rand_pool;

void list_append(struct co* head, struct co* new_co) {
    struct co* temp = head;
	while(temp->next != NULL) {
	   	temp = temp->next;
	}
	temp->next = new_co;
#ifdef DEBUG
    printf("In list_append function! ");
#endif
}

void rand_pool_append(struct co* head, struct co* new_co) {
   	struct co* temp = head;
	while(temp->brother != NULL) {
    	temp = temp->brother;
	}
	temp->brother = new_co;
#ifdef DEBUG
    printf("In rand_pool_append function! co %s was appended\n", temp->brother->name);
#endif
}

void waiter_append(struct co* prev, struct co* current) {
	assert(prev->waiter == NULL);
	prev->waiter = current;
	assert(prev->waiter != NULL);
#ifdef DEBUG
    printf("In waiter_append function!\n");
#endif
}

void rand_choose(struct co* head, struct co* candidate) {

	assert(head != NULL);

    int count = 0;

    struct co* temp = head;
    while(temp != NULL) {
        if(temp->status == CO_NEW || temp->status == CO_WAITING) {
	        rand_pool_append(rand_pool_head, temp);
		    count ++;
	    }
		temp = temp->next;
    }
	assert(rand_pool_head != NULL);

#ifdef DEBUG
	printf("There %d co in rand pool!\n", count);
#endif

	int index = 0;
	srand((unsigned)time(0));
	if(count != 0) {
        index = rand() % count + 1;
	}
	struct co* pool = rand_pool_head;
	for(int i=0; i < index; i ++) {
	    pool = pool->brother;
	}
	candidate->brother = pool;

	assert(candidate->brother != NULL);
#ifdef DEBUG
	printf("co %s was chosen to run!\n", candidate->brother->name);
#endif
    temp = rand_pool_head->brother;
	rand_pool_head->brother = NULL;
	while(temp->brother != NULL) {
	    struct co* old = temp;
		temp = temp->brother;
		old->brother = NULL;
	}
#ifdef DEBUG
	printf("rand pool was cleared!\n");
#endif
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {

	assert(name != NULL && func != NULL && arg != NULL);
	struct co *new_co = (struct co*)malloc(sizeof(struct co));
	new_co->name = (char*)malloc(32*sizeof(char));
	assert(new_co->name != NULL);
	strcpy(new_co->name, name);
	new_co->func = func;
	new_co->arg = arg;
	new_co->status = CO_NEW;
	new_co->next = NULL;
	new_co->brother = NULL;
	for(int i = 0; i < STACK_SIZE; i ++) {
	    new_co->stack[i] = 0;
	}
    
	list_append(co_list_head, new_co);
    assert(co_list_head != NULL);

#ifdef DEBUG
	printf("co %s is created!\n", new_co->name);
#endif 

    return new_co;
}

void co_wait(struct co *co) {

	if(current == NULL) {
		co->status = CO_RUNNING;
        current = co;	
		assert(current != NULL);

#ifdef DEBUG
	printf("main thread was waiting | co %s is running now!\n", current->name);
#endif 

	    current->func(current->arg);
	    current->status = CO_DEAD;
	    current = NULL;

#ifdef DEBUG
	printf("main thread was restored | co %s is finished Now!!\n", co->name);
#endif 

	free(co);
	}
	else {
	    current->status = CO_WAITING;
	    struct co *old_current = current;
	    co->status = CO_RUNNING;
		waiter_append(co, current);
	    current = co;

#ifdef DEBUG
	printf("co %s was replaced | co %s is runing Now!\n", old_current->name, current->name);
#endif 

	    current->func(current->arg);
	    current->status = CO_DEAD;
	    current = old_current;
		current->status = CO_RUNNING;

#ifdef DEBUG
	printf("co %s was restored | co %s is finished Now!!\n", current->name, co->name);
#endif 

	    free(co);
	}
}

void co_yield() {
    
	if(current == NULL) {

#ifdef DEBUG
		printf("Exit from main thread directly!\n");
#endif

	    exit(0);
	}
	else {
	    current->status = CO_WAITING;

#ifdef DEBUG
		printf("yield occured in co %s!\n", current->name);
#endif

        int val = setjmp(current->context);
        if (val == 0) {
#ifdef DEBUG
		printf("The return value of setjmp is 0 | The current co is %s\n", current->name);
#endif
            struct co new_co;
			do {
			    rand_choose(co_list_head, &new_co);
				assert(new_co.next != NULL);
#ifdef DEBUG
		        printf("The temp chosen co is %s | The running co is %s\n", new_co.brother->name, current->name);
#endif
			} while(strcmp(new_co.brother->name, current->name) == 0);
			assert(new_co.brother->status == CO_NEW || new_co.brother->status == CO_WAITING);
			if (new_co.brother->status == CO_NEW) {
#ifdef DEBUG
		        printf("Another co was chosen and it is a new co!\n");
#endif
				assert(&new_co.brother->stack[0] != NULL && new_co.brother->func != NULL && new_co.brother->arg != NULL);
			    stack_switch_call(new_co.brother->stack, new_co.brother->func, (uintptr_t)new_co.brother->arg);
			}

			else {
#ifdef DEBUG
		        printf("Another co was chosen and it is a waiting co!\n");
#endif
			   longjmp(new_co.brother->context, 2); 
			}
            
	    }
        else {
#ifdef DEBUG
		printf("The return value of setjmp is not  0 | The current co is %s\n", current->name);
#endif
			current->status = CO_RUNNING;
            return;	
	    }	
	}
}
