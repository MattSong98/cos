#define TYPE_INT 0
#define TYPE_USD_INT 1
#define TYPE_SHT 2
#define TYPE_USD_SHT 3
#define TYPE_LNG 4
#define TYPE_USD_LNG 5
#define TYPE_HEX 6
#define TYPE_STR 7
#define TYPE_CHAR 8

#define TTY_SIZE 80*25
#define TTY_ROW_SIZE 25
#define TTY_COL_SIZE 80
#define TTY_STD_ATR 0x07

extern void 
_copy_to_cga (unsigned char c, unsigned char atr, unsigned short pos);

static unsigned char buf[TTY_SIZE];
static unsigned char atr[TTY_SIZE];
static unsigned short pos;

static void print_to_tty(unsigned char);
static void scroll_up();
static void flush();

extern void init_tty();
extern void print_hello();
extern void write_tty(void *ptr, int type);
extern void clear();

void init_tty() {
	for (unsigned short i = 0; i < TTY_SIZE; i++) {
		buf[i] = ' ';
		atr[i] = TTY_STD_ATR;
	}
	flush();
}

void print_hello() {
	print_to_tty('H');
	print_to_tty('e');
	print_to_tty('l');
	print_to_tty('l');
	print_to_tty('o');
	print_to_tty('\n');
	flush();
}

void write_tty(void *ptr, int type) {
	if (type == TYPE_HEX) {
		// assume sizeof(TYPE_HEX) == 4
		unsigned char digits[8];
		unsigned long hex = *(unsigned long *)(ptr);
		unsigned char dig;
		for (int i = 0; i < 8; i++) {
			dig = (unsigned char)(hex % 16); 
			if (dig < 10) dig += 48;
			else dig += 55;
			digits[i] = dig;
			hex = hex / 16;
		}
		print_to_tty('0');
		print_to_tty('x');
		for (int i = 7; i >= 0; i--) {
			print_to_tty(digits[i]);
		}
		flush();
	} 
}

void clear() {
	pos = 0;
	for (unsigned short i = 0; i < TTY_SIZE; i++) {
		buf[i] = ' ';
	}
	flush();
}

void flush() {
	for (unsigned short i = 0; i < TTY_SIZE; i++) {
		_copy_to_cga(buf[i], atr[i], i);
	}
}

void print_to_tty(unsigned char c) {
	if (pos == TTY_SIZE) {
		scroll_up();
	}
	buf[pos++] = c;
}

void scroll_up() {
	for (unsigned short i = 0; i < TTY_SIZE - TTY_COL_SIZE; i++) {
		buf[i] = buf[i+TTY_COL_SIZE];
	}
	for (unsigned short i = TTY_SIZE - TTY_COL_SIZE; i < TTY_SIZE; i++) {
		buf[i] = ' ';
	}
	pos = TTY_SIZE - TTY_COL_SIZE;
}

