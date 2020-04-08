#ifndef _DEBUG_STUFF_H
#define _DEBUG_STUFF_H

#include "usart_driver.h"

void
assert_printf(const char *const file, int line)
{
    static char buff[128];
    buff[0] = 'A';
    buff[1] = 's';
    buff[2] = 's';
    buff[3] = 'e';
    buff[4] = 'r';
    buff[5] = 't';
    buff[6] = ' ';
    buff[7] = 'f';
    buff[8] = 'a';
    buff[9] = 'i';
    buff[10] = 'l';
    buff[11] = 'e';
    buff[12] = 'd';
    buff[13] = ' ';

    char *c = (char *)file;
    int i = 14;
    while (*c) {
        buff[i] = *c;
        i++;
        c++;
    }
    buff[i++] = ',';
    buff[i++] = ' ';
    buff[i++] = 'l';
    buff[i++] = 'i';
    buff[i++] = 'n';
    buff[i++] = 'e';
    buff[i++] = ' ';

    char line_text[7];
    int j = 0;
    while (line > 0) {
        line_text[j] = (line % 10) + '0';
        j++;
        line /= 10;
    }

    while (j > 0) {
        buff[i] = line_text[j - 1];
        i++;
        j--;
    }
    buff[i++] = '\n';
    usart_send_string(USART3, buff, i);
}

#ifndef NDEBUG
#define assert(_condition)                                             \
    do {                                                               \
        if (! (_condition)) {                                          \
            assert_printf(__FILE__, __LINE__); \
        }                                                              \
    } while (0)
#else
#define assert(_condition)
#endif

#endif /* _DEBUG_STUFF_H */

