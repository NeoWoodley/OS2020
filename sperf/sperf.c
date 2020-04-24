#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char** environ;
//#define DEBUG
//#define TEST
/*
   int execve(
       const char *filename,
	   char *const argv[],
	   char *const envp[]
   );

   strace -T 显示系统调用所花时间
*/

char read_buf[1024];

char left_buf[256];

char* upper_bound = &read_buf[1023];

char line_buf[256];

char* topfive[5];

struct item_t {
    char name[64];
	double time;
};

double total_time = 0;

double timeset[128];

typedef struct item_t item_t;

item_t libitem[128];

int end = 0;

int cmp_descend(const void* a, const void* b) {
	double* A = (double*)a;
	double* B = (double*)b;

	return *B > *A ? 1 : -1;
}

int bound_test(char* tmp) {
    uintptr_t ptr = (uintptr_t)tmp;
    uintptr_t end = (uintptr_t)&read_buf[1023];	

	int ret = end >= ptr ? 0 : 1;

	return ret;
}

void lib_init() {
    for(int i = 0; i < 128; i ++) {
		timeset[i] = 0;
	    libitem[i].time = 0;
		memset(libitem[i].name, '\0', 64);
	}
	memset(left_buf, '\0', 256);
}

void linebufsmash() {
    for(int i = 0; i < 256; i ++) {
	    line_buf[i] = '\0';
	}
}

void leftbufsmash() {
    for(int i = 0; i < 256; i ++) {
	    left_buf[i] = '\0';
	}
}

void eofsmash() {
    intptr_t len = strlen(read_buf);
	//printf("len: %ld\n", len);
	if(read_buf[0] == '\n') {
	    read_buf[0] = 32;
	}
	for(intptr_t i = 1; i < len; i++) {
	    if(read_buf[i] == '\n' && read_buf[i-1] != '>') {
		    read_buf[i] = 32;
		}
	}
}

/*
int special_strcmp(char *str1, char* str2, int len) {
    size_t len_str1 = strlen(str1);
    size_t len_str2 = strlen(str2);

	int compare_len = len_str1;
	if(len_str2 < compare_len) {
		compare_len = len_str2;	    
	}
	if(len < compare_len) {
	    compare_len = len;
	}

	for(int i = 0; i < compare_len; i ++) {
	    if( str1[i] > str2[i] ) {
		    return 1;
		}

		if( str1[i] < str2[i] ) {
		    return -1;
		} 
	}

	return 0;
}
*/

int readline() {

#ifdef TEST
	printf("Readline() Begin!\n");
#endif

    char exit[6] = "exit_";

#ifdef TEST
	printf("Is here?\n");
#endif

    linebufsmash();

#ifdef TEST
    printf("Or here?\n");
#endif

    char *tmp = read_buf;

#ifdef TEST
    printf("And here?\n");
#endif

    while(*tmp == '\0' && bound_test(tmp) == 0) {
        tmp ++;
    }

#ifdef TEST
	printf("Maybe here?\n");
#endif

    int i = 0;
    //printf("tmp:%ld\n", (uintptr_t)tmp);
    //printf("read_buf:%ld\n", (uintptr_t)&read_buf[10239]);

    while(*tmp != '\n' && i < 256 && bound_test(tmp) == 0) {
        line_buf[i] = *tmp;
	    *tmp = '\0';
	    i ++;
	    tmp ++;
    }

#ifdef TEST
	printf("Then here?\n");
#endif


	//printf("%s\n", line_buf);

	if(*tmp == '\n') {
	    *tmp = '\0';
	}

	if(strncmp(line_buf, exit, 5) == 0) {

#ifdef TEST
	    printf("Readline() End!\n");
#endif
	    return 1;
	}

	if(bound_test(tmp) == 1) {
	//	printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
	    strcpy(left_buf, line_buf);
	    return 3;
	}

	if(left_buf[0] != '\0') {
	    strcat(left_buf, line_buf);
		strcpy(line_buf, left_buf);
		leftbufsmash();
		return 0;
	}

#ifdef TEST
//	    printf("red_buf content:%s\n", line_buf);
	    printf("Readline() End!\n");
#endif

	//	printf("?\n");
	    return 0;
}

void search_insert(item_t *item) {
#ifdef DEBUG
	    printf("Search_insert() Begin!\n");
#endif
	for(int i = 0; i < end; i ++) {
		if(strcmp(item->name, libitem[i].name) == 0) {
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

char* index_name(double time) {
	assert(time != 0);
	char *ret = NULL;
    for(int i = 0; i < 128; i ++) {
	    if(libitem[i].time == 0) {
		    break;
		}
		if(time == libitem[i].time) {
		    ret = libitem[i].name; 
			break;
		}
	}

	assert(ret != NULL);
    
	return ret;
}

void info_extract() {
//	printf("%s\n", line_buf);
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
	   tmp->name[i] = *buf;
	   i ++;
	   buf ++;
	}
	tmp->name[i] = '\0';

#ifdef DEBUG
    printf("Name Got!\n");
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
    printf("Time string Got!\n");
#endif

	double base = 0.000001;
	char *mark = time;
	double factor = 0.0;

    while(*mark == '0' || *mark == '.') {
		*mark ++;	
	}

#ifdef DEBUG
    printf("Digit Found! %s\n", mark);
#endif

	while(*mark != '\0') {
		factor *= 10;
	    factor += (*mark - '0');
		mark ++;
	}

#ifdef DEBUG
    printf("Digit Got!\n");
#endif

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
  char exe_name[8] = "/strace";
  char args[64] = "";
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
	  int trash = open("/dev/null", O_RDWR);
	  dup2(trash, fileno(stdout));
	  close(trash);
	  //close(fildes[0]);
	  dup2(fildes[1], fileno(stderr));
	  char* pwd = strtok(path, ":");
	  memset(args, '\0', 64);
	  strcpy(args, pwd);
	  strcat(args, exe_name);
      //子进程，执行strace命令
	  while(execve(args, exec_argv, exec_envp) == -1) {
	      pwd = strtok(NULL, ":");
	      memset(args, '\0', 64);
	      strncpy(args, pwd, 64*sizeof(char));
	      strcat(args, exe_name);
	  }
	  //不应该执行此处代码，否则execve失败，出错处理
  }
  else {
	  sleep(1);
	  close(fildes[1]);
	  int len = 0;
	  //for(int i = 0; i < 1; i ++) {
	  while(1){
		  len = read(fildes[0], read_buf, 1024);
		  if(len == 0) {
		      break;
		  }
		  printf("%s\n", read_buf);
		  //if(read(fildes[0], read_buf, 10240) == 0) {
		  //  break;		  
		}
	      //sleep(1);
		  //read(fildes[0], read_buf, 10240);
		  //printf("============================================================================================\n");
	  }

	  /*
	  int len = read(fildes[0], read_buf, 10240);
	  printf("==================%d===================\n", len);
	  printf("%s", read_buf);
      */
	  //exit(0);

	  /*
	  unsigned read_length = 0;
	  read_length = read(fildes[0], read_buf, CAP);
	  assert(read_length != 0);
	  */
	  //printf("Len:%d\n", read_length);
	  //printf("%s\n", read_buf);
	  /*
	  for(int i = 0; i < 500; i ++ ) {
		  sleep(1);
		  read_length = read(fildes[0], read_buf, 10240);
		  printf("____________________%u________________________\n", read_length);
		  printf("%s\n", read_buf); 
		  printf("++++++++++++++++++++++++++++++++++++++++++++++\n");
	  } 
	  printf("^*^&*^*&^*&^&&$&$^&*^*(*&(&)&)(&()&*^&*&$^$^\n");
	  exit(0);
	  */
	  //regmatch_t get_match;
	  //regex_t reg;

	  //eofsmash();

	  //printf("%s\n", read_buf);

	  /*
	  printf("_____________________________________________\n");
	  printf("%s\n", read_buf);
	  printf("_____________________________________________\n");
	  printf("%d\n", read_length);
      */

	  /*+++++++++++++++++++++++++++++++++++++++++
	  int read_line = 0;
	  char* name;
	  double ratio;
	  int count = 0;
	  while(1) {
//		  printf("Line_buf:%s\n", line_buf);
		  read_line = readline();
		  if(read_line == 0) {
	          info_extract();
		  }
		  else if(read_line == 3) {
			  total_time = 0;
	          for(int i = 0; i < 128; i ++) {
		          if(libitem[i].time == 0.0) {
		              break;
		          }
				  //Total_time需要刷新
		          total_time += libitem[i].time;
		          timeset[i] = libitem[i].time;
	              //printf("Name: %s, Time elapsed: %f\n", libitem[i].name, libitem[i].time);
	          }

	          qsort(timeset, 128, sizeof(timeset[0]), cmp_descend);
			  count ++;

			  printf("Time #%d\n", count);
	          for(int i = 0; i < 5; i ++) {
                  name = index_name(timeset[i]);
                  ratio = ((timeset[i] / total_time) * 100);
		          printf("%s (%f%%)\n", name, ratio);
	          }
	          printf("====================\n");
	          //printf("====%s=========%f=======\n", name, ratio);
	          for(int i = 0; i < 80; i ++) {
	              printf("%c", '\0');
	          }
			  fflush(stdout);

			  sleep(1);
			  read_length = read(fildes[0], read_buf, 10240);
	          eofsmash();
	          //printf("%s\n", read_buf);
		      continue;
		  }
		  else {
		      break;
		  }
	  }
	  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	  /*
	  for(int i = 0; i < 128; i ++) {
		  if(libitem[i].time == 0.0) {
		      break;
		  }
		  total_time += libitem[i].time;
		  timeset[i] = libitem[i].time;
	      //printf("Name: %s, Time elapsed: %f\n", libitem[i].name, libitem[i].time);
	  }

	  qsort(timeset, 128, sizeof(timeset[0]), cmp_descend);
*/
	  /*
	  for(int i = 0; i < 128; i ++) {
	      if(timeset[i] == 0) {
		      break;
		  }
		  printf("%f\n", timeset[i]);
	  }
	  */
	  /*

	  char* name;
	  double ratio;
	  for(int i = 0; i < 5; i ++) {
          name = index_name(timeset[i]);
          ratio = (timeset[i] / total_time) * 100;
		  printf("Time #%d\n%s (%f%%)\n", (i+1), name, ratio);
	  }
	  printf("====================\n");
      */




	  //printf("%ld\n", read_length);
	  //fprintf(stderr, "%s\n", &read_buf[0]);
	  //父进程，读取strace输出并统计
 // }

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
