#include "DMA_PROXY/AXI_LITE_4.h"

unsigned char initFlg=0;
unsigned int  fpgaPgOffset=0;
volatile unsigned char * fpgaMapBase=NULL;

int fpgaInit(unsigned int BaseAddr)
{
    initFlg = 0;
	  //??mem??
    int fd = open("/dev/mem", O_RDWR | O_SYNC);

    if(fd == -1)
    {
        printf("open /dev/mem error!");
        return -1;
    }

    unsigned int base = BaseAddr & PAGE_MASK;

    fpgaPgOffset = BaseAddr & (~PAGE_MASK);
	  //????
    fpgaMapBase = (volatile unsigned char *)mmap(NULL,PAGE_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,base);
    if(fpgaMapBase == MAP_FAILED)
    {
        printf("mmap error!");
        close(fd);
        return -2;
    }
	  //????
    close(fd);

    initFlg = 1;

    return 0;
}

void fpgaWrite32(unsigned int Reg, unsigned int Data)
{
    if(initFlg == 0)
    {
        printf("fpga is not init!");
        return;
    }

    *(volatile unsigned int *)(fpgaMapBase + fpgaPgOffset + Reg) = Data;
}

int fpgaRead32(unsigned int Reg)
{
    unsigned int Value;

    if(initFlg == 0)
    {
        printf("fpga is not init!");
        return -1;
    }

    Value = *(volatile unsigned int*)(fpgaMapBase + fpgaPgOffset + Reg);

    return Value;
}

void fpgaDeInit()
{
    initFlg = 0;
    munmap((void *)fpgaMapBase, PAGE_SIZE);
}