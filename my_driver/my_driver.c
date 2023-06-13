#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/slab.h>
#include<linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nathan Palmer");

static char *buffer;
size_t buff_size = 1024;

ssize_t my_read(struct file *filp, char __user *user_buff, size_t count, loff_t *offp) {
    size_t bytes_remaining;
    
    if (count < buff_size) {
        bytes_remaining = count - *offp;
    } else {
        bytes_remaining = buff_size - *offp;
    }

    if (bytes_remaining == 0) {
        *offp = 0;
        return 0;
    }

    int copy_error = copy_to_user(user_buff, buffer + *offp, bytes_remaining);

    if (copy_error < 0) {
        return -EFAULT;
    }

    *offp += bytes_remaining;
    return bytes_remaining;
};

ssize_t my_write(struct file *filp, const char __user *user_buff, size_t count, loff_t *offp) {
    size_t write_end = count + *offp;
    bool is_overflow;

    if (write_end > buff_size) {
        is_overflow = true;
        count = buff_size - *offp;
    } else {
        is_overflow = false;
    }
    
    int write_error = copy_from_user(buffer + *offp, user_buff, count);
    
    if (write_error < 0) {
        return -EFAULT;
    }

    if (is_overflow) {
        return -1;
    } else {
        return (count - write_error);
    }
};

loff_t my_llseek(struct file *filp, loff_t offset, int whence) {
    loff_t updated_pos;
    loff_t cur_pos = filp->f_pos;
    loff_t file_dim = i_size_read(file_inode(filp));

    switch (whence) {
        case SEEK_SET:
            updated_pos = offset;
            break;
        case SEEK_CUR:
            updated_pos = offset + cur_pos;
            break;
        case SEEK_END:
            updated_pos = file_dim + offset;
            break;
    }

    if (updated_pos < 0) {
        return -EINVAL;
    }

    filp->f_pos = updated_pos;
    return updated_pos;
};

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .llseek = my_llseek, 
    .read = my_read, 
    .write = my_write
};

int module_init_func(void) {
    buffer = kmalloc(1024 * sizeof(char), GFP_KERNEL);
    memset(buffer, 0, 1024 * sizeof(char));
    register_chrdev(242, "simple_character_device", &fops);
    printk(KERN_ALERT "Sample Module: in %s function\n",__FUNCTION__);
    return 0;
};

void module_exit_func(void) {
    kfree(buffer);
    unregister_chrdev(242, "simple_character_device");
    printk(KERN_ALERT "Sample Module: in %s function\n",__FUNCTION__);
};

module_init(module_init_func);
module_exit(module_exit_func);