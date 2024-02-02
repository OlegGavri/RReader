#!/bin/bash

#
# Create debian package
#

# Copy executable from Release build dir
cp ../../build-RReader-Qt_5_9_5_in_PATH_qt5_temporary-Release/RReader debian/usr/bin/

fakeroot dpkg-deb --build debian/
mv debian.deb rreader_0.0.1-beta_amd64.deb

lintian rreader_0.0.1-beta_amd64.deb

