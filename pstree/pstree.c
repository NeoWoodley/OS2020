#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>   //char *strstr(const char *haystack, const char *needle)
#include <dirent.h>

int main(int argc, char *argv[]) {

  DIR* dir;
  struct dirent* entry;
  int files = 0;

  dir = opendir("/proc");
  assert(dir != NULL);

  char _V[13]="-V --version";
  char _n[18]="-n --numeric-sort";
  char _p[15]="-p --show-pids";

  pid_t curr_pid = getpid();
  printf("PID:%d\n",curr_pid);

  while((entry=readdir(dir))) {
      printf("File %d: Name: %s Type:%d\n", files, entry->d_name, entry->d_type);  
      files ++;
  }

  closedir(dir);

  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  return 0;
}
