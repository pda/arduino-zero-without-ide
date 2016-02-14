#!/bin/bash

# Download Atmel Software Framework (ASF), extract files needed for SAMD21,
# including the bundled ARM CMSIS.

set -eou pipefail

ZIPFILE="asf-standalone-archive-3.30.0.43.zip"
ZIPSHA256="951453ea953d38bc73474746d43772822654b3da4f1600d0018fb93a2a51c303"
TMPDIR="extract-asf-tmp"

main() {
  acquire
  extract
  rearrange
}

acquire() {
  if [[ ! -e "$ZIPFILE" ]]; then
    wget "http://www.atmel.com/images/$ZIPFILE"
  fi
  sha256sum -c <(echo $ZIPSHA256 $ZIPFILE)
}

extract() {
  rm -rf -- $TMPDIR
  unzip $ZIPFILE \
    "xdk-asf-3.30.0/readme.txt" \
    "xdk-asf-3.30.0/sam0/utils/cmsis/samd21/include/pio/samd21g18a.h" \
    "xdk-asf-3.30.0/sam0/utils/cmsis/samd21/include/samd21g18a.h" \
    "xdk-asf-3.30.0/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c" \
    "xdk-asf-3.30.0/sam0/utils/linker_scripts/samd21/gcc/samd21g18a_flash.ld" \
    "xdk-asf-3.30.0/sam0/utils/linker_scripts/samd21/gcc/samd21g18a_sram.ld" \
    "xdk-asf-3.30.0/thirdparty/CMSIS/Include/*" \
    "xdk-asf-3.30.0/thirdparty/CMSIS/Lib/*" \
    "xdk-asf-3.30.0/thirdparty/CMSIS/README.txt" \
    -d $TMPDIR
}

rearrange() {
  rm -rf -- asf cmsis
  mv $TMPDIR/xdk-asf-3.30.0/thirdparty/CMSIS cmsis
  rmdir $TMPDIR/xdk-asf-3.30.0/thirdparty
  mv $TMPDIR/xdk-asf-3.30.0 asf
  rmdir $TMPDIR
}

main
