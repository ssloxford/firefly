#!/bin/sh
set -e

## Input file parameters
# pds - input pds
# sat - AQUA/TERRA
# leapsec - leapsecond file path
# utcpole - utcpole file path
# att - attitude file path
# eph - ephemeris file path
# modis_reflective_luts - reflective look-up table file path
# modis_emissive_luts - emissive look-up table file path
# modis_qa_luts - qa look-up table file path

## Output file parameters
# mxd01 - Level 1a data output path
# mxd03 - Geolocation data output path mxf
# mxd021km - Level 1b 1km granule output path
# mxd02hkm - Level 1b 0.5km granule output path
# mxd02qkm - Level 1b 0.25km granule output path


# /root/SPA/modisl1db/algorithm/DRLshellscripts/run_modis-l1a-geo.sh

pds=$1
sat=$2
leapsec=$3
utcpole=$4
att=$5
eph=$6
modis_reflective_luts=$7
modis_emissive_luts=$8
modis_qa_luts=$9
mxd01=${10}
mxd03=${11}
mxd021km=${12}
mxd02hkm=${13}
mxd02qkm=${14}

/root/SPA/modisl1db/algorithm/DRLshellscripts/run_modis-l1a-geo.sh \
    pds "$pds" \
    sat "$sat" \
    mxd01 "$mxd01" \
    mxd03 "$mxd03" \
    leapsec "$leapsec" \
    utcpole "$utcpole" \
    att "$att" \
    eph "$eph"


/root/SPA/modisl1db/wrapper/l1atob/run modis.mxd01 "$mxd01" \
      modis.mxd03 "$mxd03" \
      gbad_eph "$eph" \
      gbad_att "$att" \
      modis_reflective_luts "$modis_reflective_luts" \
      modis_emissive_luts "$modis_emissive_luts" \
      modis_qa_luts "$modis_qa_luts" \
      modis.mxd021km "$mxd021km" \
      modis.mxd02hkm "$mxd02hkm" \
      modis.mxd02qkm "$mxd02qkm"
