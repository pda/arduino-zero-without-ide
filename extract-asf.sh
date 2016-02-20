#!/bin/bash

# Download Atmel Software Framework (ASF), extract files needed for SAMD21,
# including the bundled ARM CMSIS.

set -eou pipefail

ZIPFILE="asf-standalone-archive-3.30.0.43.zip"
ZIPSHA256="951453ea953d38bc73474746d43772822654b3da4f1600d0018fb93a2a51c303"

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
  unzip -q $ZIPFILE \
    -x xdk-asf-3.30.0/{avr32,mega,xmega}/\*
}

rearrange() {
  mv --no-target-directory --no-clobber xdk-asf-3.30.0 asf
}

main
