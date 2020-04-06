#include <common.h>

#define VALID '0'
#define MAGIC '1'
#define MARK '9'

static uintptr_t brk = 0;

void alloc_chk(void* ptr, size_t size) {
	char* tmp = (char*)ptr;
	for(int i = 0; i < size; i ++) {
	    //printf("%c",*(tmp+i));
	    assert((*(tmp+i)) == VALID);
	}
	//printf("\n");
}

void free_chk(uintptr_t begin, uintptr_t end) {
    char* tmp = (char*)begin;
	for(int i = 0; i <= end-begin; i ++) {
	    assert((*(tmp+i)) == VALID);
		//printf("%c",*(tmp+i));
	}
	//printf("\n");
}

static void *kalloc(size_t size) {
	brk = brk?
		ROUNDUP(brk, size) + size :
		(uintptr_t)_heap.start + size;
	void* ptr = (void *)(brk - size);
	assert((uintptr_t)ptr % size == 0);
	alloc_chk(ptr, size);
	memset(ptr, MAGIC, size-1);
    void* end = (void*)((uintptr_t)ptr+size-1);
	memset(end, MARK, 1);
	//printf("%c\n",*(char*)(brk-1));
  return (void *)(brk - size);
}

void brk_down() {
	uintptr_t tmp = brk;
	assert(*(char*)tmp == VALID);
	tmp -= 1;
	while(*(char*)tmp != MARK && *(char*)tmp != MAGIC) {
		*(char*)tmp = VALID;
	    tmp --;	
	}
		printf("*\n");
	assert(*(char*)tmp != VALID);
	brk = tmp+1;
	assert(*(char*)brk == VALID);
  	
}

static void kfree(void *ptr) {
	uintptr_t end = 0;
    char* tmp = (char*)ptr;
	if(*tmp == MARK) {
	    *(char*)tmp = VALID;
		end = (uintptr_t)tmp;
	}

	else if(*tmp == MAGIC){
		while(*tmp == MAGIC) {
		    *(char*)tmp = VALID;
			tmp ++;
		}	
		assert(*tmp == MARK);
		*(char*)tmp = VALID;
		end = (uintptr_t)tmp;
	}

	else {
	    assert(0);
	}
	brk_down();

	free_chk((uintptr_t)ptr, end);

}


static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)_heap.end - (uintptr_t)_heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, _heap.start, _heap.end);
  memset((void*)_heap.start, VALID, pmsize);
  brk = (uintptr_t)_heap.start;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
