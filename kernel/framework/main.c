#include <kernel.h>
#include <klib.h>

//spinlock_t biglock;

/*
void func(void *arg) {
    while(1) {
	    kmt->spin_lock(&biglock);
		printf("Thread-%s on CPU #%d acquired the lock\n", arg, _cpu());
		kmt->spin_unlock(&biglock);
		for (int volatile i = 0; i < 100000; i++) ;
	}
}

struct task tasks[] = {
  { .name = "A" },
  { .name = "B" },
  { .name = "C" },
  { .name = "D" },
  { .name = "E" },
};
*/

int main() {
  _ioe_init();
  _cte_init(os->trap);
  _vme_init(pmm->alloc, pmm->free);
/*
  for (int i = 0; i < LENGTH(tasks); i ++) {
      task_t *task  = &tasks[i];
	  _Area stack   = (_Area) { &task->context + 1, task + 1};
	  task->context = _kcontext(stack, func, (void*)task->name);
	  task->next    = &tasks[(i + 1) % LENGTH(tasks)];
  }
*/

  os->init();
  _mpe_init(os->run);
  return 1;
}
