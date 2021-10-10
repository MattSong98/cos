// here are some useful debugging tools designed for 
// kernel programming.

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
#define POS(row, col) row*TTY_COL_SIZE+col

#define STD_ATR 0b00001011

extern void 
_copy_to_cga (unsigned char c, unsigned char atr, unsigned short pos);

// static unsigned short row = 0;
// static unsigned short col = 0;

static char buf[TTY_SIZE];
static char atr[TTY_SIZE];

// static void print_to_tty(char);
// static void scroll_up();
static void flush();

extern void init_tty();
// extern void print(void *ptr, int type);
// extern void println(void *ptr, int type);
// extern void printtb(void *ptr, int type);
// extern void clear();

void init_tty() {
	for (unsigned short i = 0; i < TTY_SIZE; i++) {
		buf[i] = ' ';
		atr[i] = STD_ATR;
	}
	flush();
}

void flush() {
	for (unsigned short i = 0; i < TTY_SIZE; i++) {
		_copy_to_cga(buf[i], atr[i], i<<2);
	}
}


