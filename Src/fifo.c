#include "fifo.h"


#define TXFIFOSIZE		1024
#define RXFIFOSIZE		1024
#define TXFAIL			0
#define TXSUCCESS		1
#define RXFAIL			0
#define RXSUCCESS		1

tx_dataType static TX_FIFO[TXFIFOSIZE];
rx_dataType static RX_FIFO[RXFIFOSIZE];


uint32_t volatile tx_put_itr;
uint32_t volatile tx_get_itr;
uint32_t volatile rx_put_itr;
uint32_t volatile rx_get_itr;


/*Initialize tx fifo*/
void tx_fifo_init(void)
{
	tx_put_itr = 0;
	tx_get_itr = 0;
}

/*Put data into tx fifo*/

uint8_t  tx_fifo_put(tx_dataType data)
{
	/*Check if fifo is full*/
	if((tx_put_itr - tx_get_itr) & ~(TXFIFOSIZE -1))
	{
		/*Fifo is full*/
		return(TXFAIL);
	}

	/*Put data into fifo*/
	TX_FIFO[tx_put_itr & (TXFIFOSIZE -1)] = data;

	/*Increment itr*/
	tx_put_itr++;
	return(TXSUCCESS);

}

uint8_t  tx_fifo_get(tx_dataType *pdata)
{
	/*Check if fifo is empty*/
	if(tx_put_itr == tx_get_itr){

		/*Fifo empty*/
		return(TXFAIL);

	}
	/*Get the data*/
	*pdata =  TX_FIFO[tx_get_itr & (TXFIFOSIZE -1)];

	/*Increment the itr*/
	tx_get_itr++;

	return(TXSUCCESS);
}

uint32_t tx_fifo_size(void)
{
	return (uint32_t)(tx_put_itr - tx_get_itr);
}


/*Initialize rx fifo*/
void rx_fifo_init(void)
{
	rx_put_itr = 0;
	rx_get_itr = 0;
}

/*Put data into rx fifo*/

uint8_t  rx_fifo_put(rx_dataType data)
{
	/*Check if fifo is full*/
	if((rx_put_itr - rx_get_itr) & ~(RXFIFOSIZE -1))
	{
		/*Fifo is full*/
		return(RXFAIL);
	}

	/*Put data into fifo*/
	RX_FIFO[rx_put_itr & (RXFIFOSIZE -1)] = data;

	/*Increment itr*/
	rx_put_itr++;
	return(RXSUCCESS);

}

uint8_t  rx_fifo_get(rx_dataType *pdata)
{
	/*Check if fifo is empty*/
	if(rx_put_itr == rx_get_itr){

		/*Fifo empty*/
		return(RXFAIL);

	}
	/*Get the data*/
	*pdata =  RX_FIFO[rx_get_itr & (RXFIFOSIZE -1)];

	/*Increment the itr*/
	rx_get_itr++;

	return(RXSUCCESS);
}

uint32_t rx_fifo_size(void)
{
	return (uint32_t)(rx_put_itr - rx_get_itr);
}



