#include <common.h>
#include <klib.h>

/*
enum ops { OP_ALLOC = 1, OP_FREE };
struct op {
    enum ops type;
	union {
	    size_t alloc_size;
		void * free_addr;
	};
};

void stress_test() {
    while(1) {
	    struct op op = random_op();
      switch(op.type) {
        case OP_ALLOC:
          alloc_check(pmm->alloc(op.alloc_size, op.alloc_size));
          break;
        case OP_FREE:
          free(op.free_addr);
          break;
      }
	}
}
*/

/*
void smoke_test() {
node lib[28];
	void* cur = pmm->alloc(45*sizeof(char));
	lib[0].cur = cur;
	for(int i = 1; i < 7; i ++) {
		cur = pmm->alloc(78*sizeof(char)); 
		lib[i-1].next = cur;
		lib[i].cur = cur;
	}
	for(int i = 7; i < 12; i ++) {
		cur = pmm->alloc(127*sizeof(char)); 
		lib[i-1].next = cur;
		lib[i].cur = cur;
	}
	for(int i = 12; i < 19; i ++) {
		cur = pmm->alloc(4096*sizeof(char)); 
		lib[i-1].next = cur;
		lib[i].cur = cur;
	}
	for(int i = 19; i < 28; i ++) {
		cur = pmm->alloc(201*sizeof(char)); 
		lib[i-1].next = cur;
		lib[i].cur = cur;
	}

	for(int i = 0; i < 28; i ++) {
	    pmm->free(lib[i].cur);
	}

	*/
	/*
	while(1) {
	    uintptr_t ptr = (uintptr_t)pmm->alloc(4096*sizeof(char));
		pmm->free((void*)ptr);
	}
	*/
	
//	int i = 0;
   // for(; i < 20; i ++){
	/*
	while(1) {
//		if(i%5 == 0) {
	        void* ptr1 = pmm->alloc(sizeof(char));
//	        pmm->alloc(sizeof(char));
//		    printf("ptr->char: %p, I:%d from cpu #%d\n", ptr, i, _cpu());
		    pmm->free((void*)ptr1);	
//		}
//		else if(i%5 == 1) {
//	        uintptr_t ptr = (uintptr_t)pmm->alloc(sizeof(uintptr_t));
	        void* ptr2 = pmm->alloc(sizeof(uintptr_t));
//		    printf("ptr->uintptr_t: %p, I:%d from cpu #%d\n", ptr, i, _cpu());
		    pmm->free((void*)ptr2);	
//		}
//		else if(i%5 == 2){
//	        uintptr_t ptr = (uintptr_t)pmm->alloc(127*sizeof(char));
	        void* ptr3 = pmm->alloc(125*sizeof(char));
//		    printf("ptr->127: %p, I:%d from cpu #%d\n", ptr, i, _cpu());
		    pmm->free((void*)ptr3);	
//		}
//		else if(i%5 == 3){
//	        uintptr_t ptr = (uintptr_t)pmm->alloc(63*sizeof(char));
	        void* ptr4 = pmm->alloc(63*sizeof(char));
//		    printf("ptr->63: %p, I:%d from cpu #%d\n", ptr, i, _cpu());
		    pmm->free((void*)ptr4);	
//		}
//		else if(i%5 == 3){
//	        uintptr_t ptr = (uintptr_t)pmm->alloc(63*sizeof(char));
	        void* ptr5 = pmm->alloc(139*sizeof(char));
//		    printf("ptr->63: %p, I:%d from cpu #%d\n", ptr, i, _cpu());
		    pmm->free((void*)ptr5);	
//		}
//		else {
//	        uintptr_t ptr = (uintptr_t)pmm->alloc(4096*sizeof(char));
	        void* ptr6 = pmm->alloc(4096*sizeof(char));
//		    printf("ptr->4096: %p, I:%d from cpu #%d\n", ptr, i, _cpu());
		    pmm->free((void*)ptr6);	
//		}
	}
    
	//printf("Done From cpu:%d\n", _cpu());
  */
/*
  printf("Done From cpu:%d\n", _cpu());
  while (1) ;
}
*/

static void os_init() {
  pmm->init();
  kmt->init();
}

static void os_run() {
	_intr_write(1);
	while(1);
//  os->init();
//  _mpe_init(smoke_test);
  
  /*
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    _putc(*s == '*' ? '0' + _cpu() : *s);
  }
  while (1) ;
  */
  
}

task_t *current = NULL;
task_t tasks[] = {
  { .name = "a" },
  { .name = "b" },
  { .name = "c" },
};

static _Context* os_trap(_Event ev, _Context *context) {
	if(!current) {
	    current = &tasks[0];
	}
	else {
	    current->context = context;
		current = current->next;
	}

    return current->context;	
}

static void os_on_irq(int seq, int event, handler_t handler) {
   
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap = os_trap,
  .on_irq = os_on_irq,
};
