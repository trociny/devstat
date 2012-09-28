/*
 * Copyright (c) 2008, 2012 Mikolaj Golub
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 *
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/resource.h>

#include <err.h>
#include <ctype.h>
#include <fcntl.h>
#include <kvm.h>
#include <nlist.h>
#include <unistd.h>
#include <paths.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <devstat.h>
#include <limits.h>

#define MAXNAMELEN	256

/* This is 1/2^64 */
#define BINTIME_SCALE	5.42101086242752217003726400434970855712890625e-20
#define BT2LD(time)	((long double)(time).sec + (time).frac * BINTIME_SCALE)

static void
usage (char* progname) {
	fprintf(stderr, "usage: %s [-M core] [-N system] [-m] [devname]\n", progname);
}

int
main (int argc, char* argv[]) {
	struct statinfo	stats;
	struct devinfo	dinfo;
	int		c, i, found, mfriendly = 0;
	char		*memf = NULL, *nlistf = NULL, *check_dev = NULL;
	kvm_t		*kd = NULL;
	char		errbuf[_POSIX2_LINE_MAX];

        while ((c = getopt(argc, argv, "M:N:m")) != -1) {
		switch(c) {
		case 'N':
			nlistf = optarg;
			break;
		case 'M':
			memf = optarg;
			break;
		case 'm':
			mfriendly = 1;
			break;
		default:
			usage(argv[0]);
			exit(1);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 0)
		check_dev = argv[0];

	if (nlistf != NULL || memf != NULL) {
		kd = kvm_openfiles(nlistf, memf, NULL, O_RDONLY, errbuf);

		if (kd == NULL)
			errx(1, "kvm_openfiles: %s", errbuf);

	}

	if (devstat_checkversion(kd) == -1)
		errx(1, "userland and kernel devstat version mismatch");

	memset(&stats, 0, sizeof(stats));
	memset(&dinfo, 0, sizeof(dinfo));
	stats.dinfo = &dinfo;

	if (devstat_getdevs(kd, &stats) == -1)
		errx(1, "devstat_getdevs failed");

	for(found = 0, i = 0; i < (stats.dinfo)->numdevs; i++) {
		char	dev_name[MAXNAMELEN];
		struct devstat	dev = (stats.dinfo)->devices[i];
		snprintf(dev_name, MAXNAMELEN-1, "%s%d",
			 dev.device_name, dev.unit_number);
		if ((check_dev != NULL) && (strcmp(check_dev, dev_name) != 0))
			continue;

#define CNT	(unsigned long long)
#define PREF	do {if (mfriendly) printf("%s:", dev_name);} while (0);

		if (!mfriendly)
			printf("%s:\n", dev_name);
		PREF printf("\t%llu bytes read\n",
		    CNT dev.bytes[DEVSTAT_READ]);
		PREF printf("\t%llu bytes written\n",
		    CNT dev.bytes[DEVSTAT_WRITE]);
		PREF printf("\t%llu bytes freed\n",
		    CNT dev.bytes[DEVSTAT_FREE]);
		PREF printf("\t%llu reads\n",
		    CNT dev.operations[DEVSTAT_READ]);
		PREF printf("\t%llu writes\n",
		    CNT dev.operations[DEVSTAT_WRITE]);
		PREF printf("\t%llu frees\n",
		    CNT dev.operations[DEVSTAT_FREE]);
		PREF printf("\t%llu other\n",
		    CNT dev.operations[DEVSTAT_NO_DATA]);
		PREF printf("\tduration:\n");
		PREF printf("\t\t%.3Lf sec reads\n",
		    BT2LD(dev.duration[DEVSTAT_READ]));
		PREF printf("\t\t%.3Lf sec writes\n",
		    BT2LD(dev.duration[DEVSTAT_WRITE]));
		PREF printf("\t\t%.3Lf sec frees\n",
		    BT2LD(dev.duration[DEVSTAT_FREE]));
		PREF printf("\t%.3Lf sec busy time\n", BT2LD(dev.busy_time));
		PREF printf("\t%.3Lf sec creation time\n",
		    BT2LD(dev.creation_time));
		PREF printf("\t%llu block size\n", CNT dev.block_size);
		PREF printf("\ttags sent:\n");
		PREF printf("\t\t%llu simple\n",
		    CNT dev.tag_types[DEVSTAT_TAG_SIMPLE]);
		PREF printf("\t\t%llu ordered\n",
		    CNT dev.tag_types[DEVSTAT_TAG_ORDERED]);
		PREF printf("\t\t%llu head of queue\n",
		    CNT dev.tag_types[DEVSTAT_TAG_HEAD]);
		PREF printf("\tsupported statistics measurements flags: %llu\n",
		    CNT dev.flags);
		PREF printf("\tdevice type: %llu\n", CNT dev.device_type);
		PREF printf("\tdevstat list insert priority: %llu\n",
		    CNT dev.priority);
#undef CNT
#undef PREF
		if (check_dev != NULL) {
			found = 1;
			break;
		}
	}

	free((stats.dinfo)->mem_ptr);

	if(kd != NULL)
		kvm_close(kd);

	if ((check_dev != NULL) && (found == 0))
		errx(1, "device %s is not registered in devstat", check_dev);

	return 0;
}
