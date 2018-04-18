#!/bin/sh

LOGGING=$1

WLANS=`cat /proc/net/wireless | perl -e '/(\w*):/ and print "$1\n" while $_=<>'`

for wlan in $WLANS
do
	iwinfo $wlan assoclist \
	| perl -e '$/="\n\n"; /^([^ ]*).*?TX: ([^ ]*)/s and print "$1 ".int($2*1024/2)."\n" while $_=<>' \
	| while read line
	do
		# Format:
		#	<MAC-address> [throughput(kBit/s) = 0] [logging? = 0]
		#
		# Examples:
		#	01:23:45:67:89:AB
		#	CD:EF:11:22:33:44 10000
		#	55:66:77:88:99:AA 10000 1
		#	
		# ---> Only one line each time (max length = 99 symbols) <---
		#
		echo $line $LOGGING > /sys/kernel/debug/hello_kernel_file
	done
done
