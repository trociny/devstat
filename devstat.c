/*
 * Copyright (c) 2008 Mikolaj Golub
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
 * $Id: devstat.c,v 1.1.1.1 2008/04/12 17:16:55 mikolaj Exp $
 *
 */

#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <paths.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <devstat.h>

#define MAXNAMELEN	256
#define _CNT		(unsigned long long)

int main (int argc, char* argv[]) {
	struct statinfo	stats;
	struct devinfo	dinfo;
	int		i, found;
	char		*check_dev = NULL;

	if (argc > 1)
		check_dev = argv[1];

	if ( devstat_checkversion(NULL) == -1) {
		fprintf(stderr, "userland and kernel devstat version mismatch\n");
		return 1;
	}

	memset(&stats, 0, sizeof(stats));
	memset(&dinfo, 0, sizeof(dinfo));
	stats.dinfo = &dinfo;

	if (devstat_getdevs(NULL, &stats) == -1) {
		fprintf(stderr, "devstat_getdevs failed\n");
		return 1;
	}

	for(i = 0; i < (stats.dinfo)->numdevs; i++) {
		char	dev_name[MAXNAMELEN];
		struct devstat	dev = (stats.dinfo)->devices[i];
		snprintf(dev_name, MAXNAMELEN-1, "%s%d",
			 dev.device_name, dev.unit_number);
		if ((check_dev != NULL) && (strcmp(check_dev, dev_name) != 0))
			continue;
		found = 1;

		printf("%s:\n", dev_name);
		printf("\t%llu bytes read\n",    _CNT dev.bytes[DEVSTAT_READ]);
		printf("\t%llu bytes written\n", _CNT dev.bytes[DEVSTAT_WRITE]);
		printf("\t%llu bytes freed\n",   _CNT dev.bytes[DEVSTAT_FREE]);
		printf("\t%llu reads\n",  _CNT dev.operations[DEVSTAT_READ]);
		printf("\t%llu writes\n", _CNT dev.operations[DEVSTAT_WRITE]);
		printf("\t%llu frees\n",  _CNT dev.operations[DEVSTAT_FREE]);
		printf("\t%llu other\n",  _CNT dev.operations[DEVSTAT_NO_DATA]);
		printf("\tduration:\n");
		printf("\t\t%llu %llu/2^64 sec reads\n",
		       _CNT dev.duration[DEVSTAT_READ].sec, _CNT dev.duration[DEVSTAT_READ].frac);
		printf("\t\t%llu %llu/2^64 sec writes\n",
		       _CNT dev.duration[DEVSTAT_WRITE].sec, _CNT dev.duration[DEVSTAT_WRITE].frac);
		printf("\t\t%llu %llu/2^64 sec frees\n",
		       _CNT dev.duration[DEVSTAT_FREE].sec, _CNT dev.duration[DEVSTAT_FREE].frac);
		printf("\t%llu %llu/2^64 sec busy time\n", _CNT dev.busy_time.sec, _CNT dev.busy_time.frac);
		printf("\t%llu %llu/2^64 sec creation time\n", _CNT dev.creation_time.sec, _CNT dev.creation_time.frac);
		printf("\t%llu block size\n", _CNT dev.block_size);
		printf("\ttags sent:\n");
		printf("\t\t%llu simple\n",        _CNT dev.tag_types[DEVSTAT_TAG_SIMPLE]);
		printf("\t\t%llu ordered\n",       _CNT dev.tag_types[DEVSTAT_TAG_ORDERED]);
		printf("\t\t%llu head of queue\n", _CNT dev.tag_types[DEVSTAT_TAG_HEAD]);
		printf("\tsupported statisctics measurments flags: %llu\n", _CNT dev.flags);
		printf("\tdevice type: %llu\n", _CNT dev.device_type);
		printf("\tdevstat list insert priority: %llu\n", _CNT dev.priority);

		if (check_dev != NULL) {
			free((stats.dinfo)->mem_ptr);
			return 0;
		}
	}

	free((stats.dinfo)->mem_ptr);
	
	if (check_dev != NULL) {
		fprintf(stderr, "device %s is not registered in devstat\n", check_dev);
		return 1;
	}

	return 0;
}
