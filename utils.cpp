#include "utils.h"

#include <stdarg.h>
#include <stdio.h>

std::string stringf(std::string format, ...) {

    va_list ap;
    va_start(ap, format);
    int buf_length = vsnprintf(NULL, 0, format.data(), ap);
    va_end(ap);

    char * buf = new char[buf_length + 1];
    va_start(ap, format);
    vsnprintf(buf, buf_length + 1, format.data(), ap);
    va_end(ap);
    std::string result(buf, buf_length);
    delete [] buf;
    return result;
}
