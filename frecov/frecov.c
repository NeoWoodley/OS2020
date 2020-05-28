#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <sys/mman.h>    
#include <fcntl.h>
#include <unistd.h>   
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#define panic_on(cond, out) {if((cond) != 1) printf("%s\n",out);}

struct fat_header {
    uint8_t BS_jmpBoot[3];
	uint8_t BS_OEMName[8];
	uint32_t BPB_BytsPerSec: 16;
	uint32_t BPB_SecPerClus: 8;
	uint16_t BPB_RsvdSecCnt;
	uint8_t BPB_NumFATS;
	uint16_t BPB_RootEntCnt;
	uint16_t BPB_TotSec16;
	uint8_t BPB_Media;
    uint16_t BPB_FATSz16;
	uint16_t BPB_SecPerTrk;
	uint16_t BPB_NumHeads;
	uint32_t BPB_Hiddsec;
	uint32_t BPB_TotSec32;
	uint32_t BPB_FATSz32;
	uint16_t BPB_ExtFlags;
	uint16_t BPB_FSVer;
	uint32_t BPB_RootClus;
	uint16_t BPB_FSInfo;
	uint16_t BPB_BkBootSec;
	uint8_t BPB_Reserved[12];
	uint8_t BS_DrvNum;
	uint8_t BS_Reserved1;
	uint8_t BS_BootSig;
	uint32_t BS_VollD;
	uint8_t BS_VolLab[11];
	uint32_t BS_FilSysType[2];

	uint8_t padding[420];
	uint16_t signature;
} __attribute__((packed));


typedef struct fat_header fat_header;

int main(int argc, char *argv[]) {
	panic_on((sizeof(fat_header) == 512), "Bad!");

	FILE* tmpimg = fopen("./M5-frecov.img", "r");
	fseek(tmpimg, 0, SEEK_END);
	int size = ftell(tmpimg);
	fclose(tmpimg);


	int img = open("./M5-frecov.img", O_RDONLY);
	char template[] = "/tmp/template-XXXXXX";
	int headpart = mkstemp(template);

	uint8_t read_buf[522];
	read(img, read_buf, 512);
	write(headpart, read_buf, 512);
	

	fat_header* disk = mmap(NULL, 4096, PROT_READ, MAP_SHARED, headpart, 0);
	//assert(disk != MAP_FAILED);
    switch(errno) {
	    case EACCES: {printf("?\n"); break;}
	    case EAGAIN: {printf("!\n"); break;}
	    case EBADF: {printf("*\n"); break;}
	    case EINVAL:{printf("&\n"); break;}
	    case ENFILE:{printf("(\n"); break;}
	    case ENODEV: {printf(")\n"); break;}
	    case ENOMEM: {printf("^\n"); break;}
	    case EOVERFLOW: {printf("-\n"); break;}
	    case EPERM: {printf("+\n"); break;}
	    case ETXTBSY: {printf("]\n"); break;}
	}

	printf("%x\n",disk->signature);
	//panic_on(((disk->signature) == 0xaa55), "Not a valid fat!");

	return 0;

}
