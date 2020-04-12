#include <common.h>

#define VALID '0'
#define MAGIC '1'
#define MARK '9'

#define KiB (1 << 10)

//#define LACK (((uintptr_t)_heap.end-(uintptr_t)_heap.start) >> 2)

//#define CUR

intptr_t atomic_xchg(volatile intptr_t *addr, intptr_t newval) {
    intptr_t result;
	asm volatile ("lock xchg %0, %1":
	  "+m"(*addr), "=a"(result) : "1"(newval) : "cc");
	return result;
}

intptr_t locked = 0;
uintptr_t page_brk = 0;

static inline void lock() {
    while(1) {
	    intptr_t value = atomic_xchg(&locked, 1);
		if(value == 0) {
		    break;
		}
	}
}

static inline void unlock() {
    atomic_xchg(&locked, 0);
}

enum header_type {

	FREE_SPACE = 1,
	PTR_SPACE,

};

enum status_type {

	FREE = 1,
	USED,
	FULL,

};

struct header_t {
	union {
	    uintptr_t ptr;
		uintptr_t brk;
	};

	uintptr_t type;

	size_t size;

	struct header_t* next;
};

struct page_t {
    uintptr_t ptr;
    uintptr_t brk;

	uintptr_t status;
	uintptr_t No;

	struct page_t* next;
};

typedef struct page_t page_t;

page_t *page_head;

uintptr_t bound_aligned(size_t size) {
    uintptr_t round = 1;
	while (size > round) {
	    round = (round << 1);
	}
	return round;
}

uintptr_t page_construct() {
	uintptr_t count = 0;
	uintptr_t size = 4 * KiB;
	for(int i = 0 ; i < (1<<13); i ++) {
	    page_brk = page_brk?
		    ROUNDUP(page_brk, size) + size :
		    (uintptr_t)_heap.start + size;
	    void* ptr = (void *)(page_brk - size);	
    	    
	    page_t head = {(uintptr_t)ptr, (uintptr_t)ptr+sizeof(page_t), FREE, count, (page_t*)page_brk};

	    memcpy(ptr, &head, sizeof(page_t));
//		printf("ptr: %x\n", (uintptr_t)ptr);
//		page_t* tmp = (page_t*)ptr;
//		printf("ptr->next: %x\n", (uintptr_t)(tmp->next));

		count ++;

		if(page_brk >= (uintptr_t)_heap.end) {
		    break;
		}
	}
	return count;
}

//static uintptr_t brk = 0;

/*
void smash_bind() {
   int count = 0;
   
   header_t* upper = head.next; 

   while(upper != NULL) {
       count ++;
	   upper = upper->next;
   }

   upper = head.next;

   if(count < 2) {
       return;
   }

   else{
       WHile(upper != NULL) {
           header_t* down=upper->next;
		   while(down != NULL) {
		        if()          
		   }
       }
   }

}
*/

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
  lock();
  if(size == 4*KiB) {

	  page_t* page = page_head;

      while(page->status != FREE) {
	      page = page->next;
	  }

	  if((uintptr_t)page == (uintptr_t)_heap.end) {
#ifdef CUR
          printf("Alloc Failed!\n");
#endif
		  unlock();
	      return NULL;
	  }

	  memset((void*)((uintptr_t)page+sizeof(page_t)), MAGIC, size-sizeof(page_t)-1);
	  memset((void*)(uintptr_t)page+4*KiB-1, MARK, 1);

	  page->status = FULL;

#ifdef CUR
          printf("The whole page:%d alloced!\n", page->No);
#endif

	  //assert((uintptr_t)page % size == 0);
	  unlock();
	  return (void*)page;

  } 	

  else {
	  if((size & (size-1)) != 0) {
	      size = bound_aligned(size);
	  }

      page_t* page = page_head;
	  while(page->status == FULL || page->ptr + 4*KiB <= page->brk+size) {
	      page = page->next;       
	  }

	  if((uintptr_t)page == (uintptr_t)_heap.end) {
#ifdef CUR
          printf("Alloc Failed!\n");
#endif
		  unlock();
	      return NULL;
	  }
      
      page->brk = ROUNDUP(page->brk, size) + size;
	  uintptr_t ptr = page->brk-size;
	  alloc_chk((void*)ptr, size);
	  memset((void*)ptr, MAGIC, size-1);
	  memset((void*)ptr+size-1, MARK, 1);
      
	  if(page->brk == page->ptr + 4*KiB) {
	      page->status = FULL;
	  }
	  else {
	      page->status = USED;
	  }

#ifdef CUR
      printf("The space in page %d alloced!\n", page->No);
#endif
	  //assert((uintptr_t)ptr % size == 0);
      unlock();
	  return (void*)ptr;
     
  }
	/*
	lock();
#ifdef CUR
	printf("[#LOCK]:CPU:%d Alloc * Acquired!\n", _cpu());
#endif
	//assert(brk <= (uintptr_t)_heap.end);
	//assert((uintptr_t)(brk-size) % size == 0);
	//uintptr_t capacity = (uintptr_t)_heap.end - head.brk;

	void* ptr = NULL;
	header_t header_ptr;  //用于分配出的空间的信息

	//else {
	
        header_t tmp = head; //用于保存空闲空间信息

		memset((void*)(head.brk-sizeof(header_t)), VALID, sizeof(header_t));
		alloc_chk((void*)(head.brk-sizeof(header_t)), sizeof(header_t));

		head.brk = head.brk?
			ROUNDUP(head.brk, size) + size :
			(uintptr_t)_heap.start + size;
		ptr = (void *)(head.brk - size);

		header_ptr.ptr = (uintptr_t)ptr;
		header_ptr.size = size;
		header_ptr.next = NULL;
		header_ptr.type = PTR_SPACE;

        memcpy((void*)((uintptr_t)ptr-sizeof(header_t)), &header_ptr, sizeof(header_t));

		//printf("header_ptr at:%d\n",(uintptr_t)ptr-sizeof(header_t));
		
		tmp.brk = head.brk+sizeof(header_t);
		tmp.size = (uintptr_t)_heap.end - tmp.brk;
		tmp.type = FREE_SPACE;
		tmp.next = head.next;

		memcpy((void*)(uintptr_t)head.brk, &tmp, sizeof(header_t));

		head.brk = tmp.brk;
		
		assert((uintptr_t)ptr % size == 0);
		alloc_chk(ptr, size);
		memset(ptr, MAGIC, size-1);
    	void* end = (void*)((uintptr_t)ptr+size-1);
		memset(end, MARK, 1);
	//}
	
	unlock();
#ifdef CUR
	printf("[#LOCK]:CPU:%d Alloc * Released!\n", _cpu());
#endif
	return ptr;
*/
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
	lock();
	if(ptr == NULL) {
	    unlock();
		return;
	}
	else if((uintptr_t)ptr % (4*KiB) == 0) {
	    page_t* page = page_head;  
		while((uintptr_t)ptr != (uintptr_t)page) {
		   page = page->next; 
		}
		assert((uintptr_t)page == (uintptr_t)ptr && (uintptr_t)page <= (uintptr_t)_heap.end);
        memset((void*)((uintptr_t)ptr+sizeof(page_t)), VALID, 4*KiB-sizeof(page_t)); 		
        page->status = FREE;  
#ifdef CUR
        printf("The whole page %d was freed!\n", page->No);
#endif
		unlock();
		return;
	}
	else {
	   uintptr_t page = (uintptr_t)ptr - ((uintptr_t)ptr % 4*KiB);
	   //uintptr_t page_start = (page_t*)page->ptr;
	   uintptr_t brk = ((page_t*)page)->brk;
	   uintptr_t size = 0;
	   char* tmp = (char*)ptr;
	   while(*tmp == MAGIC) {
	       *tmp = VALID;
		   size ++;
		   tmp ++;

	   }
	   assert(*tmp == MARK);
	   *tmp =  VALID;
	   tmp ++;
	   size ++;
	   if((uintptr_t)tmp == brk) {
	      ((page_t*)page)->brk = brk - size;	   
	   }
	   if(((page_t*)page)->brk > ((page_t*)page)->ptr + sizeof(page_t)) {

	       tmp = (char*)(((page_t*)page)->brk);
	       tmp --;
	       while(*tmp == VALID && (uintptr_t)tmp >= page+sizeof(page_t)) {
			   tmp --;
		   }
	       tmp ++;
	       ((page_t*)page)->brk = (uintptr_t)tmp;
	   }
	   if(((page_t*)page)->brk == ((page_t*)page)->ptr + sizeof(page_t)) {
	       ((page_t*)page)->status = FREE;
	   }
	   else {
	       ((page_t*)page)->status = USED;
	   }
#ifdef CUR
        printf("The space in page %d was freed!\n", ((page_t*)page)->No);
#endif
	   unlock();
	   return;
	}
	/*
	lock();
#ifdef CUR
	printf("[#LOCK]:CPU:%d Free * Acquired!\n", _cpu());
#endif

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
	    assert(0);
	}

		
	free_chk((uintptr_t)ptr, end);

    tmp = (char*)((uintptr_t)ptr-sizeof(header_t));
	
	tmp --;
	while(*tmp == VALID && (uintptr_t)tmp >= (uintptr_t)_heap.start) {
		//printf("tmp:%p\n", (uintptr_t)tmp);
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

	
//	header_t* index = &head;
//
//	while(index->next != NULL) {
//	    printf("addr:%p\n", index->brk);
//		index = index->next;
//	}
//	printf("addr:%p\n", index->brk);
//  

	unlock();
#ifdef CUR
	printf("[#LOCK]:CPU:%d Free * Released!\n", _cpu());
#endif
	*/
}


static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)_heap.end - (uintptr_t)_heap.start);
//  printf("Size of header_t: %d\n", sizeof(header_t));
//  printf("Size of size_t: %d\n", sizeof(size_t));
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, _heap.start, _heap.end);
  locked = 0;

  memset((void*)_heap.start, VALID, pmsize);
  page_brk = (uintptr_t)_heap.start;
  page_head = (page_t*)page_brk;
  //uintptr_t page_nums = page_construct();
  page_construct();
  //printf("Got %d pages of heap!\n", page_nums);
  //head.next =  NULL;
  //memcpy((void*)_heap.start, (void*)(&head), sizeof(header_t));
  
//  printf("Initial brk at :%d\n", head.brk);

  
  //brk = head.brk;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
