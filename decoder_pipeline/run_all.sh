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

if ! type podman > /dev/null; then
    CONTAINER_RUNTIME=docker
else
	CONTAINER_RUNTIME=podman
fi

if [ "$#" -eq 0 ]; then
    echo "Must supply DATA_PATH"
    exit 1
fi

DATA_PATH="$(realpath $1)"
echo "DATA_PATH: $DATA_PATH"
echo "CONTAINER_RUNTIME: $CONTAINER_RUNTIME"

if [ -d "$DATA_PATH"/output ]; then
	echo "Must move $DATA_PATH/output before running the script"
fi

set +e
PDSs=$(ls "$DATA_PATH"/input | grep -e "MYD00" -e "MOD00")
if [ -z "$PDSs" ]; then
	echo "No MYD00... or MOD00... files in $DATA_PATH/input"
	exit 1
fi

leapsec=$(ls "$DATA_PATH"/input | grep "leapsec")
if [ -z "$leapsec" ]; then
	echo "No leapsec file in $DATA_PATH/input"
	exit 1
fi

utcpole=$(ls "$DATA_PATH"/input | grep "utcpole")
if [ -z "$utcpole" ]; then
	echo "No utcpole file in $DATA_PATH/input"
	exit 1
fi
set -e

modis_reflective_luts="/root/SPA/modisl1db/algorithm/run/var/modisa/cal/EVAL/MYD02_Reflective_LUTs.V6.1.15.1.hdf"
modis_emissive_luts="/root/SPA/modisl1db/algorithm/run/var/modisa/cal/EVAL/MYD02_Emissive_LUTs.V6.1.15.1.hdf"
modis_qa_luts="/root/SPA/modisl1db/algorithm/run/var/modisa/cal/EVAL/MYD02_QA_LUTs.V6.1.15.1.hdf"

PDS_suffixes=$(echo "$PDSs" | cut -d '.' --complement -f 1 | rev | cut -d '.' --complement -f 1 | rev)

if [ -z "$(echo $PDSs | grep MYD)" ]; then
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
	echo ""
    echo "### Processing new PDS: $pds"

    # Construct file name variables
    suffix=$(echo "$pds" | cut -d '.' --complement -f 1 | rev | cut -d '.' --complement -f 1 | rev)
    mxd01="/root/data/output/${prefix}01.$suffix.hdf"
    mxd03="/root/data/output/${prefix}03.$suffix.hdf"
    mxd021km="/root/data/output/${prefix}021KM.$suffix.hdf"
    mxd02hkm="/root/data/output/${prefix}02HKM.$suffix.hdf"
    mxd02qkm="/root/data/output/${prefix}02QKM.$suffix.hdf"
 
    # TODO: check variables
    # TODO: replace with "run" command so that it's closer to IPOPP
	echo ""
    echo "### Running modisl1db l1a-geo initial processing"

	$CONTAINER_RUNTIME run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
    	/root/SPA/modisl1db/algorithm/DRLshellscripts/run_modis-l1a-geo.sh \
		modisl1db \
			pds "/root/data/input/$pds" \
			sat "$sat" \
			mxd01 "$mxd01" \
			mxd03 "$mxd03" \
			leapsec "/root/data/input/$leapsec" \
			utcpole "/root/data/input/$utcpole"

	echo ""
    echo "### Running modisl1db l1atob geolocation"
 
    $CONTAINER_RUNTIME run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
        /root/SPA/modisl1db/wrapper/l1atob/run \
		modisl1db \
            modis.mxd01 "$mxd01" \
            modis.mxd03 "$mxd03" \
            modis_reflective_luts "$modis_reflective_luts" \
            modis_emissive_luts "$modis_emissive_luts" \
            modis_qa_luts "$modis_qa_luts" \
            modis.mxd021km "$mxd021km" \
            modis.mxd02hkm "$mxd02hkm" \
            modis.mxd02qkm "$mxd02qkm"

	echo ""
    echo "### Running mod14 fire detection"

    $CONTAINER_RUNTIME run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
        /root/SPA/mod14/wrapper/mod14/run \
		mod14 \
            modis.mxd021km "$mxd021km" \
            modis.mxd03 "$mxd03" \
            modis.firedetection "/root/data/output/${prefix}14.${suffix}.hdf" \
            modis.fireloc.txt "/root/data/output/${prefix}14.${suffix}.txt"

    i=$((++i))
done

echo ""
echo "Running bluemarble image generation"

# TODO: add other parameters to enable mosiacing
$CONTAINER_RUNTIME run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
    /root/SPA/BlueMarble/algorithm/DRL_scripts/modis_truecolor.sh \
    bluemarble \
        modisl1b1km /root/data/output/"$prefix""021KM*" \
        outdir "/root/data/output/images/"

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

    echo ""
    echo "Running bluemarble fire overlay"

    $CONTAINER_RUNTIME run -v "$DATA_PATH":/root/data --rm -it --entrypoint \
        /root/SPA/BlueMarble/algorithm/DRL_scripts/overlay_fires.sh \
        bluemarble \
            fireloc "/root/data/output/${prefix}14.$suffix.txt" \
            ingeotiff "/root/data/output/images/$ingeotiff" \
            outgeotiff "/root/data/output/images/$outgeotiff" \
            markersize "large"
done
