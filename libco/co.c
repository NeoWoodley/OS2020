#include "co.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <setjmp.h>
#include <string.h>
#include <time.h>

#define current_chk
#define STACK_SIZE ((1<<16))

static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg) {
  asm volatile (
#if __x86_64__
    "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
      : : "b"((uintptr_t)sp), "d"(entry), "a"(arg)
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
	struct co *    next; // to connect members in list
	struct co *    brother; // to connect members in rand_pool
	jmp_buf        context;
	uint8_t        stack[STACK_SIZE];

}__attribute__ ((aligned (16)));

struct co *current = NULL;

struct co co_list;
struct co *co_list_head = &co_list;

struct co rand_pool;
struct co *rand_pool_head = &rand_pool;

void stack_display(uint8_t* addr, int num) {
#if __x86_64__
	for(int i = 0; i < num; ) {
		if(i % 8 == 0) {
		    printf("%p: ", &addr[i]);
		}
		printf("%04d ", addr[i]);
		if((i+1) % 8 == 0) {
		    printf("\n");
		}
		i ++;
	}	
	printf("\n");
#else
	for(int i = 0; i < num; ) {
		if(i % 4 == 0) {
		    printf("%p: ", &addr[i]);
		}
		printf("%04d ", addr[i]);
		if((i+1) % 4 == 0) {
		    printf("\n");
		}
		i ++;
	}	
	printf("\n");

#endif
}

void stack_head_chk() {
#if __x86_64__
    uint64_t head = 0;
	asm volatile (
		"movq %%rsp, %0;"
		: "=r"(head) 
		: 
		: 
	);
	printf("rsp: 0x%ld\n", head);
	printf("stack bottom: 0x%ld\n", (uint64_t)current->stack);
#else
    uint32_t head = 0;
	asm volatile (
		"movl %%esp, %0;"
		: "=r"(head)
		:
	   	: 
	);
	printf("esp: 0x%d\n", head);
	printf("stack bottom: 0x%d\n", (uint32_t)current->stack);
#endif
}

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
	return;
}

void co_count() {
    assert(co_list_head->next != NULL);
	int count = 0;
	struct co* temp = co_list_head->next;
	while(temp != NULL) {
	    count ++;
		temp = temp->next;
	}
	return;
}

void co_delete(struct co* node) {
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

	int index = 0;
	srand((unsigned int)time(0));
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
	struct co* old;
	while(temp->brother != NULL) {
		old = temp;
		temp = temp->brother;
		old->brother = NULL;
	}
	return;
}

void callback() {
	printf("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n");

	current->status = CO_DEAD;
    co_delete(current);    
	assert(current->status == CO_DEAD);
    struct co* node = (struct co*)malloc(sizeof(struct co));
	do
	{
		rand_choose(co_list_head, node, current);
	} while(node->brother->status == CO_NEW || !strcmp(node->brother->name, current->name));
	assert(node->brother->status == CO_WAITING);
	node->brother->status = CO_RUNNING;
	current = node->brother;
	printf("%s\n", current->name __attribute__(aligned (16)));
	longjmp(node->brother->context, 2);

}


struct co *co_start(const char *name, void (*func)(void *), void *arg) {

	assert(name != NULL && func != NULL && arg != NULL);
	struct co *new_co = (struct co*)malloc(sizeof(struct co));
/*
#if __x86_64__
	printf("callback:%p\n", callback);
    stack_display(&new_co->stack[STACK_SIZE-32], 8);	
#else
	printf("callback:%p\n", callback);
    stack_display(&new_co->stack[STACK_SIZE-40], 4);	
#endif
*/
	new_co->name = (char*)malloc(32*sizeof(char));
	assert(new_co->name != NULL);
	strcpy(new_co->name, name);
	new_co->func = func;
	new_co->arg = arg;
	new_co->status = CO_NEW;
	new_co->next = NULL;
	new_co->brother = NULL;
    
	list_append(co_list_head, new_co);
    assert(co_list_head != NULL);
    return new_co;
}

void co_wait(struct co *co) {
	if(current == NULL && co->status != CO_DEAD) {
		if(co->status == CO_NEW) {
		printf("KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKk\n");
			co->status = CO_RUNNING;
        	current = co;	
			assert(current == co);
			assert(current != NULL);
            
	    	current->func(current->arg);
	    	current->status = CO_DEAD;
	    	current = NULL;

		    co->status = CO_DEAD;
		    assert(co != NULL);
            co_delete(co);
		    free(co);
		    return;
		}

		else if(co->status == CO_WAITING) {
			assert(current == NULL);
			printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
			current = co;
			co_yield();
			printf("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n");
		    co_delete(co);
	    	current = NULL;
			free(co);
			return;
		}
	}

	else if (co->status == CO_DEAD){
		printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\n");
		free(co);
	    return;
	}
	else {
	    assert(0);
	}
}

void co_yield() {
	assert(current != NULL);
	if(current == NULL) {

	    exit(0);
	}
	else {
	    current->status = CO_WAITING;

		assert(current->context != NULL);
		assert(current != NULL);
	
        int val = setjmp(current->context);
        if (val == 0) {
            struct co new_co;
			rand_choose(co_list_head, &new_co, current);

			assert(new_co.brother != NULL);
			assert(new_co.brother->status == CO_NEW || new_co.brother->status == CO_WAITING);
			if (new_co.brother->status == CO_NEW) {
				new_co.brother->status = CO_RUNNING;
				assert(new_co.brother->stack != NULL && new_co.brother->func != NULL && new_co.brother->arg != NULL);
				current = new_co.brother;

#if __x86_64__
	uint64_t num = (uintptr_t)callback;
    for(int i = 0; i < 16; i ++) {
		current->stack[STACK_SIZE-32+i] = num % 256;	    
		num /= 256;
	}	
#else
	uint32_t num = (uintptr_t)callback;
    for(int i = 0; i < 4; i ++) {
		current->stack[STACK_SIZE-40+i] = num % 256;	    
		num /= 256;
	}	
#endif
			    stack_switch_call(&new_co.brother->stack[STACK_SIZE-32], new_co.brother->func, (uintptr_t)new_co.brother->arg);
			}

			else {
			   current = new_co.brother;
			   current->status = CO_RUNNING;
			   longjmp(current->context, 2); 
			}
            
	    }
        else {
    			return;
	    }	
	}
}


