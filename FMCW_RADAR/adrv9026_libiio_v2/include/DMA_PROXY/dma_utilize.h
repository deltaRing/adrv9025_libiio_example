#ifndef _DMA_UTILIZE_H_
#define _DMA_UTILIZE_H_

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <signal.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <sys/param.h>
#include "dma-proxy.h"

struct channel {
	struct channel_buffer *buf_ptr;
	int fd;
};

#define TX_CHANNEL_COUNT 4
#define RX_CHANNEL_COUNT 4

#define _TX_CHAN_8192_ 0    // FFT 8192 Points
#define _RX_CHAN_8192_ 0    
#define _TX_CHAN_1024_ 1    // FFT 1024 Points
#define _RX_CHAN_1024_ 1
#define _TX_CHAN_16384_ 1   // FFT 16384 Points
#define _RX_CHAN_16384_ 1
#define _TX_CHAN_I16384_ 3  // IFFT 16384 Points
#define _RX_CHAN_I16384_ 3
#define _TX_CHAN_SEL_  4    // Select Channel
#define _RX_CHAN_SEL_  4

#define _CHECK_MOD_(X) int(X % sizeof(unsigned int) == 0? 0: 1)
 
// open the channel
// input 1: which channel
// input 2: channel name
// input 3: is it tx channel or rx channel
void init_channel(struct channel * channel_ptr, const char * name, bool tx);

// clear the channel buffer
void clear_channel(struct channel * channel_ptr);

// transfer data via DMA
// input 1: which channel
// input 2: transfer data
// input 3: data length
void transfer_tx(struct channel * channel_ptr, unsigned int * data, unsigned int length);

// receive data via DMA
// input 1: which channel
// input 2: receive data
// input 3: data length
void receive_rx(struct channel * channel_ptr, unsigned int * data, unsigned int length);


#endif