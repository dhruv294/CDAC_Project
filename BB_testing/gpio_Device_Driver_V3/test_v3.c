#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/delay.h>      // Using this header for the msleep() function
#include<linux/timer.h>
#include <linux/device.h>
#include <asm/uaccess.h>

static struct timer_list myTimer;
static unsigned int gpioLED = 49;           ///< Default GPIO for the LED is 49
static unsigned int s_BlinkPeriod = 5;  //Default 5 sec

module_param(gpioLED, uint, S_IRUGO);       ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(gpioLED, " GPIO LED number (default=49)");     ///< parameter description

static char ledName[7] = "ledXXX";          ///< Null terminated default string -- just in case
static bool ledOn = 0; 

module_param(s_BlinkPeriod, uint, S_IRUGO);   ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(s_BlinkPeriod, " LED blink period in sec (min=1, default=5, max=10000)");


static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;

void MyTimerCallback(unsigned long data)
{
	printk(KERN_INFO "EBB LED: next %u seconds!\n",s_BlinkPeriod);
	ledOn = !ledOn;
	gpio_direction_output(gpioLED, ledOn);
	mod_timer(&myTimer,jiffies+s_BlinkPeriod*HZ);
}

void timer_config(void)
{  
    init_timer(&myTimer);
	myTimer.data = 0;
	myTimer.function = MyTimerCallback;
	myTimer.expires = jiffies+s_BlinkPeriod*HZ;
	add_timer(&myTimer); 
}

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


static DEVICE_ATTR(period, S_IRWXU | S_IRWXG, NULL, set_period_callback);

static ssize_t export_gpio(struct class* class, struct class_attribute* attr, const char* buf, size_t count)
{
	int result;
	printk(KERN_INFO "EBB LED: in export gpio file!\n");
	sscanf(buf,"%d",&gpioLED);
	ledOn = true;
	gpio_request(gpioLED, "sysfs");          // gpioLED is 49 by default, request it
	gpio_direction_output(gpioLED, ledOn); 
	s_pDeviceObject = device_create(s_pDeviceClass, NULL, gpioLED, NULL, "GPIO%d",gpioLED);
	if(s_pDeviceObject == NULL)
	{
		printk(KERN_INFO "EBB LED: GPIO%d Device creation failed!\n",gpioLED);
		goto device_creation_failed;
		
	}
	result = device_create_file(s_pDeviceObject, &dev_attr_period);
	if(result != 0)
	{
			printk(KERN_INFO "EBB LED: GPIO%d Device period file creation failed!\n",gpioLED);
			goto file_creation_failed;
			
	}
	return 0;

file_creation_failed:
	device_destroy(s_pDeviceClass, 0);
	
device_creation_failed:
	gpio_free(gpioLED);                      // Free the LED GPIO
	return -1;
	
}
static ssize_t unexport_gpio(struct class* class, struct class_attribute* attr, const char* buf, size_t count)
{
	printk(KERN_INFO "EBB LED: in unexport gpio file\n");
	sscanf(buf,"%d",&gpioLED);
	gpio_set_value(gpioLED, 0);              // Turn the LED off, indicates device was unloaded
	//gpio_unexport(gpioLED);                  // Unexport the Button GPIO
	gpio_free(gpioLED);                      // Free the LED GPIO
	device_remove_file(s_pDeviceObject, &dev_attr_period);
	device_destroy(s_pDeviceClass, gpioLED);
	printk(KERN_INFO "EBB LED: Goodbye from the EBB LED LKM!\n");
	
}


static CLASS_ATTR(exportgpio, S_IRWXU | S_IRWXG, NULL, export_gpio);
static CLASS_ATTR(unexportgpio, S_IRWXU | S_IRWXG, NULL, unexport_gpio);


static int __init ebbLED_init(void){
	
	int result;
	printk(KERN_INFO "EBB LED: Hello from the EBB DESD_GPIO LKM!\n");
	//gpio_export(gpioLED, false);  // causes gpio49 to appear in /sys/class/gpio
	// the second argument prevents the direction from being changed
	timer_config();

	s_pDeviceClass = class_create(THIS_MODULE, "DESD_GPIO");
	if(s_pDeviceClass == NULL)
	{
		printk(KERN_INFO "EBB LED: Device class creation failed!\n");
		goto class_creation_failed;
		
	}
	result = class_create_file(s_pDeviceClass, &class_attr_exportgpio);
	if(result != 0)
	{
			printk(KERN_INFO "EBB LED: class file_gpio_export failed creation failed!\n");
			goto file_gpio_export;
			
	}
	result = class_create_file(s_pDeviceClass, &class_attr_unexportgpio);
	if(result != 0)
	{
			printk(KERN_INFO "EBB LED: class file_gpio_unexport failed creation failed!\n");
			goto file_gpio_unexport;
			
	}
	return 0;

file_gpio_unexport:
	class_remove_file(s_pDeviceClass, &class_attr_exportgpio);
file_gpio_export:
	class_destroy(s_pDeviceClass);
	
class_creation_failed:
	del_timer_sync(&myTimer);
	return -1;

}


static void __exit ebbLED_exit(void){
	//gpio_unexport(gpioLED);                  // Unexport the Button GPIO
	del_timer_sync(&myTimer);
    class_remove_file(s_pDeviceClass, &class_attr_unexportgpio);
    class_remove_file(s_pDeviceClass, &class_attr_exportgpio);
	class_destroy(s_pDeviceClass);
	printk(KERN_INFO "EBB LED: Goodbye from the EBB LED LKM!\n");
}

module_init(ebbLED_init);
module_exit(ebbLED_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abc");
MODULE_DESCRIPTION("A simple Linux LED driver LKM for the BBB");
MODULE_VERSION("0.1");
