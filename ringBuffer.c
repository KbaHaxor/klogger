#include <linux/slab.h>
#include "ringBuffer.h"

/**
 * Initialize an empty ring buffer
 *
 */
int initialize_ring_buffer(int numElements) 
{
	if(numElements < 0)
		return -1;

	RING_BUFFER_MAX_SIZE = numElements;
	head = (struct ringBufferElement *) kmalloc(RING_BUFFER_MAX_SIZE * sizeof(struct ringBufferElement),GFP_KERNEL);
	p = head;
	last = head+RING_BUFFER_MAX_SIZE;

	return 0;
}

/**
 * Adds one element to the ring buffer.
 *
 */
void add_ring_element(char data) 
{
	if(p == last) {
		// p points back again to the beginning of the buffer
		p = head;
	}

	p->data = data;
	p++;
}

/**
 * Cleans all data from the buffer
 *
 */
void clean_ring_buffer()
{
	int i;

	for(i=0; i < RING_BUFFER_MAX_SIZE; i++) 
	{
		(head+i)->data='0';
	}

	p = head;
} 

/**
 * Gets an array with all data currently present on the ring buffer.
 *
 */
char* get_ring_elements() 
{
	int i;
	char* allData = (char *) kmalloc(RING_BUFFER_MAX_SIZE * sizeof(char),GFP_KERNEL);

        for(i=0; i < RING_BUFFER_MAX_SIZE; i++)
        {
                allData[i] = head[i].data;
        }

	return allData;
}
