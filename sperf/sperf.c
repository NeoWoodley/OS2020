#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>

#define DEBUG

/*
   int execve(
       const char *filename,
	   char *const argv[],
	   char *const envp[]
   );

   strace -T 显示系统调用所花时间
*/

char read_buf[10240];

char line_buf[256];

struct item_t {
    char name[64];
	double time;
};

typedef struct item_t item_t;

item_t libitem[128];

int end = 0;

void lib_init() {
    for(int i = 0; i < 128; i ++) {
	    libitem[i].time = 0;
		memset(libitem[i].name, '\0', 64);
	}
}

void linebufsmash() {
    for(int i = 0; i < 256; i ++) {
	    line_buf[i] = '\0';
	}
}

void eofsmash() {
    intptr_t len = strlen(read_buf);
	//printf("len: %ld\n", len);
	for(intptr_t i = 1; i < len; i++) {
	    if(read_buf[i] == '\n' && read_buf[i-1] != '>') {
		    read_buf[i] = 32;
		}
	}
}

int my_strcmp(char *str1, char* str2) {
    size_t len_str1 = strlen(str1);
    size_t len_str2 = strlen(str2);

	if(len_str1 > len_str2) {
	    return 1;
	}

	else if(len_str1 < len_str2) {
	    return -1;
	}

	else {
		for(int i = 0; i < len_str1; i ++) {
		    if( str1[i] > str2[i] ) {
			    return 1;
			}

			if( str1[i] < str2[i] ) {
			    return -1;
			} 
		}

		return 0;
	}
}

int readline() {

#ifdef DEBUG
	printf("Readline() Begin!\n");
#endif

    char exit[6] = "exit_";

    linebufsmash();
	char *tmp = read_buf;
	while(*tmp == '\0') {
	    tmp ++;
	}
	int i = 0;
	while(*tmp != '\n') {
	    line_buf[i] = *tmp;
		*tmp = '\0';
		i ++;
		tmp ++;
	}
	*tmp = '\0';

	if(my_strcmp(read_buf, exit) == 0) {

#ifdef DEBUG
	    printf("Readline() End!\n");
#endif
	    return 1;
	}

#ifdef DEBUG
//	printf("red_buf content:%s\n", line_buf);
	printf("Readline() End!\n");
#endif

	return 0;
}

void search_insert(item_t *item) {
#ifdef DEBUG
	    printf("Search_insert() Begin!\n");
#endif
	for(int i = 0; i < end; i ++) {
		if(my_strcmp(item->name, libitem[i].name) == 0) {
			libitem[i].time += item->time;

#ifdef DEBUG
	    printf("Search_insert() End!\n");
#endif
			return;
		}
	}

	strcpy(libitem[end].name, item->name);
	libitem[end].time = item->time;
    
	end ++;

#ifdef DEBUG
	    printf("Search_insert() End!\n");
#endif
}

void info_extract() {
#ifdef DEBUG
	    printf("Info_extract() Begin!\n");
#endif
    item_t* tmp = (item_t*)malloc(sizeof(item_t));
#ifdef DEBUG
//	    printf("Malloc Success!\n");
#endif
	char *buf = line_buf;
#ifdef DEBUG
//	    printf("Buf Got!\n");
#endif
	int i = 0;
	while(*buf != '(') {
	   printf("?\n");
	   tmp->name[i] = *buf;
	   i ++;
	   buf ++;
	}
	tmp->name[i] = '\0';

#ifdef DEBUG
    printf("Name Got!");
#endif

	while(*buf != '<') {
	    buf ++;
	}
	buf ++;
    
	i = 0;
	char time[10];
	for( ; i < 10; i ++) {
	    time[i] = '\0';
	}

    i = 0;
	while(*buf != '>') {
	   time[i] = *buf;
	   buf ++;
	   i ++;
	}

#ifdef DEBUG
    printf("Time string Got!");
#endif

	double base = 0.000001;
	char *mark = time;
	double factor = 0.0;

    while(*mark == '0' || *mark == '.') {
		*mark ++;	
	}
	while(*mark != '\0') {
		factor *= 10;
	    factor += (*mark - '0');
	}

	tmp->time = factor * base;

	search_insert(tmp);

	free(tmp);
#ifdef DEBUG
	    printf("Info_extract() End!\n");
#endif
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

	  while(readline() == 0) {
		  printf("Line_buf:%s\n", line_buf);
	      info_extract();
	  }

	  for(int i = 0; i < 128; i ++) {
	      printf("Name: %s, Time elapsed: %f\n", libitem[i].name, libitem[i].time);
	  }
	  //printf("%ld\n", read_length);
	  //fprintf(stderr, "%s\n", &read_buf[0]);
	  //父进程，读取strace输出并统计
  }

//  execve("/usr/bin/strace", exec_argv, exec_envp);
//  perror(argv[0]);
//  Exit(EXIT_FAILURE);
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
