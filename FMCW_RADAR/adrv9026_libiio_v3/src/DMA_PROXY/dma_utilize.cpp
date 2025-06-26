#include "DMA_PROXY/dma_utilize.h"
#include <iostream>

// open the channel
// input 1: which channel
// input 2: channel name
// input 3: is it tx channel or rx channel
void init_channel(struct channel * channel_ptr, const char * name, bool tx) {
  char channel_name[64] = "/dev/";
  strcat(channel_name, name);
  channel_ptr->fd = open(channel_name, O_RDWR);
  if (channel_ptr->fd < 1) {
  		printf("Unable to open DMA proxy device file: %s\r", channel_name);
  		exit(-1);
  }
  if (tx) {
     channel_ptr->buf_ptr = (struct channel_buffer *)mmap(NULL, sizeof(struct channel_buffer) * TX_BUFFER_COUNT,
										PROT_READ | PROT_WRITE, MAP_SHARED, channel_ptr->fd, 0);
		if (channel_ptr->buf_ptr == MAP_FAILED) {
			printf("Failed to mmap tx channel\n");
			exit(EXIT_FAILURE);
		}
  } else {
    channel_ptr->buf_ptr = (struct channel_buffer *)mmap(NULL, sizeof(struct channel_buffer) * RX_BUFFER_COUNT,
										PROT_READ | PROT_WRITE, MAP_SHARED, channel_ptr->fd, 0);
		if (channel_ptr->buf_ptr == MAP_FAILED) {
			printf("Failed to mmap rx channel\n");
			exit(EXIT_FAILURE);
		}
  }
		
}

// clear the channel buffer
void clear_channel(struct channel * channel_ptr) {
  munmap(channel_ptr->buf_ptr, sizeof(struct channel_buffer));
  close(channel_ptr->fd);
}

void transfer_tx(struct channel * channel_ptr, unsigned int * data, unsigned int length) {
  int buffer_id = 0;
  // TX_BUFFER_COUNT = 1
  for (int buffer_id = 0; buffer_id < TX_BUFFER_COUNT; buffer_id += BUFFER_INCREMENT) {

		/* Set up the length for the DMA transfer and initialize the transmit
		 * buffer to a known pattern.
		 */
		channel_ptr->buf_ptr[buffer_id].length = length;

    // copy data to DMA
		memcpy(channel_ptr->buf_ptr[buffer_id].buffer, data, length);

		/* Start the DMA transfer and this call is non-blocking
		 *
		 */
		ioctl(channel_ptr->fd, START_XFER, &buffer_id);

	}
 
 /* Perform the DMA transfer and check the status after it completes
  * as the call blocks til the transfer is done.
  */
	ioctl(channel_ptr->fd, FINISH_XFER, &buffer_id);
	if (channel_ptr->buf_ptr[buffer_id].status != PROXY_NO_ERROR)
	  printf("Proxy tx transfer error\n");
}

void receive_rx(struct channel * channel_ptr, unsigned int * data, unsigned int length) {
  int buffer_id = 0; int num_transfers = 1; int in_progress_count = 0;
	// Start all buffers being received
	for (buffer_id = 0; buffer_id < RX_BUFFER_COUNT; buffer_id += BUFFER_INCREMENT) {

		/* Don't worry about initializing the receive buffers as the pattern used in the
		 * transmit buffers is unique across every transfer so it should catch errors.
		 */
		channel_ptr->buf_ptr[buffer_id].length = length;

		ioctl(channel_ptr->fd, START_XFER, &buffer_id);
    
    if (++in_progress_count >= num_transfers)
			break;
	}
 
	buffer_id = 0;
  ioctl(channel_ptr->fd, FINISH_XFER, &buffer_id);
  if (channel_ptr->buf_ptr[buffer_id].status != PROXY_NO_ERROR) {
		printf("Proxy rx transfer error, # transfers %d, # in progress %d\n",
				num_transfers, in_progress_count);
		exit(1);
	}
  unsigned int *buffer = channel_ptr->buf_ptr[buffer_id].buffer;
  
  // copy buffer to data
  memcpy(data, buffer, length);
}