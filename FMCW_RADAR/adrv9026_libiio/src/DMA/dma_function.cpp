#include "../../include/DMA/dma_function.h"


// in chan: 0 (8192) / 2 (512)
// out chan: 1 (8192) / 3 (512)
// 
int DMA_FFT(axidma_dev_t dev, int in_chan, 
  int out_chan, unsigned int input_size, unsigned int output_size, 
  std::complex<float> * input_buf, std::complex<float> * output_buf)
{
    int rc = -1;
    
    if (dev == NULL){
        fprintf(stderr, "Dev is NULLptr.\n");
        return rc;
    }
    
    struct dma_transfer * trans = (struct dma_transfer *)malloc(sizeof(struct dma_transfer));
    memset(trans, 0, sizeof(trans));
    trans->input_channel = in_chan;
    trans->output_channel = out_chan;
    trans->input_size = input_size * sizeof (std::complex<float>); // 8192 or 512
    trans->output_size = output_size * sizeof (std::complex<float>);
    
    // Allocate a buffer for the input file, and read it into the buffer
    trans->input_buf = (void *)axidma_malloc(dev, trans->input_size);
    if (trans->input_buf == NULL) {
        fprintf(stderr, "Failed to allocate the input buffer.\n");
        rc = -ENOMEM;
        goto ret;
    }
    
    // copy signal to input_buf
    memcpy(trans->input_buf, input_buf, trans->input_size);

    // Allocate a buffer for the output file
    trans->output_buf = (void *)axidma_malloc(dev, trans->output_size);
    if (trans->output_buf == NULL) {
        rc = -ENOMEM;
        goto free_input_buf;
    }

    // Perform the transfer
    // Perform the main transaction
    rc = axidma_twoway_transfer(dev, trans->input_channel, trans->input_buf,
            trans->input_size, NULL, trans->output_channel, trans->output_buf,
            trans->output_size, NULL, true);
    if (rc < 0) {
        fprintf(stderr, "DMA read write transaction failed.\n");
        goto free_output_buf;
    }
    rc = 0;
    
    // copy signal to input_buf
    memcpy(output_buf, trans->output_buf, trans->output_size);

free_output_buf:
    axidma_free(dev, trans->output_buf, trans->output_size);
free_input_buf:
    axidma_free(dev, trans->input_buf, trans->input_size);
ret:
    delete trans;
    return rc;
}

/*----------------------------------------------------------------------------*
 * prepare channels                                                           *
 *----------------------------------------------------------------------------*/

void set_up_DMA(axidma_dev_t & dev)
{
    int rc = 0;
    const array_t *tx_chans, *rx_chans;

    // Initialize the AXIDMA device
    dev = axidma_init();
    if (dev == NULL) {
        fprintf(stderr, "Error: Failed to initialize the AXI DMA device.\n");
        rc = 1;
        return;
    }

    // Get the tx and rx channels if they're not already specified
    tx_chans = axidma_get_dma_tx(dev);
    if (tx_chans->len < 1) {
        fprintf(stderr, "Error: No transmit channels were found.\n");
        rc = -ENODEV;
        goto destroy_axidma;
    }
    rx_chans = axidma_get_dma_rx(dev);
    if (rx_chans->len < 1) {
        fprintf(stderr, "Error: No receive channels were found.\n");
        rc = -ENODEV;
        goto destroy_axidma;
    }
    
    if (rc == 0) return;
destroy_axidma:
    axidma_destroy(dev);
ret:
    return;
}