#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#define --version -V
#define --show-pids -p
#define --numeric-sort -n

int main(int argc, char *argv[]) {
  pid_t curr_pid = getpid();
  printf("PID:%d\n",curr_pid);

  

  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  return 0;
}
