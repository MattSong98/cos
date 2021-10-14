// not included yet !

#include "types.h"
#include "tty.h"

#define MAX_BUF_SIZE 64

// access by upper layor process through syscall
extern int tty_read(uchar *buf, int count);
extern int tty_write(uchar *buf, int count);

struct buffer_q {
	uchar queue[MAX_BUF_SIZE];
	uchar r_ptr;
	uchar w_ptr;
}

static struct buffer_q read_q;
static struct buffer_q write_q;
static struct buffer_q cooked_read_q;

// for data_structure manipulation
void write_to_read_q(char c);
void copy_to_cooked(char c);
void echo_to_write_q(char c);



