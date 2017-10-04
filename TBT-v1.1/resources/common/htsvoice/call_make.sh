# !/bin/bash

find labels/full -maxdepth 1 -type f |head -10|xargs cp -t labels/gen
