#include <common.h>

#define VALID '0'
#define MAGIC '1'
#define MARK '9'

/*
uint8_t space[(uintptr_t)_heap.end-(uintptr_t)_heap.start];

struct pointer {
    uintptr_t addr;
	size_t size;

	pointer* prev;
	pointer* next;
}

struct *head = {0, 0, head, head};

void ptr_add(void* ptr, size_t size) {
}
*/

void alloc_chk(void* ptr, size_t size) {
	char* tmp = (char*)ptr;
	for(int i = 0; i < size; i ++) {
		printf("%c",*(tmp+i));
	    //assert((*(tmp+i)) == '0');
	}
	printf("\n");
}

void free_chk(uintptr_t begin, uintptr_t end) {
    char* tmp = (char*)begin;
	for(int i = 0; i <= end-begin; i ++) {
		printf("%c",*(tmp+i));
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
	memset(ptr, MAGIC, size-1);
    void* end = (void*)((uintptr_t)ptr+size-1);
	memset(end, MARK, 1);
	alloc_chk(ptr, size);
  return (void *)(brk - size);
}

static void kfree(void *ptr) {
	uintptr_t end = 0;
    char* tmp = (char*)ptr;
	if(*tmp == MARK) {
	    *tmp = VALID;
		end = (uintptr_t)tmp;
	}

	else if(*tmp == MAGIC){
		while(*tmp == MAGIC) {
		    *tmp = VALID;
			tmp ++;
		}	
		assert(*tmp == MARK);
		*tmp = VALID;
		end = (uintptr_t)tmp;
	}

	else {
	    assert(0);
	}
	free_chk((uintptr_t)ptr, end);
	printf("Free Success\n");
}


static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)_heap.end - (uintptr_t)_heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, _heap.start, _heap.end);
  memset((void*)_heap.start, VALID, pmsize);
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
