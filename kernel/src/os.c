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

void smoke_test() {
    for(int i = 0; i < 10; i ++){
		if(i%3 == 0) {
	        uintptr_t ptr = (uintptr_t)pmm->alloc(sizeof(char));
		    printf("ptr->char: %p\n", ptr);
		}
		else if(i%3 == 1) {
	        uintptr_t ptr = (uintptr_t)pmm->alloc(sizeof(uintptr_t));
		    printf("ptr->uintptr_t: %p\n", ptr);
		}
		else {
	        uintptr_t ptr = (uintptr_t)pmm->alloc(sizeof(double));
		    printf("ptr->double: %p\n", ptr);
		}
	}
	for(int i = 0; i < 2; i ++) {
	        uintptr_t ptr = (uintptr_t)pmm->alloc(4096*sizeof(char));
		    printf("ptr->4096: %p\n", ptr);
	}
}

static void os_init() {
  pmm->init();
}

static void os_run() {
  os->init();
  _mpe_init(smoke_test);
  while (1) ;
  /*
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    _putc(*s == '*' ? '0' + _cpu() : *s);
  }
  while (1) ;
  */
  
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
