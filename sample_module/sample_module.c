#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("");


int sample_module_init(void)
{
printk(KERN_ALERT "Sample Module: in %s function\n",__FUNCTION__);
return 0;
}

void sample_module_exit(void)
{
printk(KERN_ALERT "Sample Module: in %s function\n",__FUNCTION__);
}

module_init(sample_module_init);
module_exit(sample_module_exit);