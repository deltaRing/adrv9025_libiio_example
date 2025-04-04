#ifndef _DMA_FUNCTION_H_
#define _DMA_FUNCTION_H_

/**
 * @file axidma_transfer.c
 * @date Sunday, November 29, 2015 at 12:23:43 PM EST
 * @author Brandon Perez (bmperez)
 * @author Jared Choi (jaewonch)
 *
 * This program performs a simple AXI DMA transfer. It takes the input file,
 * loads it into memory, and then sends it out over the PL fabric. It then
 * receives the data back, and places it into the given output file.
 *
 * By default it uses the lowest numbered channels for the transmit and receive,
 * unless overriden by the user. The amount of data transfered is automatically
 * determined from the file size. Unless specified, the output file size is
 * made to be 2 times the input size (to account for creating more data).
 *
 * This program also handles any additional channels that the pipeline
 * on the PL fabric might depend on. It starts up DMA transfers for these
 * pipeline stages, and discards their results.
 *
 * @bug No known bugs.
 **/

#include <iostream>
#include <complex>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <fcntl.h>              // Flags for open()
#include <sys/stat.h>           // Open() system call
#include <sys/types.h>          // Types for open()
#include <unistd.h>             // Close() system call
#include <string.h>             // Memory setting and copying
#include <getopt.h>             // Option parsing
#include <errno.h>              // Error codes

#include "util.h"               // Miscellaneous utilities
#include "conversion.h"         // Convert bytes to MiBs
#include "libaxidma.h"          // Interface ot the AXI DMA library

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/


// A convenient structure to carry information around about the transfer
struct dma_transfer {
    int input_fd;           // The file descriptor for the input file
    int input_channel;      // The channel used to send the data
    int input_size;         // The amount of data to send
    void *input_buf;        // The buffer to hold the input data
    int output_fd;          // The file descriptor for the output file
    int output_channel;     // The channel used to receive the data
    int output_size;        // The amount of data to receive
    void *output_buf;       // The buffer to hold the output
};

int DMA_FFT(axidma_dev_t dev, int in_chan, int out_chan,
  unsigned int input_size, unsigned int output_size, 
  std::complex<float> * input_buf, std::complex<float> * output_buf);
  
void set_up_DMA(axidma_dev_t & axidma_dev);

#endif