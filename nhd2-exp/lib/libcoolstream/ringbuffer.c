/*
 * Copyright (C) 2000 Paul Davis
 * Copyright (C) 2003 Rohan Drape
 *   
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *   
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *   
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *   
 * ISO/POSIX C version of Paul Davis's lock free ringbuffer C++ code.
 * This is safe for the case of one read thread and one write thread.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "ringbuffer.h"


/* Create a new ringbuffer to hold at least `sz' bytes of data. The
 * actual buffer size is rounded up to the next power of two.
 */
ringbuffer_t * ringbuffer_create (int sz)
{
	int power_of_two;
	ringbuffer_t *rb;

	rb = malloc (sizeof (ringbuffer_t));

	for(power_of_two = 1; 1 << power_of_two < sz; power_of_two++)
  		;

	rb->size = 1 << power_of_two;
	rb->size_mask = rb->size-1;
	rb->write_ptr = 0;
	rb->read_ptr = 0;
	rb->buf = malloc (rb->size);
	rb->mlocked = 0;
	printf("[ringbuffer] RING buffer size %d\n", rb->size); 
	fflush(stdout);

	return rb;
}


/* Free all data associated with the ringbuffer `rb'.
 */
void ringbuffer_free (ringbuffer_t * rb)
{
	if (rb->mlocked)
		munlock (rb->buf, rb->size);

	free (rb->buf);
}

/* Lock the data block of `rb' using the system call 'mlock'.  */
int ringbuffer_mlock (ringbuffer_t * rb)
{
	if (mlock (rb->buf, rb->size))
		return -1;

	rb->mlocked = 1;
	return 0;
}

/* Reset the read and write pointers to zero. This is not thread
 * safe.
 */
void ringbuffer_reset (ringbuffer_t * rb)
{
	rb->read_ptr = 0;
	rb->write_ptr = 0;
}

/* Return the number of bytes available for reading.  This is the
 * number of bytes in front of the read pointer and behind the write
 * pointer.
 */
size_t ringbuffer_read_space (ringbuffer_t * rb)
{
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r)
		return w - r;
	else
		return (w - r + rb->size) & rb->size_mask;
}

/* Return the number of bytes available for writing.  This is the
 * number of bytes in front of the write pointer and behind the read
 * pointer.
 */
size_t ringbuffer_write_space (ringbuffer_t * rb)
{
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r)
		return ((r - w + rb->size) & rb->size_mask) - 1;
	else if (w < r)
		return (r - w) - 1;
	else
		return rb->size - 1;
}

/* The copying data reader.  Copy at most `cnt' bytes from `rb' to
 * `dest'.  Returns the actual number of bytes copied.
 */
size_t ringbuffer_read (ringbuffer_t * rb, char *dest, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_read;
	size_t n1, n2;

	if ((free_cnt = ringbuffer_read_space (rb)) == 0)
		return 0;

	to_read = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = rb->read_ptr + to_read;

	if (cnt2 > rb->size)
	{
		n1 = rb->size - rb->read_ptr;
		n2 = cnt2 & rb->size_mask;
	}
	else
	{
		n1 = to_read;
		n2 = 0;
	}

	memcpy (dest, &(rb->buf[rb->read_ptr]), n1);
	rb->read_ptr += n1;
	rb->read_ptr &= rb->size_mask;

	if (n2)
	{
		memcpy (dest + n1, &(rb->buf[rb->read_ptr]), n2);
		rb->read_ptr += n2;
		rb->read_ptr &= rb->size_mask;
	}

	return to_read;
}

/* The copying data writer.  Copy at most `cnt' bytes to `rb' from
 * `src'.  Returns the actual number of bytes copied.
 */
size_t ringbuffer_write (ringbuffer_t * rb, char *src, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_write;
	size_t n1, n2;

	if ((free_cnt = ringbuffer_write_space (rb)) == 0)
		return 0;

	to_write = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = rb->write_ptr + to_write;

	if (cnt2 > rb->size) {
		n1 = rb->size - rb->write_ptr;
		n2 = cnt2 & rb->size_mask;
	}
	else
	{
		n1 = to_write;
		n2 = 0;
	}

	memcpy (&(rb->buf[rb->write_ptr]), src, n1);
	rb->write_ptr += n1;
	rb->write_ptr &= rb->size_mask;

	if (n2)
	{
		memcpy (&(rb->buf[rb->write_ptr]), src + n1, n2);
		rb->write_ptr += n2;
		rb->write_ptr &= rb->size_mask;
	}

	return to_write;
}

/* Advance the read pointer `cnt' places.
 */
void ringbuffer_read_advance (ringbuffer_t * rb, size_t cnt)
{
	rb->read_ptr += cnt;
	rb->read_ptr &= rb->size_mask;
}

/* Advance the write pointer `cnt' places.
 */
void ringbuffer_write_advance (ringbuffer_t * rb, size_t cnt)
{
	rb->write_ptr += cnt;
	rb->write_ptr &= rb->size_mask;
}

/* The non-copying data reader.  `vec' is an array of two places.  Set
 * the values at `vec' to hold the current readable data at `rb'.  If
 * the readable data is in one segment the second segment has zero
 * length.
 */
void ringbuffer_get_read_vector (ringbuffer_t * rb, ringbuffer_data_t * vec)
{
	size_t free_cnt;
	size_t cnt2;
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r)
		free_cnt = w - r;
	else
		free_cnt = (w - r + rb->size) & rb->size_mask;

	cnt2 = r + free_cnt;

	if (cnt2 > rb->size)
	{
		/* Two part vector: the rest of the buffer after the current write
		 * ptr, plus some from the start of the buffer.
		 */
		vec[0].buf = &(rb->buf[r]);
		vec[0].len = rb->size - r;
		vec[1].buf = rb->buf;
		vec[1].len = cnt2 & rb->size_mask;
	}
	else
	{
		/* Single part vector: just the rest of the buffer */
		vec[0].buf = &(rb->buf[r]);
		vec[0].len = free_cnt;
		vec[1].len = 0;
	}
}

/* The non-copying data writer.  `vec' is an array of two places.  Set
 * the values at `vec' to hold the current writeable data at `rb'.  If
 * the writeable data is in one segment the second segment has zero
 * length.
 */
void ringbuffer_get_write_vector (ringbuffer_t * rb, ringbuffer_data_t * vec)
{
	size_t free_cnt;
	size_t cnt2;
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r)
		free_cnt = ((r - w + rb->size) & rb->size_mask) - 1;
	else if (w < r)
		free_cnt = (r - w) - 1;
	else
		free_cnt = rb->size - 1;
//free_cnt = free_cnt / 188 * 188;
	cnt2 = w + free_cnt;

	if (cnt2 > rb->size)
	{
		/* Two part vector: the rest of the buffer after the current write
		 * ptr, plus some from the start of the buffer.
		 */
		vec[0].buf = &(rb->buf[w]);
		vec[0].len = rb->size - w;
		vec[1].buf = rb->buf;
		vec[1].len = cnt2 & rb->size_mask;
	}
	else
	{
		vec[0].buf = &(rb->buf[w]);
		vec[0].len = free_cnt;
		vec[1].len = 0;
	}
}
