/**
 * @file mjaron_fakeuname.h
 * @brief The fakeuname library which shadows original uname function.
 *        Sample usage:
 *
 *        gcc -c -Wall -Werror -fpic mjaron-fakeuname-all.c
 *        gcc -shared -o libmjaron-fakeuname.so mjaron-fakeuname-all.o
 *        env MJARON_FAKEUNAME_PRESET=i686 LD_PRELOAD=./libmjaron-fakeuname.so uname -a
 */

#ifndef mjaron_fakeuname_H
#define mjaron_fakeuname_H

#define  _GNU_SOURCE

#include <string.h>
#include <stdio.h>

#include <sys/utsname.h>

#if _UTSNAME_DOMAIN_LENGTH - 0
/* Name of the domain of this node on the network.  */
# ifdef __USE_GNU
// The name of domainname member.
#define MJARON_FAKEUNAME_DOMAINNAME domainname
# else
// The name of domainname member.
#define MJARON_FAKEUNAME_DOMAINNAME __domainname
# endif
#endif

#define MJARON_FAKEUNAME_INLINE static inline

typedef struct mjaron_fakeuname_struct
{
    char* sysname;
    char* nodename;
    char* release;
    char* version;
    char* machine;
    char* domainname;
} mjaron_fakeuname_struct;

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set(char* dst, const char* src, size_t len)
{
    strncpy(dst, src, len - 1); // NOLINT
    dst[len - 1] = 0x00;
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set_sysname(struct utsname *un, const char* what)
{
    mjaron_fakeuname_set(un->sysname, what, _UTSNAME_SYSNAME_LENGTH);
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set_nodename(struct utsname *un, const char* what)
{
    mjaron_fakeuname_set(un->nodename, what, _UTSNAME_NODENAME_LENGTH);
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set_release(struct utsname *un, const char* what)
{
    mjaron_fakeuname_set(un->release, what, _UTSNAME_RELEASE_LENGTH);
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set_version(struct utsname *un, const char* what)
{
    mjaron_fakeuname_set(un->version, what, _UTSNAME_VERSION_LENGTH);
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set_machine(struct utsname *un, const char* what)
{
    mjaron_fakeuname_set(un->machine, what, _UTSNAME_MACHINE_LENGTH);
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set_domainname(struct utsname *un, const char* what)
{
#ifdef MJARON_FAKEUNAME_DOMAINNAME
    mjaron_fakeuname_set(un->MJARON_FAKEUNAME_DOMAINNAME, what, _UTSNAME_DOMAIN_LENGTH);
#endif // !MJARON_FAKEUNAME_DOMAINNAME
}

MJARON_FAKEUNAME_INLINE const char* mjaron_fakeuname_get_domainname(const struct utsname *un)
{
#ifdef MJARON_FAKEUNAME_DOMAINNAME
    return un->MJARON_FAKEUNAME_DOMAINNAME;
#else
    return NULL;
#endif // !MJARON_FAKEUNAME_DOMAINNAME
}

MJARON_FAKEUNAME_INLINE const char* mjaron_fakeuname_get_domainname_or(const struct utsname *un, const char* const orValue)
{
    const char* domainValue = mjaron_fakeuname_get_domainname(un);
    return (domainValue) ? domainValue : orValue;
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_set_struct(struct utsname *un, const mjaron_fakeuname_struct what)
{
    mjaron_fakeuname_set_sysname(un, what.sysname);
    mjaron_fakeuname_set_nodename(un, what.nodename);
    mjaron_fakeuname_set_release(un, what.release);
    mjaron_fakeuname_set_version(un, what.version);
    mjaron_fakeuname_set_machine(un, what.machine);
    mjaron_fakeuname_set_domainname(un, what.domainname);
}

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_print_utsname(const struct utsname* un)
{
    printf("sysname:    %s\n", un->sysname);
    printf("nodename:   %s\n", un->nodename);
    printf("release:    %s\n", un->release);
    printf("version:    %s\n", un->version);
    printf("machine:    %s\n", un->machine);
    printf("domainname: %s\n", mjaron_fakeuname_get_domainname_or(un, "(none)"));
}

#endif // !mjaron_fakeuname_H

