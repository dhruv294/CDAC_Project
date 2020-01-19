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
#include <linux/slab.h>


static unsigned int gpioLED = 49;           ///< Default GPIO for the LED is 49
static unsigned int irqNumber;

static unsigned int interrupt_count;
static unsigned int gpioButton = 115; 

typedef struct my_priv_struct
{
	struct cdev mycdev;
	int dev_num;
}my_priv_t;

static my_priv_t *devices;
static dev_t first;
static int major = 250;

static int devcnt=2;
static dev_t devno; 

module_param(gpioLED, uint, S_IRUGO);       ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(gpioLED, " GPIO LED number (default=49)");     ///< parameter description


static bool ledOn = 0; 

static irqreturn_t gpio_irq_handler(unsigned int irq, void *dev_id);


static int gpio_open(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "%s: gpio_open() is called.\n", THIS_MODULE->name);
	pfile->private_data = container_of(pinode->i_cdev,my_priv_t,mycdev);
	return 0;
}

static int gpio_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "%s: gpio_close() is called.\n", THIS_MODULE->name);
	return 0;
}

ssize_t gpio_write(struct file *pfile, const char __user *pbuf, size_t size, loff_t *pfpos)
{
	my_priv_t *dev = (my_priv_t*)pfile->private_data;
	int max_bytes, bytes_to_write, nbytes;
	char value[10];
	static unsigned int received_value;
	printk(KERN_INFO "%s: gpio_write() is called.\n", THIS_MODULE->name);
	max_bytes = 10 - *pfpos;
	bytes_to_write = size < max_bytes? size : max_bytes;
	if(bytes_to_write == 0)
	{
		printk(KERN_INFO "%s: No space left on device.\n", THIS_MODULE->name);
		return -ENOSPC;
	}
	nbytes = bytes_to_write - copy_from_user(value, pbuf, bytes_to_write);
	sscanf(value,"%u",&received_value);
	printk(KERN_INFO "%s:received value: %u",THIS_MODULE->name,received_value);
	if(dev->dev_num == 0)
	{
		ledOn = 0;
		gpio_direction_output(gpioLED, ledOn); 
		gpio_free(gpioLED); 
		gpioLED = received_value;
		gpio_request(gpioLED, "sysfs");        
		gpio_direction_output(gpioLED, ledOn);   
		printk(KERN_INFO "%s: GPIOLED opened!!!",THIS_MODULE->name);

	}
	else
	{
		int result;
	//	gpio_direction_output(gpioButton, 0);
		free_irq(irqNumber, NULL);
		gpio_free(gpioButton); 
		gpio_request(gpioButton, "sysfs");
		gpioButton = received_value;
		gpio_direction_input(gpioButton); 
		irqNumber = gpio_to_irq(gpioButton);
		printk(KERN_INFO "GPIO_TEST: New IRQ: %d\n", irqNumber);
		result = request_irq(irqNumber,(irq_handler_t) gpio_irq_handler,IRQF_TRIGGER_RISING,"gpio_handler",NULL);
		printk(KERN_INFO "%s: GPIOButton opened!!!",THIS_MODULE->name);
	}
	return nbytes;
}

ssize_t gpio_read(struct file *pfile, char __user *pbuf, size_t size, loff_t *pfpos)
{
	my_priv_t *dev = (my_priv_t*)pfile->private_data;
	int max_bytes, bytes_to_read, nbytes;
	char status[100];
	int led_status = 0;
	max_bytes = 50- *pfpos;
	printk(KERN_INFO "%s: gpio_read() is called.\n", THIS_MODULE->name);
	bytes_to_read = size < max_bytes? size : max_bytes;
	printk(KERN_INFO "%s: bytes_to_read determined.\n", THIS_MODULE->name);
	led_status = ledOn?1:0;

	if(bytes_to_read == 0)
	{
		printk(KERN_INFO "%s: No data left on device.\n", THIS_MODULE->name);
		return 0;	
	}
	if(dev->dev_num == 0)
	{
	sprintf(status,"led staus: %d, LED gpio: %u",led_status,gpioLED);
	printk(KERN_INFO "%s: status :%s\n", THIS_MODULE->name,status);
	}
	else
	{
		sprintf(status,"interrupt count: %u, Button gpio: %u",interrupt_count,gpioButton);
	}

	nbytes = bytes_to_read - copy_to_user(pbuf,status,bytes_to_read);
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


static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;

static int __init ebbLED_init(void){

	int result;
	int i;
	printk(KERN_INFO "EBB LED: Hello from the EBB LED LKM!\n");
	ledOn = 0;
	gpio_request(gpioLED, "sysfs");          // gpioLED is 49 by default, request it
	gpio_direction_output(gpioLED, ledOn);   // Set the gpio to be in output mode and turn on
	//gpio_export(gpioLED, false);  // causes gpio49 to appear in /sys/class/gpio
	// the second argument prevents the direction from being changed
	gpio_request(gpioButton, "sysfs");       // Set up the gpioButton
	gpio_direction_input(gpioButton);        // Set the button GPIO to be an input
	gpio_set_debounce(gpioButton, 200);      // Debounce the button with a delay of 200ms

	devices = (my_priv_t*)kmalloc(devcnt * sizeof(my_priv_t), GFP_KERNEL);
	if(devices == NULL)
	{
		printk(KERN_INFO "%s: kmalloc() is failed.\n",THIS_MODULE->name);
		result = -ENOMEM;
		goto kmalloc_failed;
	}
	printk(KERN_INFO "%s: kmalloc() allocated device private struct array.\n", THIS_MODULE->name);

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
	if((s_pDeviceObject = device_create(s_pDeviceClass, NULL, devno, NULL, "GPIOLED")) == NULL)
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
	for(i =0; i < devcnt; i++)
	{
		devices[i].dev_num = i;
		cdev_init(&devices[i].mycdev, &gpio_fops);
		devno = MKDEV(major,i);
		result = cdev_add(&devices[i].mycdev, devno, 1);
		if(result != 0)
		{
			printk(KERN_INFO "%s: cdev_add() is failed.\n", THIS_MODULE->name);
			goto cdev_failed;

		}
		printk(KERN_INFO "%s: cdev_add() is successful.\n", THIS_MODULE->name);
	}


	irqNumber = gpio_to_irq(gpioButton);
	printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);
	result = request_irq(irqNumber,(irq_handler_t) gpio_irq_handler,IRQF_TRIGGER_RISING,"gpio_handler",NULL);
	printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);
	return 0;

cdev_failed:
	for(i=i-1; i>=0; i--)
		cdev_del(&devices[i].mycdev);	
	i = devcnt;
button_device_creation_failed:
	for(i = i -1; i >= 0; i--)
	{
		first = MKDEV(major,i);
		device_destroy(s_pDeviceClass, first);
	}
device_creation_failed:
	class_destroy(s_pDeviceClass);
class_creation_failed:
	unregister_chrdev_region(first, devcnt);
alloc_chrdev_region_failed:  
	kfree(devices);
kmalloc_failed:
	gpio_free(gpioLED);// Free the LED GPO
	gpio_free(gpioButton);
	return -1;

}

static void __exit ebbLED_exit(void){
	int i = devcnt;
	free_irq(irqNumber, NULL);
	for(i=i-1; i>=0; i--)
		cdev_del(&devices[i].mycdev);	
	for(i = 0; i < devcnt; i++)
	{
		devno = MKDEV(major,i);
		device_destroy(s_pDeviceClass, devno);
		printk(KERN_INFO "%s: device_destroy() delete device file.\n", THIS_MODULE->name);
	}
	class_destroy(s_pDeviceClass);
	unregister_chrdev_region(first, devcnt);
	kfree(devices);
	gpio_set_value(gpioLED, 0);              // Turn the LED off, indicates device was unloaded
	gpio_direction_output(gpioLED, 0);
	gpio_free(gpioLED);                      // Free the LED GPIO
	gpio_direction_output(gpioButton,0);
	gpio_free(gpioButton);           
	printk(KERN_INFO "EBB LED: Goodbye from the EBB LED LKM!\n");
}

static irqreturn_t gpio_irq_handler(unsigned int irq, void *dev_id){
	printk(KERN_INFO "%s: interrupt received!\n",THIS_MODULE->name);
	ledOn = !ledOn;
	gpio_set_value(gpioLED, ledOn);          // Set the physical LED accordingly
		
	interrupt_count++;	
	return IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}


module_init(ebbLED_init);
module_exit(ebbLED_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abc");
MODULE_DESCRIPTION("A simple Linux LED driver for the BBB");
MODULE_VERSION("0.1");
