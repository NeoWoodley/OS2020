#include <common.h>

#define MAGIC 1

struct lib {
    uintptr_t addr;
	size_t size;
};

void alloc_chk(void* ptr, size_t size) {
	char* tmp = (char*)ptr;
	for(int i = 0; i < size; i ++) {
		printf("%c\t", *(tmp+i));
		//printf("c\t");
	    //assert((*(tmp+i)) == 0);
	}
	printf("\n");
}

static void *kalloc(size_t size) {
	static uintptr_t brk = 0;
	brk = brk?
		ROUNDUP(brk, size) + size :
		(uintptr_t)_heap.start + size;
	void* ptr = (void *)(brk - size);
	assert((uintptr_t)ptr % size == 0);
	alloc_chk(ptr, size);
	memset(ptr, MAGIC, size);
  return (void *)(brk - size);
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)_heap.end - (uintptr_t)_heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, _heap.start, _heap.end);
  memset((void*)_heap.start, 0, pmsize);
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
