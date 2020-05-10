#include <common.h>

#define VALID '0'
#define MAGIC '1'
#define MARK '9'

#define KiB (1 << 10)

//#define LACK (((uintptr_t)_heap.end-(uintptr_t)_heap.start) >> 2)

#define CUR
//#define DET
//#define PRE
//#define SPA

#define PTR

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

enum status_type {

	FREE = 1,
	USED,
	FULL,

};

struct page_t {
    uintptr_t ptr;
    uintptr_t brk;

	uintptr_t status;
	uintptr_t No;

	struct page_t* next;
	uint8_t fence[8];
};

typedef struct page_t page_t;

page_t *page_head;


uintptr_t pmsize;

uintptr_t bound_aligned(size_t size) {
    uintptr_t round = 1;
	while (size > round) {
	    round = (round << 1);
	}
	return round;
}

void show_page_head(page_t* ptr) {
	printf("==================================\n");
    printf("{@}Page ptr:%p\n", ptr->ptr);
    printf("{@}Page brk:%p\n", ptr->brk);
    printf("{@}Page status:%p\n", ptr->status);
    printf("{@}Page no:%d\n", ptr->No);
    printf("{@}Page next:%p\n", ptr->next);
	for(int i = 0; i < 8; i ++) {
	    printf("%c", ptr->fence[i]);
	}
	printf("\n==================================\n");
}

uintptr_t page_construct() {
	uintptr_t count = 0;
	uintptr_t size = 4 * KiB;
	for(int i = 0 ; i < (pmsize / (4*KiB)); i ++) {
		if(page_brk >= (uintptr_t)_heap.end) {
		    break;
		}
	    page_brk = page_brk?
		    ROUNDUP(page_brk, size) + size :
		    (uintptr_t)_heap.start + size;
	    void* ptr = (void *)(page_brk - size);	

		if(page_brk == (uintptr_t)_heap.end) {
	        page_t head = {(uintptr_t)ptr, (uintptr_t)ptr+sizeof(page_t), FREE, count, NULL, {'#','#','#','#','#','#','#','#'}};
	        memcpy(ptr, &head, sizeof(page_t));
		    memset(ptr+sizeof(page_t), VALID, 4*KiB-sizeof(page_t));
		}
    	    
		else{
	        page_t head = {(uintptr_t)ptr, (uintptr_t)ptr+sizeof(page_t), FREE, count, (page_t*)page_brk, {'#','#','#','#','#','#','#','#'}};
	        memcpy(ptr, &head, sizeof(page_t));
		    memset(ptr+sizeof(page_t), VALID, 4*KiB-sizeof(page_t));
		}

		count ++;

	}
#ifdef PRE
    printf("Page Construct Done!\n");
#endif
	return count;
}

void alloc_chk_before(void* ptr, size_t size) { //用于检查是否这些要分配的区域都是VALID的
	char* tmp = (char*)ptr;
	for(int i = 0; i < size; i ++) {
	    //printf("%c",*(tmp+i));
	    assert((*(tmp+i)) == VALID);
	}
	//printf("\n");
}

void alloc_chk_after(void* ptr, size_t size) { //用于检查是否这些要分配的区域都是VALID的
	char* tmp = (char*)ptr;
	for(int i = 0; i < size - 1; i ++) {
	    printf("%c",*(tmp+i));
	    assert((*(tmp+i)) == MAGIC);
	}
	assert((*(tmp+size-1)) == MARK);
	printf("%c\n", *(tmp+size-1));
}

void free_chk_after(uintptr_t begin, uintptr_t end) { //用于检查这些释放了的区域都是有效的
    char* tmp = (char*)begin;
	for(int i = 0; i <= end-begin; i ++) {
	    assert((*(tmp+i)) == VALID);
		printf("%c",*(tmp+i));
	}
	printf("\n");
}

static void *kalloc(size_t size) {
  lock();
#ifdef CUR
  printf("Lock acquired by #CPU:%d in alloc | LINE:%d\n", _cpu(), __LINE__);
#endif
  if(size == 4*KiB) {

	  page_t* page = page_head;

      while(page->status != FREE && (uintptr_t)page < (uintptr_t)_heap.end) {
	      page = page->next;
	  }

	  if((uintptr_t)page >= (uintptr_t)_heap.end) {
#ifdef CUR
          printf("Alloc Failed!\n");
          printf("Lock released by #CPU:%d in alloc | LINE:%d\n", _cpu(), __LINE__);
#endif
		  unlock();
	      return NULL;
		      
     }

	  memset((void*)((uintptr_t)page+sizeof(page_t)), MAGIC, size-sizeof(page_t)-1);
	  memset((void*)(uintptr_t)page+4*KiB-1, MARK, 1);

	  page->status = FULL;

#ifdef PTR
    printf("[#Alloced](CPU%d) ptr:%p | size:%d\n",_cpu(), page, size);
#endif

#ifdef CUR
      printf("The whole page:%d alloced!\n", page->No);
      printf("Lock released by #CPU:%d in alloc | LINE:%d\n", _cpu(), __LINE__);
#endif

	  unlock();
	  return (void*)page;

  } 	

  else {
	  if((size & (size-1)) != 0) {
	      size = bound_aligned(size);
	  }

      page_t* page = page_head;
	  while((page->status == FULL || page->ptr + 4*KiB <= page->brk+size) && ((uintptr_t)page < (uintptr_t)_heap.end)) {
	      page = page->next;       
	  }

	  if((uintptr_t)page >= (uintptr_t)_heap.end) {
#ifdef CUR
          printf("Alloc Failed!\n");
          printf("Lock released by #CPU:%d in alloc | LINE:%d\n", _cpu(), __LINE__);
#endif
		  unlock();
	      return NULL;
	  }
	  uintptr_t backup_brk = page->brk;
      
      page->brk = ROUNDUP(page->brk, size) + size;
	  uintptr_t ptr = page->brk-size;
	  if(page->brk >= page->ptr+4*KiB) {
		  page->brk = backup_brk;
	      unlock();
		  return NULL;
	  }

	  alloc_chk_before((void*)ptr, size);
	  memset((void*)ptr, MAGIC, size-1);
	  memset((void*)ptr+size-1, MARK, 1);
	  alloc_chk_after((void*)ptr, size);
      
	  if(page->brk >= page->ptr + 4*KiB - 32) {
	      page->status = FULL;
	  }
	  else {
	      page->status = USED;
	  }
#ifdef PTR
    printf("[#Alloced](CPU%d) ptr:%p | size:%d\n",_cpu(), ptr, size);
#endif

#ifdef CUR
      printf("%d space in page %d alloced! | Original brk:%p Now brk:%p\n", size, page->No, backup_brk, page->brk);
      printf("Lock released by #CPU:%d in alloc | LINE:%d\n", _cpu(), __LINE__);
#endif
      unlock();
	  return (void*)ptr;
     
  }
}

static void kfree(void *ptr) {
	lock();
	//printf("%c\n", *(char*)ptr);
	//assert(*(char*)ptr == MARK || *(char*)ptr == MAGIC || ((uintptr_t)ptr%(4*KiB)==0));

#ifdef CUR
    printf("Lock acquired by #CPU:%d in free | LINE:%d\n", _cpu(), __LINE__);
	printf("Ptr to br freed: %p | LINE:%d\n", (uintptr_t)ptr, __LINE__);
#endif
	if(ptr == NULL) {
#ifdef DET
		printf("[#]Ptr is NULL\n");
#endif
#ifdef CUR
        printf("Lock released by #CPU:%d in free | LINE:%d\n", _cpu(), __LINE__);
#endif
	    unlock();
		return;
	}
	else if((uintptr_t)ptr % (4*KiB) == 0) {
#ifdef DET
		printf("[#]This is a whole page\n");
#endif
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

#ifdef PTR
    printf("[#Freed](CPU%d) ptr:%p\n",_cpu(), ptr);
#endif

#ifdef CUR
        printf("Lock released by #CPU:%d in free | LINE:%d\n", _cpu(), __LINE__);
#endif
		unlock();
		return;
	}
	else {
#ifdef DET
		printf("[#]This is a < one page\n");
#endif
	   uintptr_t page = (uintptr_t)ptr - ((uintptr_t)ptr % (4*KiB));
	   
#ifdef DET
	   page_t* page_ptr = (page_t*)page;
	   //show_page_head(page_ptr);
#endif

	   //uintptr_t page_start = (page_t*)page->ptr;
	   uintptr_t brk = ((page_t*)page)->brk;
	   uintptr_t size = 0;
	   char* tmp = (char*)ptr;
	   if(*tmp == VALID) {
#ifdef PTR
    printf("(*Error)(CPU%d) ptr:%p\n",_cpu(), ptr);
#endif
	       unlock();
		   return;
	   }
	   //printf("%c\n", *tmp);
	   while(*tmp == MAGIC) {
	       *tmp = VALID;
		   size ++;
		   tmp ++;
	   }
#ifdef DET
		printf("[*]Magic cleared\n");
#endif
	   //printf("%c\n", *(tmp-1));
	   //printf("%c\n", *tmp);
	   //printf("%c\n", *(tmp+1));
	   assert(*tmp == MARK);
	   *tmp =  VALID;
	   free_chk_after((uintptr_t)ptr, (uintptr_t)tmp);
	   tmp ++;
	   size ++;
	   if((uintptr_t)tmp == brk) {
	      ((page_t*)page)->brk = brk - size;	   
	//	  printf("brk: %d\n",((page_t*)page)->brk);
#ifdef DET
		  printf("[*]brk adjusted!\n");
#endif
	   }
	   if(brk > ((page_t*)page)->ptr + sizeof(page_t)) {
#ifdef DET
		   printf("[*]before brk adjusted pro!\n");
#endif

	       tmp = (char*)(((page_t*)page)->brk);

	       tmp --;

	//	   printf("Wait Brk:%p\n", ((page_t*)page)->brk);
	       //while(*tmp == VALID && ((uintptr_t)tmp >= page+sizeof(page_t))) {
	       while(*tmp == VALID && *tmp != '#') {
	//	       printf("?\n");
			   tmp --;
		   }
	//	   printf("Wait Again\n");
	       //printf("\n");
	       tmp ++;
	       ((page_t*)page)->brk = (uintptr_t)tmp;
#ifdef DET
		  printf("[*]brk adjusted pro!\n");
#endif
	   }
	   if(((page_t*)page)->brk == ((page_t*)page)->ptr + sizeof(page_t)) {
	       ((page_t*)page)->status = FREE;
#ifdef DET
		   printf("[*]status adjusted!\n");
#endif
	   }
	   else {
	       ((page_t*)page)->status = USED;
#ifdef DET
		   printf("[*]status adjusted!\n");
#endif
	   }
#ifdef CUR
        printf("The space in page %d was freed! | Old brk:%p Now brk:%p\n", ((page_t*)page)->No, brk, ((page_t*)page)->brk);
#endif

#ifdef PTR
    printf("[#Freed](CPU%d) ptr:%p\n",_cpu(), ptr);
#endif

#ifdef CUR
       printf("Lock released by #CPU:%d in free | LINE:%d\n", _cpu(), __LINE__);
#endif
	   unlock();
	   return;
	}
}

static void pmm_init() {
  pmsize = ((uintptr_t)_heap.end - (uintptr_t)_heap.start);
//  printf("Size of header_t: %d\n", sizeof(header_t));
//  printf("Size of size_t: %d\n", sizeof(size_t));
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, _heap.start, _heap.end);
  locked = 0;

  memset((void*)_heap.start, VALID, pmsize);
  page_brk = (uintptr_t)_heap.start;
  page_head = (page_t*)page_brk;
  //uintptr_t page_nums = page_construct();
  page_construct();
  /*
  page_t* tnp = page_head; 
  while(tnp != NULL) {
	  show_page_head(tnp);
	tnp = tnp->next;  
  }
  */
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
