# mjaron-fakeuname
Fake version of Linux uname function.

## Integration

* Download the release source code file:
  ```bash
  curl -L -o mjaron-fakeuname-all.c https://github.com/mjfryc/mjaron-fakeuname/releases/download/v0.0.2/mjaron-fakeuname-all.c
  ```
* Compile the code - make shared library:
  ```bash
  gcc -c -Wall -Werror -fpic mjaron-fakeuname-all.c && gcc -shared -o libmjaron-fakeuname.so mjaron-fakeuname-all.o
  ```
* Configure and override the system library:
  ```bash
  env MJARON_FAKEUNAME_PRESET=i686 LD_PRELOAD=./libmjaron-fakeuname.so uname -a
  ```

## Alternative confuguration ways

The library does the following:

1. Check if `MJARON_FAKEUNAME_PRESET` is set to use predefined values.
   * Currently only `i686` value is defined which simulates 32-bit operating system.
2. Look for the contiguration file at:
   * `MJARON_FAKEUNAME_CONF` environment variable if set.
   * `/etc/mjaron_fakeuname.conf` by default, if exists.
3. Look for indifivual values and override above configuration:
   * `MJARON_FAKEUNAME_MACHINE` - changes the machine value.

