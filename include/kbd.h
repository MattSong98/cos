#ifndef _KBD_H
#define _KBD_H

// PC keyboard interface constants
#define NO              0
#define SHIFT           (1<<0)
#define CTL             (1<<1)

// C('A') == Control-A
#define C(x) (x - '@')

#endif
