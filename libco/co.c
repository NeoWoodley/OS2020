#include "co.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <setjmp.h>
#include <string.h>
#include <time.h>

#define STACK_SIZE (1<<16)+16

//#define DEBUG
//#define JMP
//#define TEST_2
//#define CO_DELETE
#define BUG

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

enum co_status {

    CO_NEW = 1,
	CO_RUNNING,
	CO_WAITING,
	CO_DEAD,

};

struct co {

	char *name;
	__attribute__ ((aligned (16))) void (*func)(void *);
	void *arg;

	enum co_status status;
	struct co *    waiter;
	struct co *    next; // to connect members in list
	struct co *    brother; // to connect members in rand_pool
	jmp_buf        context;
	uint8_t        stack[STACK_SIZE];

}__attribute__ ((aligned (16)));;

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
	return;
}

void co_list_check()
{
    assert(co_list_head->next == NULL);
}

void rand_pool_append(struct co* head, struct co* new_co) {
   	struct co* temp = head;
	while(temp->brother != NULL) {
    	temp = temp->brother;
	}
	temp->brother = new_co;
}

void waiter_append(struct co* prev, struct co* current) {
	assert(prev->waiter == NULL);
	prev->waiter = current;
	assert(prev->waiter != NULL);
	return;
}

void co_delete(struct co* node) {
#ifdef CO_DELETE
	printf("***In co_delete function\n");
#endif
    struct co* temp = co_list_head->next;
	struct co* before_temp = co_list_head;
	while(temp != NULL) {
	    if(!strcmp(temp->name, node->name)) {
			struct co* next = temp->next;
			if(next == NULL) {
			    before_temp->next = NULL;
				break;
			}	
			else {
				assert(next != NULL);
				before_temp->next = next;
				break;
			}
		}
		else {
		    before_temp = temp;
			temp = temp->next;
		}
	}
#ifdef CO_DELETE
	printf("co %s was deleted from co_list\n", node->name);
#endif
#ifdef BUG
	printf("###[DELETE]:co %s was deleted\n",node->name);
#endif
	return;
}

void rand_choose(struct co* head, struct co* candidate, struct co* current) {

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
	printf("### In rand_choose function, there are %d members in rand pool\n", count);
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

	if(!strcmp(candidate->brother->name, current->name)) {
	    if(count == 2) {
		    index = count + 1 - index;
	        pool = rand_pool_head;
	        for(int i=0; i < index; i ++) {
	   	        pool = pool->brother;
	        }
	        candidate->brother = pool;
		}

		else {
		    index = (index + 1) % count + 1;
	        pool = rand_pool_head;
	        for(int i=0; i < index; i ++) {
	   	        pool = pool->brother;
	        }
	        candidate->brother = pool;
		}
	}

	assert(candidate->brother != NULL);
    temp = rand_pool_head->brother;
	rand_pool_head->brother = NULL;
	while(temp->brother != NULL) {
	    struct co* old = temp;
		temp = temp->brother;
		old->brother = NULL;
	}
#ifdef BUG
	printf("###[CHOOSE]:co %s was chosen\n",pool->name);

#endif
	return;
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {

	assert(name != NULL && func != NULL && arg != NULL);
	struct co *new_co = (struct co*)malloc(sizeof(struct co));
#ifdef TEST_2
	printf("A new space was distributed!\n");
#endif
	new_co->name = (char*)malloc(32*sizeof(char));
	assert(new_co->name != NULL);
	strcpy(new_co->name, name);
#ifdef TEST_2
	printf("The name was copied!\n");
#endif
	new_co->func = func;
#ifdef TEST_2
	printf("The function was pointed!\n");
#endif
	new_co->arg = arg;
#ifdef TEST_2
	printf("The arg was assigned!\n");
#endif
	new_co->status = CO_NEW;
	new_co->next = NULL;
	new_co->brother = NULL;
    
	list_append(co_list_head, new_co);
#ifdef TEST_2
	printf("The new co was appended!\n");
#endif
    assert(co_list_head != NULL);
#ifdef DEBUG
	printf("co %s was created! It's state is %d\n", new_co->name, new_co->status);
#endif
#ifdef BUG
	printf("###[CREATE]:co %s was deleted\n",new_co->name);
#endif
    return new_co;
}

void co_wait(struct co *co) {
#ifdef BUG
	printf("###[WAIT]:co %s was waited\n",co->name);
#endif
#ifdef DEBUG
	printf("co %s is to be waited, its state is %d\n", co->name, co->status);
#endif
	if(current == NULL && co->status != CO_DEAD) {
		if(co->status == CO_NEW) {
			co->status = CO_RUNNING;
        	current = co;	
			assert(current != NULL);

	    	current->func(current->arg);
	    	current->status = CO_DEAD;
	    	current = NULL;

		co->status = CO_DEAD;
		assert(co != NULL);
#ifdef DEBUG
		printf("co %s was freed\n", co->name);
#endif
        co_delete(co);
#ifdef BUG
	printf("###[FREE]:co %s was freed\n",co->name);
#endif
		free(co);
		return;
		}

		else if(co->status == CO_WAITING) {
#ifdef JMP
			printf("co %s was once run\n", co->name);
#endif
			current = co;
			/*
			longjmp(current->context, 2);
			*/
			co_yield();
#ifdef DEBUG
		printf("co %s was freed\n", co->name);
#endif
		    co_delete(co);
#ifdef BUG
	printf("###[FREE]:co %s was freed\n",co->name);
#endif
			free(co);
			return;
		}
	}
	else if (current != NULL && co->status != CO_DEAD){
	    current->status = CO_WAITING;
	    struct co *old_current = current;
	    co->status = CO_RUNNING;
		waiter_append(co, current);
	    current = co;

	    current->func(current->arg);
	    current->status = CO_DEAD;
	    current = old_current;
		current->status = CO_RUNNING;

	    assert(co != NULL);
		co->status = CO_DEAD;
		co_delete(co);
#ifdef BUG
	printf("###[FREE]:co %s was freed\n",co->name);
#endif
	    free(co);
		return;
	}

	else {
	    return;
	}
}

void co_yield() {
#ifdef BUG
	printf("###[YIELD]:co %s was yield\n",current->name);
#endif
	if(current == NULL) {
	    exit(0);
	}
	else {
	    current->status = CO_WAITING;

        int val = setjmp(current->context);
        if (val == 0) {
#ifdef BUG
	printf("###[SETJMP]:co %s's context was saved\n",current->name);
#endif
#ifdef JMP
			printf("A longjmp returned 0, co %s's context was saved\n", current->name);
#endif
            struct co new_co;
			rand_choose(co_list_head, &new_co, current);
			assert(new_co.brother != NULL);
			assert(new_co.brother->status == CO_NEW || new_co.brother->status == CO_WAITING);
			if (new_co.brother->status == CO_NEW) {
				assert(new_co.brother->stack != NULL && new_co.brother->func != NULL && new_co.brother->arg != NULL);
				current = new_co.brother;
#ifdef BUG
	printf("###[STACK_SWITCH_CALL]:co %s was put on stack\n",current->name);
#endif
			    stack_switch_call(&new_co.brother->stack[STACK_SIZE-16], new_co.brother->func, (uintptr_t)new_co.brother->arg);
#ifdef DEBUG
				printf("Haha! I am here\n");
#endif
			}

			else {
			   current = new_co.brother;
			   current->status = CO_RUNNING;
#ifdef BUG
	printf("###[LONGJMP]:co %s's context was restored\n",current->name);
#endif
			   longjmp(current->context, 2); 
			}
            
	    }
        else {
#ifdef JMP
			printf("A longjmp returned 2, co %s's context was restored\n", current->name);
#endif
#ifdef BUG
	printf("###[SETJMP]:co %s's context was restored\n",current->name);
#endif
			return;
	    }	
	}
}


