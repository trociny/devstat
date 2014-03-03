#!/bin/sh
#
# $Id$
#

#file with statistics

DISKSTATS=/proc/diskstats

#
# From Documentation/iostats.txt:
#
# Field  1 -- # of reads completed
#     This is the total number of reads completed successfully.
# Field  2 -- # of reads merged, field 6 -- # of writes merged
#     Reads and writes which are adjacent to each other may be merged for
#     efficiency.  Thus two 4K reads may become one 8K read before it is
#     ultimately handed to the disk, and so it will be counted (and queued)
#     as only one I/O.  This field lets you know how often this was done.
# Field  3 -- # of sectors read
#     This is the total number of sectors read successfully.
# Field  4 -- # of milliseconds spent reading
#     This is the total number of milliseconds spent by all reads (as
#     measured from __make_request() to end_that_request_last()).
# Field  5 -- # of writes completed
#     This is the total number of writes completed successfully.
# Field  7 -- # of sectors written
#     This is the total number of sectors written successfully.
# Field  8 -- # of milliseconds spent writing
#     This is the total number of milliseconds spent by all writes (as
#     measured from __make_request() to end_that_request_last()).
# Field  9 -- # of I/Os currently in progress
#     The only field that should go to zero. Incremented as requests are
#     given to appropriate struct request_queue and decremented as they finish.
# Field 10 -- # of milliseconds spent doing I/Os
#     This field is increases so long as field 9 is nonzero.
# Field 11 -- weighted # of milliseconds spent doing I/Os
#     This field is incremented at each I/O start, I/O completion, I/O
#     merge, or read of these stats by the number of I/Os in progress
#     (field 9) times the number of milliseconds spent doing I/O since the
#     last update of this field.  This can provide an easy measure of both
#     I/O completion time and the backlog that may be accumulating.

PROGNAME=`basename $0`
FILTER=cat
DEVICE=
MASHINE=1

#
# Functions
#

usage()
{
    echo
    echo "usage: $PROGNAME [options] [device]"
    echo
    echo "Options:"
    echo
    echo "  -h           print this help and exit"
    echo "  -f <file>    path to diskstats file (default is $DISKSTATS)"
    echo "  -m           machine-friendly (more convenient for parsing) output"
    echo "  -p <filter>  filter diskstats file using this programm (e.g. to unzip)"
    echo
}

#
# Main
#

while getopts "hf:mp:" opt; do

    case "$opt" in

	h)
	    usage
	    exit 0
	    ;;
	f)
	    DISKSTATS=$OPTARG
	    ;;
	m)
	    MACHINE=1
	    ;;
	p)
	    FILTER=$OPTARG
	    ;;
	\?)
	    usage >&2
	    exit 1
	    ;;
    esac
done

shift $((OPTIND - 1))

if [ -n "$1" ]; then
    DEVICE="$1"
fi

if ! [ -f "$DISKSTATS" -a -r "$DISKSTATS" ]; then
    echo "Can't access diskstats file '$DISKSTATS'" >&2
    exit 1
fi

eval "$FILTER" "$DISKSTATS" |
awk -v m=$MACHINE -v d="$DEVICE" \
    'd == "" || $3 == d {
         if (m) {
             header = "";
             prefix = $3 " (" $1 "/" $2 "):";
             footer = "";
         } else {
             header = $3 " (" $1 "/" $2 "):\n";
             prefix = "";
             footer = "\n";
         }
         printf "%s", header;
         printf "%s\t%d reads completed\n", prefix, $4;
         printf "%s\t%d reads merged\n", prefix, $5;
         printf "%s\t%d sectors read\n", prefix, $6;
         printf "%s\t%d milliseconds spent reading\n", prefix, $7;
         printf "%s\t%d writes completed\n", prefix, $8;
         printf "%s\t%d writes merged\n", prefix, $9;
         printf "%s\t%d sectors written\n", prefix, $10;
         printf "%s\t%d milliseconds spent writing\n", prefix, $11;
         printf "%s\t%d I/Os currently in progress\n", prefix, $12;
         printf "%s\t%d milliseconds spent doing I/Os\n", prefix, $13;
         printf "%s\t%d milliseconds spent doing I/Os (weighted)\n", prefix, $14;
         printf "%s", footer;
    }'
