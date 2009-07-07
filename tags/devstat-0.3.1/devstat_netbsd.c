/*
 * Copyright (c) 2009 Mikolaj Golub
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
 * $Id: devstat_netbsd.c,v 1.2 2009/07/05 19:46:30 mikolaj Exp $
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
#include <sys/sysctl.h>
#include <sys/iostat.h>
#include <limits.h>

#define MAXNAMELEN	256

static struct nlist namelist[] = {
#define X_DRIVE_COUNT   0
        { .n_name = "_iostat_count" },  /* number of drives */
#define X_DRIVELIST     1
        { .n_name = "_iostatlist" },    /* TAILQ of drives */
        { .n_name = NULL },
};

static int		ndisk;
static struct io_sysctl	*dk_sysctl;
static struct io_stats	*dk_kvm;
static kvm_t		*kd;

static int
devstat_getdevs(void)
{
	if (kd != NULL) {
		struct io_stats		*p, *dk = NULL;
		struct iostatlist_head	dkhead;
		int			i;

 		if (kvm_read(kd, namelist[X_DRIVE_COUNT].n_value, &ndisk, sizeof(ndisk)) < 0)
			return -1;
		if (ndisk <= 0)
			return -1;

		dk = malloc(ndisk * sizeof(*dk));
		if (dk == NULL)
			return -1;

		if (kvm_read(kd, namelist[X_DRIVELIST].n_value, &dkhead, sizeof(dkhead)) < 0) {
			free(dk);
			return -1;
		}

		for (p = dkhead.tqh_first, i = 0; i < ndisk; p = dk[i].io_link.tqe_next, i++) 
			if (kvm_read(kd, (u_long) p, &dk[i], sizeof(dk[i])) < 0) {
				free(dk);
				return -1;
			}

		dk_kvm = dk;

	} else {

		struct io_sysctl	*dk = NULL;
		int	dmib[3] = {CTL_HW, HW_IOSTATS, sizeof(struct io_sysctl)};

		size_t	size;

		if (sysctl(dmib, 3, NULL, &size, NULL, 0) < 0) {
			perror("Can't get size of HW_DISKSTATS/HW_IOSTATS mib");
			return -1;
		}
	
		ndisk = size / dmib[2];
		if (ndisk == 0)
			return -1;

		dk = malloc(ndisk * sizeof(*dk));
		if (dk == NULL)
			return -1;

		if (sysctl(dmib, 3, dk, &size, NULL, 0) < 0) {
			perror("Can't get HW_DISKSTATS/HW_IOSTATS mib");
			return -1;
		}

		dk_sysctl = dk;
	}

	return 0;
}

static void
usage (char* progname) {
	fprintf(stderr, "usage: %s [-M core] [-N system] [devname]\n", progname);
}

int
main (int argc, char* argv[]) {
	int		c, i, found;
	char		*memf = NULL, *nlistf = NULL, *check_dev = NULL;
	char		errbuf[_POSIX2_LINE_MAX];

	while ((c = getopt(argc, argv, "M:N:")) != -1) {
		switch(c) {
		case 'N':
			nlistf = optarg;
			break;
		case 'M':
			memf = optarg;
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

                if (kvm_nlist(kd, namelist))
                        errx(1, "kvm_nlist failed to read symbols");
	}

	if (devstat_getdevs() == -1)
		errx(1, "devstat_getdevs failed");

	for (found = 0, i = 0; i < ndisk; i++) {

#define CNT (unsigned long long)
#define GET_STATS(var) \
	((kd == NULL) ? dk_sysctl[i].var : dk_kvm[i].io_##var)
#define GET_STATS_TIME(var, sec) \
	((kd == NULL) ? CNT dk_sysctl[i].var##_##sec : CNT dk_kvm[i].io_##var.tv_##sec)

		if ((check_dev != NULL) && (strcmp(check_dev, GET_STATS(name)) != 0))
			continue;

		printf("%s:\n", GET_STATS(name));
		printf("\tdevice type: %llu\n",  CNT GET_STATS(type));
		printf("\tbusy counter: %llu\n", CNT GET_STATS(busy));
		printf("\t%llu bytes read\n",    CNT GET_STATS(rbytes));
		printf("\t%llu bytes written\n", CNT GET_STATS(wbytes));
		printf("\t%llu reads\n",  CNT GET_STATS(rxfer));
		printf("\t%llu writes\n", CNT GET_STATS(wxfer));
		printf("\t%llu seeks\n",  CNT GET_STATS(wxfer));
		printf("\t%llu.%06llu sec spent busy\n",
			GET_STATS_TIME(time, sec),
			GET_STATS_TIME(time, usec));
		printf("\tattach timestamp: %llu.%06llu sec\n",
			GET_STATS_TIME(attachtime, sec),
			GET_STATS_TIME(attachtime, usec));
		printf("\tunbusy timestamp: %llu.%06llu sec\n",
			GET_STATS_TIME(timestamp, sec),
			GET_STATS_TIME(timestamp, usec));

		if (check_dev != NULL) {
			found = 1;
			break;
		}

#undef CNT
#undef GET_STATS
#undef GET_STATS_TIME

	}

	if (kd != NULL) {
		kvm_close(kd);
		free(dk_kvm);
	} else {
		free(dk_sysctl);
	}
	
	if ((check_dev != NULL) && (found == 0))
		errx(1, "device %s is not registered in iostat", check_dev);

	return 0;
}
