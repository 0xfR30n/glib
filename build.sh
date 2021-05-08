#!/bin/bash

set -e

source /opt/devkitpro/3dsvars.sh 

export PKG_CONFIG_PATH=/opt/devkitpro/portlibs/3ds/lib/pkgconfig/
export BUILDDIR=/tmp/3ds/__build__
export INSTALLDIR=$PORTLIBS_PREFIX

mkdir -p $BUILDDIR
rm -fr $BUILDDIR

meson --buildtype=plain \
    --cross-file=crossfile-3ds.txt \
    --default-library=static \
    --prefix=$INSTALLDIR \
    --libdir=lib \
    -Dtests=false \
    -Dinstalled_tests=false \
    -Dglib_assert=false \
    -Dglib_checks=false \
    -Dgtk_doc=false \
    -Dselinux=disabled \
    -Dxattr=false \
    -Dglib_debug=disabled \
    -Diconv=auto \
    -Ddtrace=false \
    $BUILDDIR

ninja -C $BUILDDIR
meson install -C $BUILDDIR
