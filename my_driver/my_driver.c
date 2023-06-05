#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

MODULE_AUTHOR("Nathan Palmer");
MODULE_LICENSE("GPL");

static char *kern_buff;
size_t buffer_size = 1024;



ssize_t my_read(struct file *filp, char __user *user_buff, size_t count, loff_t *offp) {
    size_t bytes_left = buffer_size - *offp;
    if (bytes_left == 0) {
        return 0;
    }

    printk(KERN_INFO "count: %d", count);
    printk(KERN_INFO "offp: %lld", *offp);

    int not_copied = copy_to_user(user_buff, kern_buff + *offp, bytes_left);
    *offp += bytes_left;

    return bytes_left;
};


int my_open (struct inode *, struct file *);

ssize_t my_write(struct file *filp, const char __user *user_buff, size_t count, loff_t *offp) {
    printk(KERN_INFO "In write\n");
    int write_ret = copy_from_user(kern_buff, user_buff, 1024);
    if (write_ret == 0) {  
        printk(KERN_INFO "Write Successful\n");
        //data_size = count;  // Update the size of the data
        printk(KERN_INFO "%s\n", kern_buff);  
    }
    return (count - write_ret);
};

loff_t my_llseek (struct file *, loff_t, int);

int my_release(struct inode*, struct file*);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    //.llseek = my_llseek, 
    .read = my_read, 
    .write = my_write, 
    //.open = my_open 
};

int sample_module_init(void)
{
	kern_buff = kmalloc(1100 * sizeof(char), GFP_KERNEL);
	register_chrdev (0, "simple_character_device", &fops);
	printk(KERN_ALERT "Sample Module: in %s function\n",__FUNCTION__);
	return 0;
};

void sample_module_exit(void)
{
	kfree(kern_buff);
	unregister_chrdev (0, "simple_character_device");
	printk(KERN_ALERT "Sample Module: in %s function\n",__FUNCTION__);
};

module_init(sample_module_init);
module_exit(sample_module_exit);