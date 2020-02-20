#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>   //char *strstr(const char *haystack, const char *needle)
#include <dirent.h>

struct node {
    int pid;
	char* name;
	int ppid;
};

/*
char* pid2name(int pid) {
    char* name = (char*)calloc(128,sizeof(char));
    if(name){
	    sprintf(name, "/proc/%d/cmdline", pid);
		FILE* f = fopen(name,"r");
		if(f){
		    size_t size;
			size = fread(name, sizeof(char), 128, f);
			if(size > 0) {
			    if('\n'==name[size-1]){
				    name[size-1]='\0';
				}
			}
			fclose(f);
		}
	}
	assert(name != NULL);
	return name;
}
*/

int str2int(char *str) {
    int val = 0;
	int i = 0;
	while(str[i] != '\0') {
		val *= 10;
		val += str[i] - '0';
		i ++;
	}
	return val;
}

int find_num(char *str) {
    int return_code;

    if(strchr(str, '0')==NULL && strchr(str, '1')==NULL && strchr(str, '2')==NULL && strchr(str, '3')==NULL && strchr(str, '4')==NULL && strchr(str, '5')==NULL && strchr(str, '6')==NULL && strchr(str, '7')==NULL && strchr(str, '8')==NULL && strchr(str, '9')==NULL) {
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

  struct node infolib[512];

  dir = opendir("/proc");
  assert(dir != NULL);

  char _V[13]="-V --version";
  char _n[18]="-n --numeric-sort";
  char _p[15]="-p --show-pids";

//  pid_t curr_pid = getpid();
//  printf("PID:%d\n",curr_pid);
  
  int count = 0;

  while((entry=readdir(dir))) {
        if(find_num(entry->d_name) == 0) {
            // printf("File %s :Not a process file!\n", entry->d_name);
        }
        else {
            // printf("File %s :Process!\n", entry->d_name);
			infolib[count].pid = str2int(entry->d_name);
            //infolib[count].name = entry->d_name;
            count ++;
        }
  }

  printf("Total process number:%d\n",count);

  closedir(dir);

  char* path = (char*)malloc(32*sizeof(char));
  char* line = (char*)malloc(64*sizeof(char));
  size_t line_buf_size = 63;
  FILE* file;
  for(int i = 0; i < count; i ++) {
	  sprintf(path,"/proc/%d/status",infolib[i].pid);
      file = fopen(path, "r");
      if(file) {
	      getline(&line, &line_buf_size, file);
		  printf("name? %s\n",line);
		  sprintf((char*)(infolib[i].name), "%s",&line[6]);
		  //infolib[i].name = &line[6];
		  
		  for(int j = 0; j < 5; j ++) {
		      getline(&line, &line_buf_size, file);
		      printf("?????%s\n",line);
		  }
		  
	  } 	  
      printf("File %s PID %d\n", infolib[i].name, infolib[i].pid);
  }

  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  return 0;
  
}
