#!/usr/bin/env sh

prog=$1
if [[ ! ("$prog" = "chotrainer" || "$prog" = "chotrainerCreator") ]]
then
	echo "Please use ...."
	exit 1
fi

cd $(dirname $(readlink -f $0))

SYSARCH=$(getconf LONG_BIT)
ARCH=$(expr "$(file chotrainer)" : '.*ELF \([[:digit:]]*\)')
if [[ ! ARCH -eq SYSARCH ]]
then
	echo "You are trying to use the $ARCH bit version of Chotrainer on a $SYSARCH bit system."
	echo "Please get the $SYSARCH version and try again."
	exit 1
fi

libMissing="false"

/sbin/ldconfig -p | grep -q libQtGui.so
if [[ $? -eq 1 ]]
then
	echo "You are missing libQt5Gui. Please use your package manager to install it and try again."
	libMissing="true"
fi

/sbin/ldconfig -p | grep -q libfluidsynth.so
if [[ $? -eq 1 ]]
then
	echo "You are missing libfluidsynth. Please use your package manager to install it and try again."
	libMissing="true"
fi

if [[ "$libMissing" = "true" ]]
then
	exit 1
fi

export LD_LIBRARY_PATH=$(readlink -f ./)
nohup "./$prog" > /dev/null 2>&1 &
exit 0

if [[ $prog = "chotrainer" ]]
then
	nohup ./chotrainer & > /dev/null
elif [[ $prog = "chotrainerCreator" ]]
then
	nohup ./chotrainerCreator & > /dev/null
fi
