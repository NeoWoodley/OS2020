#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>   //char *strstr(const char *haystack, const char *needle)
#include <dirent.h>

int count = 0;
char space[3] = "  ";

struct node {
    int pid;
	char name[50];
	int ppid;
	int depth;
	struct node* child;
	struct node* peer;
};

struct node* findpeertail(struct node* Node) {
	// struct node* end = Node;
    while(Node->peer!=NULL) {
	    Node = Node->peer;
	}
	return Node;
}

int len2n(char *str) {
    int len = 0;
	while (str[len] != '\n') {
	    len ++;
	}
	return len;
}

int str2int(char *str) {
    int val = 0;
	int i = 0;
	while(str[i] != '\0' && str[i] != '\n') {
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

void buildtree(struct node* Node) {
	struct node* temp;
    if(Node->child == NULL) {
		for(int i = 0; i < Node->depth; i ++) {
		    printf("%s",&space[0]);
		}
		printf("%s{%d}--ppid{%d}--depth{%d}\n",&(Node)->name[0],Node->pid,Node->ppid,Node->depth);
       // printf("%s{%d}\n",&(Node->name[0]), Node->pid);
		return;
	}
	
	else if(Node->child != NULL) {
		for(int i = 0; i < Node->depth; i ++) {
		    printf("%s",&space[0]);
		}
		printf("%s{%d}--ppid{%d}--depth{%d}\n",&(Node)->name[0],Node->pid,Node->ppid,Node->depth);
       // printf("%s{%d}\n",&(Node->name[0]), Node->pid);
		temp = (struct node*)Node->child->peer;
	    while(temp != NULL) {
		   buildtree(temp);
		   temp = (struct node*)(temp->peer);
		}
		return;
	}
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

  for(int i = 0; i < count; i ++){
      infolib[i].depth = 0;
	  infolib[i].peer = NULL;
	  infolib[i].child = NULL;
  }

  printf("Total process number:%d\n",count);

  closedir(dir);

  char* path = (char*)malloc(32*sizeof(char));
  char* name = (char*)malloc(64*sizeof(char));
  char* ppid = (char*)malloc(32*sizeof(char));
  FILE* file;
  for(int i = 0; i < count; i ++) {
	  sprintf(path,"/proc/%d/status",infolib[i].pid);
      file = fopen(path, "r");
      if(file) {
	      fgets(name, 63, file);
		  strcat(&infolib[i].name[0], &name[6]);
		  infolib[i].name[len2n(infolib[i].name)] = '\0';
      for(int j = 0; j < 6; j ++) {
	      fgets(ppid, 32, file);
	  }   
//	  printf("-----%s",ppid);
	  infolib[i].ppid=str2int(&ppid[6]);
	  } 	  

  }
  free(path);
  free(name);
  free(ppid);


  //struct node* temp = (struct node*)malloc(sizeof(struct node));
  struct node* temp;
  for(int i = 0; i < count; i ++) {
	  for(int j = 0; j < count; j ++) {
	      if(infolib[i].child == NULL && infolib[j].ppid == infolib[i].pid) {
		      infolib[i].child = (struct node*)&infolib[j];
			  infolib[j].depth = infolib[i].depth + 1; 
		  }
		  else if(infolib[i].child != NULL && infolib[j].ppid == infolib[i].pid) {
			  if(infolib[i].child->peer == NULL) {
			      infolib[i].child->peer = (struct node*)&infolib[j];
			  }
			  else if(infolib[i].child->peer != NULL){
                  temp = findpeertail((struct node*)&(infolib[i].child->peer));
			      temp->peer = (struct node*)&infolib[j];
			  }
			  infolib[j].depth = infolib[i].depth + 1; 
		  }
	  }
  }

  assert(infolib[0].child->peer != NULL);

  /*
  temp = infolib[0].child;
  while(temp != NULL) {
	  printf("Thread:%s Pid:%d PPid:%d\n", temp->name, temp->pid, temp->ppid);
	  temp = temp->peer;
  }
  */

  for(int i = 0; i < 1; i ++) {
	  if(infolib[i].depth == 0) {
		  assert(infolib[i].child->peer != NULL);
		  printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].name, infolib[i].pid, infolib[i].ppid);
		  printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].child->name, infolib[i].child->pid, infolib[i].child->ppid);
		  printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].child->peer->name, infolib[i].child->peer->pid, infolib[i].child->peer->ppid);
		  if(infolib[i].child->peer->peer == NULL) {
		      printf("Yes!!!!!!!!!!!!!!!!\n");
		  }
		  else {
		      printf("No?????????????????\n");
		  
		  }
		  //printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].child->peer->peer->name, infolib[i].child->peer->peer->pid, infolib[i].child->peer->peer->ppid);
  //        buildtree((struct node*)&infolib[i].child->peer);
		  printf("-------------------------------------------------------------\n");
	  }
//	  printf("%s{%d}--ppid{%d}--depth{%d}\n",&infolib[i].name[0], infolib[i].pid, infolib[i].ppid, infolib[i].depth);
  }

  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
//    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  return 0;
  
}
