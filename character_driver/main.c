/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Suraj Thite
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include "aesdchar.h"

int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("SURAJ THITE"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int temp =0;
int flag =0;
int sum = 0;
char *Temp_Array = NULL;


int aesd_open(struct inode *inode, struct file *filp)
{
	PDEBUG("open start");
	struct aesd_dev *dev;
	dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
	filp->private_data = dev;
	PDEBUG("open stop");
	return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
	PDEBUG("release");
	return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	
	struct aesd_dev *dev = filp->private_data;

	mutex_lock_interruptible(&dev->lock);
	PDEBUG("read start");
	
	ssize_t retval = 0;
	
	size_t off;
	
	struct aesd_buffer_entry *ret_buffer;

	PDEBUG("read %zu bytes with offset %lld",count,*f_pos);

	size_t byte_ret = 0;

	PDEBUG("READ after lock");

	ret_buffer = aesd_circular_buffer_find_entry_offset_for_fpos(&(aesd_device.c_buffer),*f_pos,&byte_ret);
	
	PDEBUG("FPOS value is : %d",f_pos);
	if(ret_buffer == NULL)
	{
		PDEBUG("READ NULL");
		goto out;
	}

	int read_values = 0;
	
	PDEBUG("Current In_Offs: %d",aesd_device.c_buffer.in_offs);
	
	PDEBUG("Current Out_Offs: %d",aesd_device.c_buffer.out_offs);
	
	PDEBUG("f_pos : %ld",*f_pos);
	
	PDEBUG("size: %d",ret_buffer->size);
	
	PDEBUG("buffptr: %d",ret_buffer->buffptr);
	
	PDEBUG("Count: %d",count);
	
	//aesd_device.c_buffer.out_offs++;
	
	if(count > ret_buffer->size)
	{
		count = ret_buffer->size;
	}
	
	//ret_buffer->buffptr[count+1] = '\0'
	
	PDEBUG("COUNT: %d",count);

	int val =0;
	val = copy_to_user(buf,ret_buffer->buffptr,ret_buffer->size);

	PDEBUG("String copied %s",ret_buffer->buffptr);

	*f_pos += ret_buffer->size;
	
	retval += ret_buffer->size;
	
	PDEBUG("mutex unlocked in READ");
	//TODO: handle read
out:	
	mutex_unlock(&dev->lock);
	return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
    
    ssize_t retval = -ENOMEM;            
	PDEBUG("write start");
	//mutex_lock_interruptible(&lock);
	struct aesd_dev *dev = filp->private_data;
	mutex_lock_interruptible(&dev->lock);
	char *temp_buff = NULL;
	
	temp_buff = ( char *)kmalloc(count*sizeof(char),GFP_KERNEL);	

	
	struct aesd_buffer_entry entry;
	
	entry.size = count;

	copy_from_user(temp_buff,buf,count);
	
	temp_buff[count] = '\0';
	
	PDEBUG("String copied %s",temp);

	if(temp_buff[count-1] != '\n' && flag ==0 )
	{
		Temp_Array = (char *)kmalloc(100*sizeof(char),GFP_KERNEL);
	}

	if(temp_buff[count-1] == '\n' && flag ==0 )
	{
		entry.buffptr = temp_buff;
		aesd_circular_buffer_add_entry(&(dev->c_buffer),&entry);
		
	
		int k=0;
		for(k=0;k<10;k++)
		{
			printk(" ***************Buffer value at position %d is %s",k,dev->c_buffer.entry[k].buffptr);
		}
	}

	else 
	{
		if(Temp_Array != NULL)
		{
			memcpy(Temp_Array+sum,temp_buff,count);
			PDEBUG("Data copied to temp Temp_Array : %s ",temp_buff);
			kfree(temp_buff);
		}
		sum = sum + count;
		flag = 1;

	if(Temp_Array[sum-1] == '\n')
	{
		entry.buffptr = Temp_Array;
		entry.size = sum;
		int i;
		for( i =0;i<=(sum +1);i++)
		{
			PDEBUG(">>>>>>>>>Array is '%c'",Temp_Array[i]);
		}
		aesd_circular_buffer_add_entry(&(dev->c_buffer),&entry);
		int k =0;
		for(k=0;k<10;k++)
		{
			printk(" ***************Buffer value at position %d is %s",k,dev->c_buffer.entry[k].buffptr);
		}
		retval = sum;
		PDEBUG("RETVAL : %d",retval);
		int j ; 
		sum = 0;
		temp = 0;
		flag = 0;
		goto out;
	}

	}

	retval = count;
out:
	mutex_unlock(&dev->lock);
	return retval;
	
}

struct file_operations aesd_fops = {
	.owner =    THIS_MODULE,
	.read =     aesd_read,
	.write =    aesd_write,
	.open =     aesd_open,
	.release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
	int err, devno = MKDEV(aesd_major, aesd_minor);

	cdev_init(&dev->cdev, &aesd_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &aesd_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	if (err) {
		printk(KERN_ERR "Error %d adding aesd cdev", err);
	}
	return err;
}



int aesd_init_module(void)
{
	dev_t dev = 0;
	int result=0,i=0;
	result = alloc_chrdev_region(&dev, aesd_minor, 1,
			"aesdchar");
	aesd_major = MAJOR(dev);
	dev = MKDEV(aesd_major, aesd_minor);
	PDEBUG("*********************************************************************");
	if (result < 0) {
		printk(KERN_WARNING "Can't get major %d\n", aesd_major);
		return result;
	}
	memset(&aesd_device,0,sizeof(struct aesd_dev));

	
	 //TODO: initialize the AESD specific portion of the device
	aesd_device.c_buffer.in_offs = 0;
	aesd_device.c_buffer.out_offs =0;
	aesd_device.c_buffer.full=0;

	result = aesd_setup_cdev(&aesd_device);

	if( result ) {
		unregister_chrdev_region(dev, 1);
	}
	return result;

}

void aesd_cleanup_module(void)
{
	PDEBUG("Cleanup Module");
	dev_t devno = MKDEV(aesd_major, aesd_minor);
	int i;
	struct aesd_dev  *dev;
	cdev_del(&aesd_device.cdev);

	/**
	 * TODO: cleanup AESD specific poritions here as necessary
	 */

	unregister_chrdev_region(devno, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
