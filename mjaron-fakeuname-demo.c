#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "mjaron-fakeuname.h"

int main()
{
    struct utsname un;
    const int ret = uname(&un);
    mjaron_fakeuname_print_utsname(&un);
    return ret;
}
