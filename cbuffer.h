/* Circular buffer, taken and modified from an example on wikipedia
http://en.wikipedia.org/w/index.php?title=Circular_buffer&oldid=474915405
*/

#ifndef _cbuffer_h_
#define _cbuffer_h_

//#define TEST_CBUFF

#include <malloc.h>
#include <pthread.h>

typedef short buff_t;


/* Circular buffer object */
typedef struct {
	int         size;   /* maximum number of elements           */
	int         start;  /* index of oldest element              */
	int         end;    /* index at which to write new element  */
	pthread_mutex_t mutex;
	buff_t   *elems;  /* vector of elements                   */
} CircularBuffer;


void cbInit(CircularBuffer *cb, int size);
void cbFree(CircularBuffer *cb);
int cbIsFull(CircularBuffer *cb);
int cbIsEmpty(CircularBuffer *cb);
/* Write an element, overwriting oldest element if buffer is full. App can
choose to avoid the overwrite by checking cbIsFull(). */
void cbWrite(CircularBuffer *cb, buff_t elem);
/* Read oldest element. App must ensure !cbIsEmpty() first. */
void cbRead(CircularBuffer *cb, buff_t *elem);

#endif
