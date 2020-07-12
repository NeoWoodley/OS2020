#include <common.h>
#include <klib.h>

//static int task_count = 0;

static void kmt_init() {
	printf("Hello from kmt_init()!\n");
    //glk->locked = 0;
	//ncpu = 4;
	//ntask = 8;
	return;
}

static int count = 0;

static int create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {
    count ++;
	assert(count <= 5);
    return 0;
}

static void teardown(task_t *task) {
	//assert(0);
}

static void spin_init(spinlock_t *lk, const char *name) {
	lk->locked = 0;
	strcpy(&lk->name[0], name);

}

static void spin_unlock(spinlock_t *lk) {
	_atomic_xchg(&lk->locked, 0);
	_intr_write(1);
}

static void spin_lock(spinlock_t *lk) {
	_intr_write(0);
	while (_atomic_xchg(&lk->locked, 1));
}

static void sem_init(sem_t *sem, const char *name, int value) {
	return;
}

static void sem_wait(sem_t *sem) {
	return;
}

static void sem_signal(sem_t *sem) {
	return;
}

MODULE_DEF(kmt) = {
  .init = kmt_init,
  .create = create,
  .teardown = teardown,
  .spin_init = spin_init,
  .spin_lock = spin_lock,
  .spin_unlock = spin_unlock,
  .sem_init = sem_init,
  .sem_wait = sem_wait,
  .sem_signal = sem_signal,
};
