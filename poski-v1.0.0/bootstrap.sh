#! /bin/bash
##
# NOTE: If you just want to build pOSKI, first try to run INSTALL.sh script
# or ./configure && make commands as described in the installation section 
# of the user's guide.
# This file is only for updating the GNU Build System.
##

rm -rf autom4te.cache

touch NEWS README AUTHORS ChangeLog
autoreconf -fvi

#./INSTALL.sh


