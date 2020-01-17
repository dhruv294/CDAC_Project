#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/delay.h>      // Using this header for the msleep() function
#include<linux/timer.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>

static struct timer_list myTimer;
static unsigned int gpioLED = 49;           ///< Default GPIO for the LED is 49
static unsigned int gpioButton = 115;           ///< Default GPIO for the Button is 115
static unsigned int irqNumber;  
static unsigned int s_BlinkPeriod = 5;  //Default 5 sec
static dev_t first;
static int major = 250;

module_param(gpioLED, uint, S_IRUGO);       ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(gpioLED, " GPIO LED number (default=49)");     ///< parameter description

static char ledName[7] = "ledXXX";          ///< Null terminated default string -- just in case
static bool ledOn = 0; 

static int devcnt=2;
static dev_t devno; 
static struct class *s_pDeviceClass;

static irqreturn_t gpio_irq_handler(unsigned int irq, void *dev_id);

static int __init ebbLED_init(void){

	int result;
	printk(KERN_INFO "EBB LED: Hello from the EBB LED LKM!\n");
	ledOn = true;
	gpio_request(gpioLED, "sysfs");          // gpioLED is 49 by default, request it
	gpio_direction_output(gpioLED, ledOn);   // Set the gpio to be in output mode and turn on
	//gpio_export(gpioLED, false);  // causes gpio49 to appear in /sys/class/gpio
	// the second argument prevents the direction from being changed
	gpio_request(gpioButton, "sysfs");       // Set up the gpioButton
	gpio_direction_input(gpioButton);        // Set the button GPIO to be an input
	gpio_set_debounce(gpioButton, 200);      // Debounce the button with a delay of 200ms
	first = MKDEV(major,0);
	result = alloc_chrdev_region(&first,0,devcnt,"GPIODRIVER");
	if(result != 0)
	{
		printk(KERN_INFO "%s: alloc_chrdev_region() is failed.\n", THIS_MODULE->name);
		result = -1;
		goto alloc_chrdev_region_failed;
	}

	major = MAJOR(first);
	printk(KERN_INFO "%s: alloc_chrdev_region() allocated major num = %d.\n", THIS_MODULE->name, major);
	s_pDeviceClass = class_create(THIS_MODULE, "DESD_LED");
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

	irqNumber = gpio_to_irq(gpioButton);
	printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);
	result = request_irq(irqNumber,(irq_handler_t) gpio_irq_handler,IRQF_TRIGGER_RISING,"gpio_handler",NULL);
	printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);
	return 0;

button_device_creation_failed:
	devno = MKDEV(major,0);
	device_destroy(s_pDeviceClass,devno);
device_creation_failed:
	class_destroy(s_pDeviceClass);
class_creation_failed:
	gpio_free(gpioLED);                      // Free the LED GPIO
	gpio_free(gpioButton);                      // Free the LED GPIO
alloc_chrdev_region_failed:
	return -1;

}


static void __exit ebbLED_exit(void){
	int i;
	gpio_set_value(gpioLED, 0);              // Turn the LED off, indicates device was unloaded
	//gpio_unexport(gpioLED);                  // Unexport the Button GPIO
	gpio_free(gpioLED); // Free the LED GPIO
	gpio_free(gpioButton);           
	for(i = 0; i < devcnt; i++)
	{
		devno = MKDEV(major,i);
		device_destroy(s_pDeviceClass, devcnt);
	}
	class_destroy(s_pDeviceClass);
	free_irq(irqNumber, NULL);
	printk(KERN_INFO "EBB LED: Goodbye from the EBB LED LKM!\n");
}

static irqreturn_t gpio_irq_handler(unsigned int irq, void *dev_id){
	printk(KERN_INFO "%s: interrupt received!\n",THIS_MODULE->name);
	ledOn = !ledOn;                          // Invert the LED state on each button press
	gpio_set_value(gpioLED, ledOn);          // Set the physical LED accordingly
	printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(gpioButton));
	return IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

module_init(ebbLED_init);
module_exit(ebbLED_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abc");
MODULE_DESCRIPTION("A simple Linux LED driver LKM for the BBB");
MODULE_VERSION("0.1");
