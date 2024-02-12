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
#include <string.h>

#include "libkalnet.h"

/*
 * Allocate a kn_channel structure and return a pointer to it.
 */
struct kn_channel *
kn_channel_alloc(void)
{
	static int next_channo = 0;
	struct kn_channel *kcp;

	if ((kcp = kn_get_queue_head(KN_FREE_Q)) == NULL) {
		if ((kcp = malloc(sizeof(struct kn_channel))) == NULL) {
			perror("kn_channel_alloc: malloc");
			exit(1);
		}
		memset(kcp, 0, sizeof(struct kn_channel));
		kn_enqueue(kcp, KN_BUSY_Q);
	} else
		kn_qmove(kcp, KN_BUSY_Q);
	kcp->fd = -1;
	kcp->state = KN_CHANNEL_STATE_ACTIVE;
	kcp->type = KN_CHANNEL_TYPE_UNKNOWN;
	kcp->channo = ++next_channo;
	kcp->rdfunc = kcp->wrfunc = NULL;
	return(kcp);
}

/*
 * Channel read or write error. It's dead. Kill it.
 */
void
kn_channel_close(struct kn_channel *kcp)
{
	if (kcp->fd >= 0)
		close(kcp->fd);
	kcp->state = KN_CHANNEL_STATE_DEAD;
	kn_disable_channel_fd(kcp, KN_IO_READ | KN_IO_WRITE);
	kn_qmove(kcp, KN_FREE_Q);
}

/*
 * Is this channel still active?
 */
int
kn_is_active(struct kn_channel *kcp)
{
	return(kcp->state == KN_CHANNEL_STATE_ACTIVE);
}
