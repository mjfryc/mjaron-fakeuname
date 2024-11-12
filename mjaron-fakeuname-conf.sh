#!/bin/bash
# Generates fakeunode configuration file.
#
# E.g:
#
#     ./mjaron-fakeuname-conf.sh > fakeuname.conf;
#     MJARON_FAKEUNAME_CONF=fakeuname.conf uname -a;
#

set -e;

echo "# MJaron FakeNode";
echo "# Configuration of : $(uname -n)";
echo "# Used when MJARON_FAKEUNAME_CONF is set.";
echo "";
echo "sysname: $(uname -o)";
echo "nodename: $(uname -n)";
echo "release: $(uname -r)";
echo "version: $(uname -v)";
echo "machine: $(uname -m)";
#echo "domainname: (none)";
echo "";

