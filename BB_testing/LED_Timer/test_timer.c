#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/delay.h>      // Using this header for the msleep() function
#include<linux/timer.h>

static struct timer_list myTimer;
static unsigned int gpioLED = 49;           ///< Default GPIO for the LED is 49

module_param(gpioLED, uint, S_IRUGO);       ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(gpioLED, " GPIO LED number (default=49)");     ///< parameter description

static unsigned int blinkPeriod = 1000;     ///< The blink period in ms
module_param(blinkPeriod, uint, S_IRUGO);   ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(blinkPeriod, " LED blink period in ms (min=1, default=1000, max=10000)");

static char ledName[7] = "ledXXX";          ///< Null terminated default string -- just in case
static bool ledOn = 0; 

void MyTimerCallback(unsigned long data)
{
	printk(KERN_INFO "EBB LED: next 5 seconds!\n");
	ledOn = !ledOn;
	gpio_direction_output(gpioLED, ledOn);
	mod_timer(&myTimer,jiffies+5*HZ);
}

void timer_config(void)
{  
        init_timer(&myTimer);
	myTimer.data = 0;
	myTimer.function = MyTimerCallback;
	myTimer.expires = jiffies+5*HZ;
	add_timer(&myTimer); 
}

static int __init ebbLED_init(void){
	
	printk(KERN_INFO "EBB LED: Hello from the EBB LED LKM!\n");
	ledOn = true;
	gpio_request(gpioLED, "sysfs");          // gpioLED is 49 by default, request it
	gpio_direction_output(gpioLED, ledOn);   // Set the gpio to be in output mode and turn on
	//gpio_export(gpioLED, false);  // causes gpio49 to appear in /sys/class/gpio
	// the second argument prevents the direction from being changed
	timer_config();
	return 0;

}


static void __exit ebbLED_exit(void){
	gpio_set_value(gpioLED, 0);              // Turn the LED off, indicates device was unloaded
	//gpio_unexport(gpioLED);                  // Unexport the Button GPIO
	gpio_free(gpioLED);                      // Free the LED GPIO
	del_timer_sync(&myTimer);
	printk(KERN_INFO "EBB LED: Goodbye from the EBB LED LKM!\n");
}

module_init(ebbLED_init);
module_exit(ebbLED_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abc");
MODULE_DESCRIPTION("A simple Linux LED driver LKM for the BBB");
MODULE_VERSION("0.1");
