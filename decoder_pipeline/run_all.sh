#!/bin/sh
set -e

# Runs level 0 (PDSes of raw CCSDS packets) MODIS data through the pipeline
# to produce images tagged with forest fires

# Usage: run_all.sh DATA_PATH
# PATH_TO_INPUT: Path to a directory containing all of the following input files:
#   Defaults to "./data
#   Input PDS (of expected naming convention MYD00F.xxx.001.PDS / MOD00F.xxx.001.PDS)
#   leapsec.xxx.dat - the leapsecond adjustment file
#   utcpole.xxx.dat - the utcpole file
#

if [ "$#" -eq 0 ]; then
    echo "Must supply DATA_PATH"
    exit 1
fi

DATA_PATH="$1"

PDSs=$(ls "$DATA_PATH"/input | grep -e "MYD00" -e "MOD00")
leapsec=$(ls "$DATA_PATH"/input | grep "leapsec")
utcpole=$(ls "$DATA_PATH"/input | grep "utcpole")
modis_reflective_luts="/root/SPA/modisl1db/algorithm/run/var/modisa/cal/EVAL/MYD02_Reflective_LUTs.V6.1.15.1.hdf"
modis_emissive_luts="/root/SPA/modisl1db/algorithm/run/var/modisa/cal/EVAL/MYD02_Emissive_LUTs.V6.1.15.1.hdf"
modis_qa_luts="/root/SPA/modisl1db/algorithm/run/var/modisa/cal/EVAL/MYD02_QA_LUTs.V6.1.15.1.hdf"

PDS_suffixes=$(echo "$PDSs" | cut -d '.' --complement -f 1 | rev | cut -d '.' --complement -f 1 | rev)


if [ "$(echo $PDSs | grep MYD)" == "" ]; then
    sat="TERRA"
    prefix="MOD"
else
    sat="AQUA"
    prefix="MYD"
fi

mkdir -p "$DATA_PATH/output/images"
mkdir -p "$DATA_PATH/tmp"

# TODO: add optional att/eph files

# TODO: take input arguments as we'd expect

# Create level 1A, 1B, geolocated, and fire information products
# NB for loops don't work on quoted strings, so we don't quote $PDS
i=0
for pds in $PDSs; do
    echo "Processing new PDS: $pds"

    # Construct file name variables
    suffix=$(echo "$pds" | cut -d '.' --complement -f 1 | rev | cut -d '.' --complement -f 1 | rev)
    mxd01="/root/data/output/${prefix}01.$suffix.hdf"
    mxd03="/root/data/output/${prefix}03.$suffix.hdf"
    mxd021km="/root/data/output/${prefix}021KM.$suffix.hdf"
    mxd02hkm="/root/data/output/${prefix}02HKM.$suffix.hdf"
    mxd02qkm="/root/data/output/${prefix}02QKM.$suffix.hdf"
 
    # TODO: check variables
    # TODO: replace with "run" command so that it's closer to IPOPP
    podman run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
        '["/root/SPA/modisl1db/algorithm/DRLshellscripts/run_modis-l1a-geo.sh",
            "pds", "/root/data/input/'"$pds"'",
            "sat", "'"$sat"'",
            "mxd01", "'"$mxd01"'",
            "mxd03", "'"$mxd03"'",
            "leapsec", "/root/data/input/'"$leapsec"'",
            "utcpole", "/root/data/input/'"$utcpole"'"]' \
        modisl1db

    podman run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
        '["/root/SPA/modisl1db/wrapper/l1atob/run",
            "modis.mxd01", "'"$mxd01"'",
            "modis.mxd03", "'"$mxd03"'",
            "modis_reflective_luts", "'"$modis_reflective_luts"'",
            "modis_emissive_luts", "'"$modis_emissive_luts"'",
            "modis_qa_luts", "'"$modis_qa_luts"'",
            "modis.mxd021km", "'"$mxd021km"'",
            "modis.mxd02hkm", "'"$mxd02hkm"'",
            "modis.mxd02qkm", "'"$mxd02qkm"'"]' \
        modisl1db

    podman run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
        '["/root/SPA/mod14/wrapper/mod14/run",
            "modis.mxd021km", "'"$mxd021km"'",
            "modis.mxd03", "'"$mxd03"'",
            "modis.firedetection", "/root/data/output/'"$prefix"'14.'"$suffix"'.hdf",
            "modis.fireloc.txt", "/root/data/output/'"$prefix"'14.'"$suffix"'.txt"]' \
        mod14

    i=$((++i))
done


echo "Running bluemarble"

# TODO: add other parameters to enable mosiacing
podman run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
    '["/root/SPA/BlueMarble/algorithm/DRL_scripts/modis_truecolor.sh",
        "modisl1b1km", "/root/data/output/'"$prefix"'021KM*",
        "outdir", "/root/data/output/images/"]' \
    bluemarble

for l1b1 in "$DATA_PATH"/output/"${prefix}"021KM*; do
    number="s$(echo \"$l1b1\" | cut -d '.' -f2,3 | sed 's/\.//' | tail -c +4)"
    ingeotiff="$(ls $DATA_PATH/output/images | grep $number)"
    outgeotiff="$(echo $ingeotiff | sed 's/\.tif/_fire\.tif/')"
    suffix=$(echo "$l1b1" | cut -d '.' --complement -f 1 | rev | cut -d '.' --complement -f 1 | rev)

    # TODO: redo echos
    echo "/root/SPA/BlueMarble/algorithm/DRL_scripts/modis_truecolor.sh/overlay_fires.sh"
    echo "fireloc \"/root/data/output/${prefix}14.$suffix.txt\""
    echo "ingeotiff \"$ingeotiff\""
    echo "outgeotiff \"$outgeotiff\""
    echo "markersize large"

    podman run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
        '["/root/SPA/BlueMarble/algorithm/DRL_scripts/overlay_fires.sh",
            "fireloc", "/root/data/output/'"${prefix}"'14.'"$suffix"'.txt",
            "ingeotiff", "/root/data/output/images/'"$ingeotiff"'",
            "outgeotiff", "/root/data/output/images/'"$outgeotiff"'",
            "markersize", "large"]' \
        bluemarble
done
