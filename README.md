# mjaron-fakeuname
Fake version of Linux [`uname()`](https://www.man7.org/linux/man-pages/man2/uname.2.html) function. Allows replacing the result of Linux [`uname -a`](https://www.man7.org/linux/man-pages/man1/uname.1.html) and Python [os.uname](https://docs.python.org/3/library/os.html#os.uname) commands. Impacts the yum package manager to select packages architecture.

> [!NOTE]
> Use linux [setarch](https://www.man7.org/linux/man-pages/man8/setarch.8.html) command instead of such custom tools, e.g: `setarch i686`.

## Integration

All steps in one:

```bash
curl -L -o mjaron-fakeuname-all.c https://github.com/mjfryc/mjaron-fakeuname/releases/download/v0.0.5/mjaron-fakeuname-all.c &&
  gcc -c -Wall -Werror -fpic mjaron-fakeuname-all.c && gcc -shared -o libmjaron-fakeuname.so mjaron-fakeuname-all.o  &&
  export LD_PRELOAD=$(pwd)/libmjaron-fakeuname.so
```

Detailed steps:

* Download the release source code file:
  ```bash
  curl -L -o mjaron-fakeuname-all.c https://github.com/mjfryc/mjaron-fakeuname/releases/download/v0.0.5/mjaron-fakeuname-all.c
  ```
* Compile the code - make shared library:
  ```bash
  gcc -c -Wall -Werror -fpic mjaron-fakeuname-all.c && gcc -shared -o libmjaron-fakeuname.so mjaron-fakeuname-all.o
  ```
* Configure and override the system library:
  ```bash
  export LD_PRELOAD=$(pwd)/libmjaron-fakeuname.so
  ```

## Usage examples

```bash
env MJARON_FAKEUNAME_PRESET=i686 uname -a
```
or:
```bash
env MJARON_FAKEUNAME_MACHINE=i686 uname -a
```
or:
```bash
export MJARON_FAKEUNAME_CONF=/etc/mjaron-fakenode.conf
/etc/mjaron-fakenode.conf # ...Edit the conf...
uname -a
```

## Alternative confuguration ways

The library does the following:

1. Check if `MJARON_FAKEUNAME_PRESET` is set to use predefined values.
   * Currently only `i686` value is defined which simulates 32-bit operating system.
2. Look for the contiguration file at:
   * `MJARON_FAKEUNAME_CONF` environment variable if set.
   * `/etc/mjaron-fakeuname.conf` by default, if exists.
3. Look for indifivual values and override above configuration:
   * `MJARON_FAKEUNAME_MACHINE` - changes the machine value.

## The configuration file

The default location of configration file is `/etc/mjaron-fakeuname.conf` and may be changed with `export MJARON_FAKEUNAME_CONF=...`.

Current computer configuration may be generated with `mjaron-fakeuname-conf.sh`, e.g:

```bash
./mjaron-fakeuname-conf.sh 

sysname: GNU/Linux
nodename: hostname
release: 6.8.0-48-generic
version: #48-Ubuntu SMP PREEMPT_DYNAMIC Fri Sep 27 14:04:52 UTC 2024
machine: x86_64
```
