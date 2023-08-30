#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 756

/* Define device_buffer and other global data structures you will need here */
char* device_buffer;
int device_number = 240; 
char device_name[] = "simple_character_device";
int open = 0;
int close = 0;

ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t 
length, loff_t *offset)
{
    int copy;
    printk(KERN_INFO "PA2 Driver Read\n");
    /* *buffer is the userspace buffer to where you are writing the data you want
    to be read from the device file*/
    /* length is the length of the userspace buffer*/
    /* offset will be set to current position of the opened file after read*/
    /* copy_to_user function: source is device_buffer and destination is the 
    userspace buffer *buffer */
    if(length + *offset < 0){
        printk(KERN_ALERT "ERROR: Trying to read before the beginning of device buffer\n");
        return -1;
    }
    if (length + *offset > BUFFER_SIZE){
        printk(KERN_ALERT "ERROR: Trying to read beyond the end of device buffer\n");
        return -1;
    }
    // implement copy_to_user using current position from device buffer as source
    // copy_to_user will return num bytes not copied (0 if all are copied)
    copy = copy_to_user(buffer, device_buffer + *offset, length);
    if (copy != 0){
        printk(KERN_ALERT "ERROR: Buffer overflow\n");
        return -1;
    }
    else{
        // update offset pointer:
        *offset = *offset + length;
        printk(KERN_INFO "Read Successful!\nNumber of bytes read: %lu\n", length);
    }
    return length;
}

ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, 
size_t length, loff_t *offset)
{
    int copy;
    printk(KERN_INFO "PA2 Driver Write\n");
    /* *buffer is the userspace buffer where you are writing the data you want to
    be written in the device file*/
    /* length is the length of the userspace buffer*/
    /* current position of the opened file*/
    /* copy_from_user function: destination is device_buffer and source is the 
    userspace buffer *buffer */
    if(length + *offset < 0){
        printk(KERN_ALERT "ERROR: Trying to write before the beginning of device buffer\n");
        return -1;
    }
    if (length + *offset > BUFFER_SIZE){
        printk(KERN_ALERT "ERROR: Trying to write beyond the end of device buffer\n");
        return -1;
    }
    // implement copy_from_user using current position from device buffer as source
    // copy_from_user will return num bytes not copied (0 if all are copied)
    copy = copy_from_user(device_buffer + *offset, buffer, length);
    if (copy != 0){
        printk(KERN_ALERT "ERROR: Buffer overflow\n");
        return -1;
    }
    else{
        // update offset pointer:
        *offset = *offset + length;
        printk(KERN_INFO "Write Successful!\nNumber of bytes written: %lu\n", length);
    }
    return length;
}

int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
    /* print to the log file that the device is opened and also print the number 
    of times this device has been opened until now*/
    printk(KERN_INFO "PA2 Driver Opened\nDevice opened %d time(s)\n", open++);
    return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
    /* print to the log file that the device is closed and also print the number 
    of times this device has been closed until now*/
    printk(KERN_INFO "PA2 Driver Closed\nDevice closed %d time(s)\n", close++);
    return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
    /* Update open file position according to the values of offset and whence */
    int pos, buf_overflow;
    printk(KERN_INFO "PA2 Driver Seek\n");
    switch (whence){
        case 0:
            pos = offset;
            break; 
        case 1:
            pos = pfile->f_pos + offset; 
            break;
        case 2: 
            pos = BUFFER_SIZE + offset;
            break;
        default: 
            printk(KERN_ALERT "ERROR: Invalid whence value\n");
            return -1;
    }
    buf_overflow = BUFFER_SIZE - pos; 
    while (buf_overflow > BUFFER_SIZE || buf_overflow <= 0){
        buf_overflow = BUFFER_SIZE - pos; 
        // if seek is before beginning of device buffer, 
        // pos is (-) and buf_overflow will therefore be greater than BUFFER_SIZE
        // Thus, reset pos to equivalent offset of end of buffer
        if (buf_overflow > BUFFER_SIZE){
            pos = BUFFER_SIZE + pos;
        }
        // if seek is at end of of device buffer, 
        // pos > BUFFER_SIZE and buf_overflow will therefore be less than 0
        // reset pos to equivalent offset of beginning of buffer
        if (buf_overflow <= 0){
            pos = pos - BUFFER_SIZE;
        }
    }
    pfile->f_pos = pos;
    return pos;
}

struct file_operations pa2_char_driver_file_operations = {
    .owner   = THIS_MODULE,
    /* add the function pointers to point to the corresponding file operations. 
    look at the file fs.h in the linux souce code*/
    .open    = pa2_char_driver_open,
    .release = pa2_char_driver_close,
    .read    = pa2_char_driver_read,
    .write   = pa2_char_driver_write,
    .llseek  = pa2_char_driver_seek
};

static int pa2_char_driver_init(void)
{
    /* print to the log file that the init function is called.*/
    printk(KERN_INFO "PA2 Driver Init\n");
    // initialize buffer: 
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    /* register the device */
    // device num = 240, device name = "simple_character_device", pointer to file op struct 
    register_chrdev(device_number, device_name, &pa2_char_driver_file_operations);
    return 0;
}

static void pa2_char_driver_exit(void)
{
    /* print to the log file that the exit function is called.*/
    printk(KERN_INFO "PA2 Driver Exit\n");
    // free device buffer memory
    kfree(device_buffer);
    /* unregister  the device using the register_chrdev() function. */
    unregister_chrdev(device_number, device_name);
}

/* add module_init and module_exit to point to the corresponding init and exit 
function*/
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);