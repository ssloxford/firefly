#!/bin/bash

do_process () {
    cat overshadow_processed/$1 | awk '{$1=$1};1' | sed -e 's/ /,/g' | while read line; do echo "$1,$line"; done
}
export -f do_process

echo "filename,count,bit" > overshadow_results.csv
ls overshadow_processed | parallel 'do_process {}' >> overshadow_results.csv
