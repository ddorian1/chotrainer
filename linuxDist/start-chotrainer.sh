#!/usr/bin/env sh

cd $(dirname $(readlink -f $0))
./chotrainer/start.sh chotrainer
