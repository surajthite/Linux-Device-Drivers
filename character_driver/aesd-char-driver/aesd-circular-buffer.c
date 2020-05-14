/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Suraj Thite
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned
 *      aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset
 *      is found in aesd_buffer.
 *
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
			size_t char_offset, size_t *entry_offset_byte_rtn )
{
    /**
    * TODO: implement per description
    */
	size_t buff_size_total = 0;
	size_t bytes_con_total = 0; 
	size_t search_buff_size = 0; 
	size_t prev_index_size = 0;
	size_t pres_index_size = 0;
	size_t out_index = buffer->out_offs; 
	uint8_t i = 0;     
	uint8_t j = 0; 
	for(j = 0; j < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; j++) //itereate through the circular buffer
	{
		buff_size_total = buff_size_total + buffer->entry[j].size;
		// #ifdef __KERNEL__
		// 	printk("\n total buffer size is :  %d \n", buff_size_total);
		// #else
		// 	printf("\n total buffer size is :    %d \n", buff_size_total);
		// #endif
	}

	if(char_offset >= buff_size_total) //check for valid offset 
		return NULL;

	while(i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
	{
		// #ifdef __KERNEL__
		// 	printk("out index is :%d\n", out_index);
		// #else
		// 	printf("out index is :%d\n", out_index);
		// #endif
		pres_index_size = buffer->entry[out_index].size;

		if(char_offset < pres_index_size)
		{
			*entry_offset_byte_rtn = char_offset;
			return &buffer->entry[out_index]; //return the address 
		}
		else if (char_offset == pres_index_size)
		{
			size_t temp = 0;
			if(out_index == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED -1)
				temp = 0;
			else
				temp = out_index + 1;
			return &buffer->entry[temp]; //return the address
		}
		else 
			if(char_offset > pres_index_size)
			char_offset = char_offset - buffer->entry[out_index].size;
		
		bytes_con_total = bytes_con_total + pres_index_size;
		if(out_index == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED -1)
		{
			out_index = 0; //set out_index =0
		}
		else
		{
			out_index++; //Increment out_index
		}
		
		i++;
	}
    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/





void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{

	
	if(buffer->in_offs == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)  // check for  10 entries
	{
		
		printk("\n Buffer is full \n");
		buffer->in_offs = 0;
		buffer->out_offs = 0;
		buffer->full = true; //set full flag 
	}

	// if(buffer->out_offs == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
	// {
	// 	buffer->out_offs = 0;
	// }
	
	buffer->entry[buffer->in_offs].size = add_entry->size;
	buffer->entry[buffer->in_offs].buffptr = add_entry->buffptr;
	
	
	
	if(buffer->full == true)
	{	
		buffer->out_offs++; // increment out_offs
		buffer->full = false;
		buffer->in_offs ++;
				
	}
	else
	{
	#ifdef __KERNEL__
	printk("The contents of buffer is : %s \n ", buffer->entry[buffer->in_offs].buffptr);
	#else
	printf("The contents of buffer is : %s \n ", buffer->entry[buffer->in_offs].buffptr);
	#endif
	buffer->in_offs++; // increment in_offs	
	}

int k = 0;
for(k=0;k<10;k++)
{
	printk("The contents of buffer is : %s at position %d \n ",buffer->entry[k].buffptr,k);	
}

}


/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}
