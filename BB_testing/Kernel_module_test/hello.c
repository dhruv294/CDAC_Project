#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

int __init ds_init(void)
{
 printk(KERN_INFO "%s:hello from inside kernel\n",THIS_MODULE->name);	
 return 0;
}

void __exit ds_exit(void)
{
 printk(KERN_INFO "%s:hello from inside kernel\n",THIS_MODULE->name);	
	
}

module_init(ds_init);
module_exit(ds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xyz <xyz@abc.com>");
MODULE_DESCRIPTION("TEST MODULE FOR beagale bone");
