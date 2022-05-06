#!/bin/sh
set -e

install -D -m 755 cadu_utils/bin/caduinfo ~/.local/bin/
install -D -m 755 cadu_utils/bin/cadupack ~/.local/bin/
install -D -m 755 cadu_utils/bin/caduunpack ~/.local/bin/
install -D -m 755 cadu_utils/bin/cadurandomise ~/.local/bin/
install -D -m 755 cadu_utils/bin/caduhead ~/.local/bin/
install -D -m 755 cadu_utils/bin/cadutail ~/.local/bin/
install -D -m 755 ccsds_utils/bin/ccsdsinfo ~/.local/bin/
install -D -m 755 ccsds_utils/bin/ccsdspack ~/.local/bin/
install -D -m 755 ccsds_utils/bin/ccsdsunpack ~/.local/bin/
install -D -m 755 modis_utils/bin/modismaskfires ~/.local/bin/
