/*
 * Copyright (c) 2024, Kalopa Robotics Limited.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither Kalopa Robotics Limited nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY KALOPA ROBOTICS LIMITED AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL KALOPA ROBOTICS LIMITED OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "libkalnet.h"

int				maxfds;
static fd_set		mrdfdset;
static fd_set		mwrfdset;

/*
 * Initialize the main loop code.
 */
void
kn_loop_init(void)
{
	maxfds = 0;
	FD_ZERO(&mrdfdset);
	FD_ZERO(&mwrfdset);
}

/*
 * Wait for a hit on the various file descriptors, or a timeout.
 * Whichever happens first.
 */
void
kn_main_loop(void)
{
	int n, ret = 0;
	fd_set rfds, wfds;
	struct timeval tv;
	struct kn_channel *kcp;
	time_t now;

	tv.tv_sec = 300;
	tv.tv_usec = 0;
	memcpy((char *)&rfds, (char *)&mrdfdset, sizeof(fd_set));
	memcpy((char *)&wfds, (char *)&mwrfdset, sizeof(fd_set));
	if ((n = select(maxfds, &rfds, &wfds, NULL, &tv)) < 0) {
		if (errno == EINTR)
			return;
		perror("kn_main_loop: select");
		exit(1);
	}
	if (n == 0)
		return;
	time(&now);
	/*
	 * Scan through the active channels, looking for read/write action.
	 */
	kn_dump_queues();
	for (kcp = kn_get_queue_head(KN_BUSY_Q); kcp != NULL; kcp = kcp->next) {
		if (FD_ISSET(kcp->fd, &rfds)) {
			kcp->last_read = now;
			if (kcp->type == KN_CHANNEL_TYPE_TCP_ACCEPT)
				ret = kn_tcp_accept(kcp);
			else
				ret = kcp->rdfunc(kcp);
			if (ret < 0) {
				kn_channel_close(kcp);
				continue;
			}
		}
		if (FD_ISSET(kcp->fd, &wfds) && kcp->wrfunc(kcp) < 0)
			kn_channel_close(kcp);
	}
}

/*
 * Enable a channel for read or write.
 */
void
kn_enable_channel_fd(struct kn_channel *kcp, int rw)
{
	if (rw & KN_IO_READ) {
		if (maxfds <= kcp->fd)
			maxfds = kcp->fd + 1;
		FD_SET(kcp->fd, &mrdfdset);
	}
	if (rw & KN_IO_WRITE) {
		if (maxfds <= kcp->fd)
			maxfds = kcp->fd + 1;
		FD_SET(kcp->fd, &mwrfdset);
	}
}

/*
 * Disnable a channel for reading and/or writing. Probably because it's closed.
 */
void
kn_disable_channel_fd(struct kn_channel *kcp, int rw)
{
	if (rw & KN_IO_READ)
		FD_CLR(kcp->fd, &mrdfdset);
	if (rw & KN_IO_WRITE)
		FD_CLR(kcp->fd, &mwrfdset);
}
