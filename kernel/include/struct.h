typedef struct task {
	struct {
	    const char *name;
		struct task *next;
		_Context *context;
	};
	uint8_t stack[4096];
} task_t;

task_t *Current = NULL, task[] = {
  { .name = "a" },
  { .name = "b" },
  { .name = "c" },
};


struct spinlock {
	char name[64];
	volatile intptr_t locked;
};

struct semaphore {
	int value;
	int cond;
	intptr_t locked;

};
