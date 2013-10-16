# pcat

Fast threaded data flow statistics generator for piped data.

- Author: Eivind Magnus Hvidevold
- Credits: (Tor Hveem)[https://github.com/torhve/], formatting suggestions
- License: GPL v2

Similar projects:
- (bar/Command Line Progress Bar)[http://clpbar.sf.net]
- (bfr/buffer)[http://www.glines.org:8000/software/buffer.html]

# Compile

    sudo aptitude install libtool autoconf automake &&
    autoreconf --force --install &&
    ./configure &&
    make

# Examples

    emh@ubuntu ~/Dropbox/homedir-alexis/devel/pcat % cat /dev/urandom | ./pcat > /dev/null
    ^Ccc: 183.8 MiB/s    0:13   Cur:  15.9 MiB/s   Avg:  14.1 MiB/s     

    emh@ubuntu ~/Dropbox/homedir-alexis/devel/pcat % cat /dev/zero | ./pcat > /dev/null 
    ^Ccc:   7.3 GiB/s    0:07   Cur:   1.4 GiB/s   Avg:   1.0 GiB/s     

# Cleanup

    make distclean
    rm Makefile.in aclocal.m4 config.h.in configure depcomp install-sh missing
    rm -rf autom4te.cache
