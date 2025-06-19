#ifndef _AXI_LITE_4_H_
#define _AXI_LITE_4_H_

// AXI LITE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#define  _AXI_LITE_ADDRESS_ 0xA0080000
#define  xPAA_WR_DATA_REG0    0
#define  xPAA_WR_DATA_REG1    4
#define  xPAA_WR_DATA_REG2    8
#define  xPAA_WR_DATA_REG3    12

#define PAGE_SIZE  ((size_t)getpagesize())
#define PAGE_MASK ((unsigned long) (long)~(PAGE_SIZE - 1))

void fpgaDeInit();
int fpgaInit(unsigned int BaseAddr);
int fpgaRead32(unsigned int Reg);
void fpgaWrite32(unsigned int Reg, unsigned int Data);

#endif