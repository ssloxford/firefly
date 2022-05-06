#!/bin/bash

data_dir=/mnt/data/firefly/data
temp_dir_prefix=/mnt/data/firefly/data_temp
results_dir=/mnt/data/firefly/data_results
pds_original=$data_dir/input/original/MYD00F.A2015299.2110.20152992235.001.PDS
#decoder_pipeline=/mnt/data/firefly/repo/decoder_pipeline
decoder_pipeline=/home/josh/git/firefly/decoder_pipeline

name=$1
temp_dir=$temp_dir_prefix/data_$name
shift

echo Running experiment out of $temp_dir with arguments $*...

mkdir $temp_dir
mkdir $temp_dir/input
cp $data_dir/input/leapsec.2022012900.dat $temp_dir/input/leapsec.2022012900.dat
cp $data_dir/input/utcpole.2022012900.dat $temp_dir/input/utcpole.2022012900.dat

echo "Masking fires."
args2=$*
echo $args2
cat $pds_original | modismaskfires $@ > $temp_dir/input/MYD00F.A2015299.2110.20152992235.001.PDS

echo "Finished masking fires, running decoder pipeline."
$decoder_pipeline/run_all.sh $temp_dir

echo "Finished! Copying results."
mv $temp_dir/output/images $results_dir/images_$name
#rm -rf $temp_dir
