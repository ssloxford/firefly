#!/bin/bash

ls overshadow_output | parallel 'od -vtu1 -An -w1 overshadow_output/{} | sort -n | uniq -c > overshadow_processed/{}'
