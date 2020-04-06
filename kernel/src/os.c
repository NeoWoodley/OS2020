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
	    uintptr_t ptr = (uintptr_t)pmm->alloc(sizeof(char));
		printf("ptr: %p\n", ptr);
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
