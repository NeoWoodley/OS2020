#include <common.h>

#define VALID '0'
#define MAGIC '1'
#define MARK '9'

#define LACK (((uintptr_t)_heap.end-(uintptr_t)_heap.start) >> 2)

struct header_t {
	union {
	    uintptr_t ptr;
		uintptr_t brk;
	};

	size_t size;

	struct header_t* next;
};

typedef struct header_t header_t;

header_t head;

//static uintptr_t brk = 0;

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
	uintptr_t capacity = (uintptr_t)_heap.end - head.brk;
	void* ptr = NULL;
	header_t header_ptr;  //用于分配出的空间的信息
	if(capacity <= LACK) {
       assert(0); 	    
	}
	else {
        header_t tmp = head; //用于保存空闲空间信息

		memset((void*)(head.brk-sizeof(header_t)), VALID, sizeof(header_t));

		head.brk = head.brk?
			ROUNDUP(head.brk, size) + size :
			(uintptr_t)_heap.start + size;
		ptr = (void *)(head.brk - size);

		header_ptr.ptr = (uintptr_t)ptr;
		header_ptr.size = size;
		header_ptr.next = NULL;

        memcpy((void*)((uintptr_t)ptr-sizeof(header_t)), &header_ptr, sizeof(header_t));
		
		tmp.brk = head.brk+sizeof(header_t);
		tmp.size = (uintptr_t)_heap.end - tmp.brk;
		tmp.next = head.next;

		memcpy((void*)(uintptr_t)head.brk, &tmp, sizeof(header_t));

		head.brk = tmp.brk;
		
		assert((uintptr_t)ptr % size == 0);
		alloc_chk(ptr, size);
		memset(ptr, MAGIC, size-1);
    	void* end = (void*)((uintptr_t)ptr+size-1);
		memset(end, MARK, 1);
	}

  	return ptr;
}

/*
void brk_down() {
	uintptr_t tmp = brk;
	assert(*(char*)tmp == VALID);
	tmp -= 1;
	if(*(char*)tmp == MARK && *(char*)(tmp-1) == MARK) {
		brk = tmp;
		return;
	}
	while(*(char*)tmp != MARK && *(char*)tmp != MAGIC) {
	    tmp --;	
	}
	assert(*(char*)tmp != VALID);
	brk = tmp+1;
	assert(*(char*)brk == VALID);
	printf("brk:%p\n",brk);
	return;
}
*/

static void kfree(void *ptr) {

	header_t free_sp;

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
		printf("char: %c\n",*tmp);
		printf("char: %c\n",*(tmp+1));
	    assert(0);
	}

	free_chk((uintptr_t)ptr, end);

    tmp = ((char*)ptr-sizeof(header_t));
	
	while(*tmp == VALID && (uintptr_t)tmp >= (uintptr_t)_heap.start) {
	    tmp --;
	}
	tmp ++;

	size_t size = (((header_t*)(ptr-sizeof(header_t)))->size);
   
    free_sp.brk = (uintptr_t)tmp + sizeof(header_t);
	free_sp.size = (uintptr_t)ptr+size - free_sp.brk;
	free_sp.next = NULL;

    memcpy((void*)tmp, &free_sp, sizeof(header_t));
	
	header_t* next = &head;

    while(next->next != NULL) {
	    next = next->next;
	} 

	next->next = (header_t*)((uintptr_t)tmp);

	printf("Free success !\n");
		
}


static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)_heap.end - (uintptr_t)_heap.start);
//  printf("Size of header_t: %d\n", sizeof(header_t));
//  printf("Size of size_t: %d\n", sizeof(size_t));
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, _heap.start, _heap.end);
  memset((void*)_heap.start, VALID, pmsize);
  head.brk = (uintptr_t)_heap.start+sizeof(header_t);
  head.size = pmsize-sizeof(header_t);
  head.next =  NULL;
  memcpy((void*)_heap.start, (void*)(&head), sizeof(header_t));
  
  //brk = head.brk;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
