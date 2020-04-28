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
#include <time.h>
#include <stdbool.h>

extern char** environ;
//#define DETAIL
//#define CRAZY
/*
   int execve(
       const char *filename,
	   char *const argv[],
	   char *const envp[]
   );

   strace -T 显示系统调用所花时间
*/

char read_buf[512];

//char test_buf[128];

char left_buf[512];

char* upper_bound = &read_buf[511];

char line_buf[512];

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
    uintptr_t end = (uintptr_t)&read_buf[511];	

	int ret = end >= ptr ? 0 : 1;

	return ret;
}

void lib_init() {
    for(int i = 0; i < 128; i ++) {
		timeset[i] = 0;
	    libitem[i].time = 0;
		memset(libitem[i].name, '\0', 64);
	}
	memset(left_buf, '\0', 512);
}

void linebufsmash() {
#ifdef CRAZY
    printf("[#linebufsmash] Begin!\n");
#endif
    for(int i = 0; i < 512; i ++) {
	    line_buf[i] = '\0';
	}
#ifdef CRAZY
    printf("[*linebufsmash] Over!\n");
#endif
}

void leftbufsmash() {
#ifdef CRAZY
    printf("[#linebufsmash] Begin!\n");
#endif
    for(int i = 0; i < 512; i ++) {
	    left_buf[i] = '\0';
	}
#ifdef CRAZY
    printf("[*leftbufsmash] Over!\n");
#endif
}

bool leftbufemptytest() {
    for(int i = 0; i < 512; i ++) {
	    if(left_buf[i] != '\0') {
		    return false;
		}
	}
	return true;
}

void eofsmash() {
#ifdef CRAZY
    printf("[#eofsmash] Begin!\n");
#endif
    intptr_t len = strlen(read_buf);
	if(read_buf[0] == '\n') {
	    read_buf[0] = 32;
	}
	for(intptr_t i = 1; i < len; i++) {
	    if(read_buf[i] == '\n' && read_buf[i-1] != '>') {
		    read_buf[i] = 32;
		}
	}
#ifdef CRAZY
    printf("[*eofsmash] Over!\n");
#endif
}

bool end_detect() {

	if(read_buf[strlen(read_buf)-2] != '>') {
	    return false;
	}

	else {
	    return true;
	}
}

bool terminate_detect() {

    char exit[6] = "exit_";

	if(strncmp(read_buf, exit, 5) == 0) {
	    return false;
	}

	else {
	    return true;
	}
    
}

/*
int readline() {
#ifdef CRAZY
    printf("[#readline] Begin!\n");
#endif

    char exit[6] = "exit_";

    linebufsmash();

#ifdef DETAIL
    printf("MARK 1\n");
#endif

    char *tmp = read_buf;

    while(*tmp == '\0' && bound_test(tmp) == 0) {
        tmp ++;
    }

#ifdef DETAIL
    printf("MARK 2\n");
#endif

    int i = 0;

    while(*tmp != '\n' && i < 200 && bound_test(tmp) == 0) {
        line_buf[i] = *tmp;
	    *tmp = '\0';
	    i ++;
	    tmp ++;
    }

#ifdef DETAIL
    printf("MARK 3\n");
#endif

	if(*tmp == '\n') {
	    *tmp = '\0';
	}

	printf("Temp_linebuf:%s\n", line_buf);

#ifdef DETAIL
    printf("MARK 4\n");
#endif

	if(strncmp(line_buf, exit, 5) == 0) {

#ifdef CRAZY
        printf("[*readline] Over!\n");
#endif
	    return 1;
	}

#ifdef DETAIL
    printf("MARK 5\n");
#endif

	if(bound_test(tmp) == 1) {
	    strcpy(left_buf, line_buf);
		printf("Left:%s\n", left_buf);
#ifdef CRAZY
        printf("[*readline] Over!\n");
#endif
	    return 3;
	}

#ifdef DETAIL
    printf("MARK 6\n");
#endif

	if(leftbufemptytest() == false) {

#ifdef DETAIL
    printf("MARK 7\n");
	printf("left_buf:%s\n", left_buf);
	printf("line_buf:%s\n", line_buf);
	printf("left_buf:%lu\n", strlen(left_buf));
	printf("line_buf:%lu\n", strlen(line_buf));
#endif
	    strcat(left_buf, line_buf);
		linebufsmash();
		strcpy(line_buf, left_buf);
		leftbufsmash();
#ifdef CRAZY
        printf("[*readline] Over!\n");
#endif
		return 0;
	}

#ifdef CRAZY
    printf("[*readline] Over!\n");
#endif
	return 0;
}*/

char* readline() {
    
	return NULL;
}

void search_insert(item_t *item) {
#ifdef CRAZY
    printf("[#search_insert] Begin!\n");
#endif
	for(int i = 0; i < end; i ++) {
		if(strcmp(item->name, libitem[i].name) == 0) {
			libitem[i].time += item->time;

#ifdef CRAZY
            printf("[*search_insert] Over!\n");
#endif
			return;
		}
	}

	strcpy(libitem[end].name, item->name);
	libitem[end].time = item->time;
    
	end ++;
#ifdef CRAZY
            printf("[*search_insert] Over!\n");
#endif

}

char* index_name(double time) {
#ifdef CRAZY
    printf("[#index_name] Begin!\n");
#endif
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
    
#ifdef CRAZY
            printf("[*index_name] Over!\n");
#endif
	return ret;
}

void info_extract() {
//	printf("Line_buf%s\n", line_buf);
#ifdef CRAZY
            printf("[#info_extract] Begin!\n");
#endif
    item_t* tmp = (item_t*)malloc(sizeof(item_t));
	char *buf = read_buf;
	int i = 0;

#ifdef DETAIL
    printf("MARK A\n");
#endif

	while(*buf != '(') {
	   tmp->name[i] = *buf;
	   i ++;
	   buf ++;
	}
	tmp->name[i] = '\0';

#ifdef DETAIL
    printf("MARK B\n");
#endif

	while(*buf != '<') {
//		printf("?\n");
	    buf ++;
	}
	buf ++;

#ifdef DETAIL
    printf("MARK C\n");
#endif
    
	i = 0;
	char time[10];
	for( ; i < 10; i ++) {
	    time[i] = '\0';
	}

#ifdef DETAIL
    printf("MARK D\n");
#endif

    i = 0;
	while(*buf != '>') {
	   time[i] = *buf;
	   buf ++;
	   i ++;
	}

#ifdef DETAIL
    printf("MARK E\n");
#endif

	double base = 0.000001;
	char *mark = time;
	double factor = 0.0;

    while(*mark == '0' || *mark == '.') {
		*mark ++;	
	}

#ifdef DETAIL
    printf("MARK F\n");
#endif

	while(*mark != '\0') {
		factor *= 10;
	    factor += (*mark - '0');
		mark ++;
	}

#ifdef DETAIL
    printf("MARK G\n");
#endif

	tmp->time = factor * base;

	search_insert(tmp);

#ifdef DETAIL
    printf("MARK H\n");
#endif

	free(tmp);

#ifdef CRAZY
            printf("[*info_extract] Over!\n");
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

#ifdef DEBUG
 for(int i = 0; i < argc + 2; i ++) {
     printf("%s\t", exec_argv[i]);
 }
 
 printf("\n");
#endif
  
  char *path = NULL; 
  //char path_name[6] = "PATH=";
  char exe_name[8] = "/strace";
  char args[64] = "";
  //strcpy(PATH, path_name);
  path = getenv("PATH");
  char PATH[strlen(path) + 1];
  strcpy(PATH, path);
 
#ifdef DEBUG
    printf("PATH:%s\n", PATH);
#endif
  //strcat(PATH, path);

  //char *exec_envp[] = { PATH, NULL, };

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
	  //close(trash);
	  //close(fildes[0]);
	  dup2(fildes[1], fileno(stderr));
	  char* pwd = strtok(PATH, ":");
	  memset(args, '\0', 64);
	  strcpy(args, pwd);
	  strcat(args, exe_name);
      //子进程，执行strace命令
	  int retcode;
	  do {
	      retcode = execve(args, exec_argv, environ);
	      pwd = strtok(NULL, ":");
	      memset(args, '\0', 64);
	      strncpy(args, pwd, 64*sizeof(char));
	      strcat(args, exe_name);
	  } while( retcode == -1);
	  //不应该执行此处代码，否则execve失败，出错处理
  }
  else {
	  sleep(1);
	  close(fildes[1]);

	  dup2(fildes[0], fileno(stdin));

      double total_time = 0;
      char* name;
      double ratio;
      int count = 0;
	  clock_t begin =  clock();
	  while(fgets(read_buf, 511, stdin) != NULL) {
		  if(terminate_detect() == false) {
		      break;
		  }
		  bool output = false;
		  eofsmash();
		  if(end_detect() == false) {
		      char* left = fgets(left_buf, 511, stdin); 
			  assert(left != NULL);
			  strcat(read_buf, left_buf);
		  }
		  info_extract();
          clock_t current = clock();

		  if((current - begin) / 1000 > 1) {
		      output = true;
		  }
         
		  if(output == true) {
		  
			  total_time = 0.0;

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

			  begin = clock();
		  }

		  memset(read_buf, '\0', 512);
	  }
	  /*
	  int len = 0;
	  //for(int i = 0; i < 1; i ++) {
	  while(1){
		  sleep(0.5);
		  len = read(fildes[0], read_buf, 512);
		  if(len == 0) {
		      break;
		  }
		  printf("$$$$%d$$$$\n%s\n=====================================================================================\n", len, read_buf);
		  memset(read_buf, '\0', 512);
		  fflush(stdout);
		  //if(read(fildes[0], read_buf, 10240) == 0) {
		  //  break;		  
		}
	      //sleep(1);
		  //read(fildes[0], read_buf, 10240);
		  //printf("============================================================================================\n");
	  }
	  */

	  //int len = read(fildes[0], read_buf, 10240);
	  //printf("==================%d===================\n", len);
	  //printf("%s", read_buf);
	  //exit(0);

	  // ========================================================
	  //unsigned read_length = 0;
	  //read_length = read(fildes[0], read_buf, 200);
	  //assert(read_length != 0);

	  //printf("Len:%d\n", read_length);
	  //eofsmash();
	  //printf("%s\n", test_buf);
	  //memset(test_buf, '\0', 128);

	  /*
	  //for(int i = 0; i < 1000; i ++ ) {
	  while(read_length != 0) {
		  //sleep(1);
		  read_length = read(fildes[0], test_buf, 128);
	      printf("%s", test_buf);
	      memset(test_buf, '\0', 128);
		  //printf("____________________%u________________________\n", read_length);
		  //printf("++++++++++++++++++++++++++++++++++++++++++++++\n");
	  } 
	  printf("^*^&*^*&^*&^&&$&$^&*^*(*&(&)&)(&()&*^&*&$^$^\n");
	  exit(0);
	  // =========================================================
      */

/*-----------------------------------------------------------
	  char* read_length = NULL;
	  int read_line = 0;
	  char* name;
	  double ratio;
	  int count = 0;
	  clock_t begin =  clock();

	  while(1) {
	      read_length = fgets(line_buf, 200, stdin);

		  if(read_length == NULL) {
		      break;
		  }

	      eofsmash();
	      bool output = false;

	      info_extract();
	  
          clock_t current = clock();

		  if((current - begin) / 1000 > 1) {
		      output = true;
		  }
          
		  if(output == true) {

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

			  begin = clock();
		  }
------------------------------------------------------------------*/
		  //sleep(1);
//	  for(int i = 0; i < 8; i ++) {
//		  printf("Line_buf:%s\n", line_buf);
			  //memset(read_buf, '\0', 128);
			  //sleep(1);
			  //memset(read_buf, '\0', 200);
			  //read_length = read(fildes[0], read_buf, 200);
	          //eofsmash();
	          //printf("%s\n", read_buf);
		      //continue;
		  //}
		  //else {
		   //   break;
		  //}
	  //父进程，读取strace输出并统计
  //   }
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
