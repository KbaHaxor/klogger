#include <linux/slab.h>

/**
 * Structure that stores information of a ring's element
 * It can be extended on the future implementations
 */
struct ringBufferElement {
	int counter;
	char data;
	struct ringBufferElement *next;
};

struct ringBufferElement *head,*last,*p;
int RING_BUFFER_MAX_SIZE;

int initialize_ring_buffer(int numElements);
void add_ring_element(char data);
void clean_ring_buffer(void);
char* get_ring_elements(void);
