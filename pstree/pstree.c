#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>   //char *strstr(const char *haystack, const char *needle)
#include <dirent.h>

int find_num(char *str) {
    int return_code;

    if(strchr(str, '0') || strchr(str, '1') || strchr(str, '2') || strchr(str, '3') || strchr(str, '4') || strchr(str, '5') || strchr(str, '6') || strchr(str, '7') || strchr(str, '8') || strchr(str, '9') == NULL) {
        return_code = 0;
    }
    else {
        return_code = 1; 
    }
    return return_code;
}

int main(int argc, char *argv[]) {

  DIR* dir;
  struct dirent* entry;
  //int files = 0;

  dir = opendir("/proc");
  assert(dir != NULL);

  char _V[13]="-V --version";
  char _n[18]="-n --numeric-sort";
  char _p[15]="-p --show-pids";

  pid_t curr_pid = getpid();
  printf("PID:%d\n",curr_pid);

  while((entry=readdir(dir))) {
        if(find_num(entry->d_name) == 0) {
            printf("Not a process file!\n");
        }
        else {
            printf("Process!\n");
        }
//      printf("File %d: Name: %s Type:%d\n", files, entry->d_name, entry->d_type);  
//      files ++;
  }

  closedir(dir);

  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  return 0;
}
