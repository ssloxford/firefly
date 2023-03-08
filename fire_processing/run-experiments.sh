#!/bin/bash

cat args | parallel --ungroup --colsep ' ' './run-experiment.sh {}'
