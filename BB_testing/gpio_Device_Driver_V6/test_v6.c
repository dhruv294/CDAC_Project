#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/delay.h>      // Using this header for the msleep() function
#include <linux/timer.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>

static struct timer_list myTimer;
static unsigned int gpioLED = 49;           ///< Default GPIO for the LED is 49
static unsigned int s_BlinkPeriod = 5;  //Default 5 sec
static unsigned int irqNumber;
static unsigned int Interrupt_flag = 0; 
static unsigned int gpioButton = 115; 

struct cdev mycdev;
static dev_t first;
static int major = 250;

static int devcnt=2;
static dev_t devno; 

module_param(gpioLED, uint, S_IRUGO);       ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(gpioLED, " GPIO LED number (default=49)");     ///< parameter description


module_param(s_BlinkPeriod, uint, S_IRUGO);   ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(s_BlinkPeriod, " LED blink period in sec (min=1, default=5, max=10000)");

static char ledName[7] = "ledXXX";          ///< Null terminated default string -- just in case
static bool ledOn = 0; 

static irqreturn_t gpio_irq_handler(unsigned int irq, void *dev_id);
static void timer_set(void);

void MyTimerCallback(unsigned long data)
{
	printk(KERN_INFO "EBB LED: next %u seconds!\n",s_BlinkPeriod);
	ledOn = !ledOn;
	gpio_direction_output(gpioLED, ledOn);
	mod_timer(&myTimer,jiffies+s_BlinkPeriod*HZ);
}


static int gpio_open(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "%s: gpio_open() is called.\n", THIS_MODULE->name);
	return 0;
}

static int gpio_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "%s: gpio_close() is called.\n", THIS_MODULE->name);
	return 0;
}

ssize_t gpio_write(struct file *pfile, const char __user *pbuf, size_t size, loff_t *pfpos)
{
	printk(KERN_INFO "%s: gpio_read() is called.\n", THIS_MODULE->name);
	return size;
}

ssize_t gpio_read(struct file *pfile, char __user *pbuf, size_t size, loff_t *pfpos)
{
	int max_bytes, bytes_to_read, nbytes;
	char timer[10];
	max_bytes = 10- *pfpos;
	printk(KERN_INFO "%s: gpio_read() is called.\n", THIS_MODULE->name);
	bytes_to_read = size < max_bytes? size : max_bytes;
	printk(KERN_INFO "%s: bytes_to_read determined.\n", THIS_MODULE->name);

	if(bytes_to_read == 0)
	{
		printk(KERN_INFO "%s: No data left on device.\n", THIS_MODULE->name);
		return 0;	
	}
	sprintf(timer,"%u",s_BlinkPeriod);
	printk(KERN_INFO "%s: conversion done.\n", THIS_MODULE->name);
	printk(KERN_INFO "%s: timer :%s\n", THIS_MODULE->name,timer);
	
	nbytes = bytes_to_read - copy_to_user(pbuf,timer,bytes_to_read);
	printk(KERN_INFO "%s: copies to user done.\n", THIS_MODULE->name);
	*pfpos = *pfpos + nbytes;
	return 0;
}

struct file_operations gpio_fops = {
	.owner = THIS_MODULE,
	.open = gpio_open,
	.release = gpio_close,
	.write = gpio_write,
	.read = gpio_read
};


static ssize_t set_period_callback(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	long period_value = 0;
	if (kstrtol(buf, 10, &period_value) < 0)
		return -EINVAL;
	if (period_value < 1)	//Safety check
		return - EINVAL;

	s_BlinkPeriod = period_value;
	printk(KERN_INFO "%s: Blink period changed to %u\n",THIS_MODULE->name,s_BlinkPeriod);
	return count;
}


void timer_config(void)
{  
	init_timer(&myTimer);
	myTimer.data = 0;
	myTimer.function = MyTimerCallback;
	myTimer.expires = jiffies+s_BlinkPeriod*HZ;
	add_timer(&myTimer); 
}


static DEVICE_ATTR(period, S_IRWXU | S_IRWXG, NULL, set_period_callback);

static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;

static int __init ebbLED_init(void){

	int result;
	printk(KERN_INFO "EBB LED: Hello from the EBB LED LKM!\n");
	ledOn = 0;
	gpio_request(gpioLED, "sysfs");          // gpioLED is 49 by default, request it
	gpio_direction_output(gpioLED, ledOn);   // Set the gpio to be in output mode and turn on
	//gpio_export(gpioLED, false);  // causes gpio49 to appear in /sys/class/gpio
	// the second argument prevents the direction from being changed
	gpio_request(gpioButton, "sysfs");       // Set up the gpioButton
	gpio_direction_input(gpioButton);        // Set the button GPIO to be an input
	gpio_set_debounce(gpioButton, 200);      // Debounce the button with a delay of 200ms


	first = MKDEV(major, 0);
	result = alloc_chrdev_region(&first, 0, devcnt, "DESD_gpio");
	if(result != 0)
	{
		printk(KERN_INFO "%s: alloc_chrdev_region() is failed.\n", THIS_MODULE->name);
		goto alloc_chrdev_region_failed;
	}

	major = MAJOR(first);
	printk(KERN_INFO "%s: alloc_chrdev_region() allocated major num = %d.\n", THIS_MODULE->name, major);

	s_pDeviceClass = class_create(THIS_MODULE, "DESD_gpio_class");
	if(s_pDeviceClass == NULL)
	{
		printk(KERN_INFO "EBB LED: Device class creation failed!\n");
		goto class_creation_failed;

	}
	devno = MKDEV(major,0);
	if((device_create(s_pDeviceClass, NULL, devno, NULL, "GPIOLED")) == NULL)
	{
		printk(KERN_INFO "EBB LED:gpioLED creation failed!\n");
		goto device_creation_failed;

	}

	devno = MKDEV(major,1);
	if((device_create(s_pDeviceClass, NULL, devno, NULL, "GPIOButton")) == NULL)
	{
		printk(KERN_INFO "EBB LED: gpioButton creation failed!\n");
		goto button_device_creation_failed;

	}
	result = device_create_file(s_pDeviceObject, &dev_attr_period);
	if(result != 0)
	{
		printk(KERN_INFO "EBB LED: Device file creation failed!\n");
		goto file_creation_failed;

	}

	cdev_init(&mycdev, &gpio_fops);
	result = cdev_add(&mycdev, first, 1);

	if(result != 0)
	{
		printk(KERN_INFO "%s: cdev_add() is failed.\n", THIS_MODULE->name);
		goto cdev_failed;

	}
	irqNumber = gpio_to_irq(gpioButton);
	printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);
	result = request_irq(irqNumber,(irq_handler_t) gpio_irq_handler,IRQF_TRIGGER_RISING,"gpio_handler",NULL);
	if(result != 0)
	{
		printk(KERN_INFO "%s: request-irq() failed.\n", THIS_MODULE->name);
		goto irq_request_failed;

	}
	printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);
	return 0;

irq_request_failed:
	cdev_del(&mycdev);
cdev_failed:
	device_remove_file(s_pDeviceObject, &dev_attr_period);
file_creation_failed:
button_device_creation_failed:
	device_destroy(s_pDeviceClass, first);
device_creation_failed:
	class_destroy(s_pDeviceClass);
class_creation_failed:
	unregister_chrdev_region(first, 1);
alloc_chrdev_region_failed:
	del_timer_sync(&myTimer);
	gpio_free(gpioLED);                      // Free the LED GPO
	return -1;

}

static void __exit ebbLED_exit(void){
	int i;
	free_irq(irqNumber, NULL);
	cdev_del(&mycdev);
	device_remove_file(s_pDeviceObject, &dev_attr_period);
	for(i = 0; i < devcnt; i++)
	{
		devno = MKDEV(major,i);
		device_destroy(s_pDeviceClass, devcnt);
	}
	class_destroy(s_pDeviceClass);
	unregister_chrdev_region(first, 1);
	if(Interrupt_flag == 1)
	{
	del_timer_sync(&myTimer);
	}
	gpio_set_value(gpioLED, 0);              // Turn the LED off, indicates device was unloaded
	gpio_free(gpioLED);                      // Free the LED GPIO
	gpio_free(gpioButton);           
	printk(KERN_INFO "EBB LED: Goodbye from the EBB LED LKM!\n");
}

static irqreturn_t gpio_irq_handler(unsigned int irq, void *dev_id){
	printk(KERN_INFO "%s: interrupt received!\n",THIS_MODULE->name);
	ledOn = !ledOn;// Invert the LED state on each button press
	if(Interrupt_flag == 0)
	{
            gpio_set_value(gpioLED, ledOn);          // Set the physical LED accordingly
	    	Interrupt_flag = 1;
	}
	else
	{
	gpio_set_value(gpioLED, 0);          // Set the physical LED accordinglyi
	Interrupt_flag = 0;
	}
	timer_set();
	return IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

static void timer_set(void)
{
  if(Interrupt_flag == 0)
	{
	del_timer_sync(&myTimer);
	}
	else
	{
	timer_config();
	}
}
module_init(ebbLED_init);
module_exit(ebbLED_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abc");
MODULE_DESCRIPTION("A simple Linux LED driver for the BBB");
MODULE_VERSION("0.1");
