#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <regex.h>

/*
   int execve(
       const char *filename,
	   char *const argv[],
	   char *const envp[]
   );

   strace -T 显示系统调用所花时间
*/


char read_buf[10240];

struct item_t {
    char name[64];
	double time;
};

typedef struct item_t item_t;

item_t libitem[128];

void lib_init() {
    for(int i = 0; i < 128; i ++) {
	    libitem[i].time = 0;
		memset(libitem[i].name, '\0', 64);
	}
}

void eofsmash() {
    intptr_t len = strlen(read_buf);
	printf("len: %ld\n", len);
}

int main(int argc, char *argv[]) {

  lib_init();

  char *exec_argv[argc + 2];
  exec_argv[0] = "strace";
  exec_argv[1] = "-T";
  exec_argv[argc + 2 - 1] = NULL;

  for(int i = 0; i < argc-1; i ++) {
      exec_argv[i+2] = argv[i+1];
  }
  
  char PATH[128] = "";
  char *path = NULL; 
  char path_name[6] = "PATH=";
  strcpy(PATH, path_name);
  path = getenv("PATH");
  strcat(PATH, path);

  char *exec_envp[] = { PATH, NULL, };

  int fildes[2];

  if(pipe(fildes) != 0) {
	  printf("Pipe failed!\n");
      assert(0);
  }
  pid_t pid = fork();
  if(pid == 0) {
	  close(fildes[0]);
	  close(2);
	  close(1);
	  dup2(fildes[1], 2);
      //子进程，执行strace命令
	  execve("/usr/bin/strace", exec_argv, exec_envp);
	  //不应该执行此处代码，否则execve失败，出错处理
  }
  else {
	  sleep(1);
	  close(fildes[1]);
	  intptr_t read_length = read(fildes[0], read_buf, 10240);

	  //regmatch_t get_match;
	  //regex_t reg;

	  eofsmash();
	  //printf("%ld\n", read_length);
	  //fprintf(stderr, "%s\n", &read_buf[0]);
	  //父进程，读取strace输出并统计
  }

//  execve("/usr/bin/strace", exec_argv, exec_envp);
//  perror(argv[0]);
//  exit(EXIT_FAILURE);
//  char *exec_envp[] = { 0, NULL, };

  /*
  execve("/usr/bin/strace", exec_argv, exec_envp);
  perror(argv[0]);
  exit(EXIT_FAILURE);
  */
  /*
  printf("----------argc:%d------------\n",argc);
  for(int i = 0; i < argc - 1; i ++) {
      printf("i:%d\t%s\n",i, exec_argv[i]);
  }
  printf("-----------------------\n");
	return 0;
*/
/*
  char *exec_argv[] = { "strace", "ls", NULL, };
  char *exec_envp[] = { "PATH=/bin", NULL, };
//  execve("strace",          exec_argv, exec_envp);
//  execve("/bin/strace",     exec_argv, exec_envp);
  execve("/usr/bin/strace", exec_argv, exec_envp);
  perror(argv[0]);
  exit(EXIT_FAILURE);
*/
  return 0;
}
