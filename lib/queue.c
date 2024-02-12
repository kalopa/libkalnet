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

struct	kn_queue	queues[KN_MAX_Q];

/*
 * Initialize the queueing subsystem.
 */
void
kn_queue_init()
{
	int i;

	for (i = 0; i < KN_MAX_Q; i++)
		queues[i].head = queues[i].tail = NULL;
}

/*
 * Return the head of the channel list.
 */
struct kn_channel *
kn_get_queue_head(int qid)
{
	return(queues[qid].head);
}

/*
 * Move the channel from its current queue to another queue.
 */
void
kn_qmove(struct kn_channel *kcp, int newqid)
{
	if (kcp->qid == newqid)
		return;
	kn_dequeue(kcp);
	kn_enqueue(kcp, newqid);
}

/*
 * Enqueue a channel onto the specified queue.
 */
void
kn_enqueue(struct kn_channel *kcp, int qid)
{
	struct kn_queue *qp = &queues[qid];

	kcp->next = NULL;
	kcp->qid = qid;
	if (qp->head == NULL)
		qp->head = kcp;
	else
		qp->tail->next = kcp;
	qp->tail = kcp;
}

/*
 * Remove a channel from the specified queue.
 */
void
kn_dequeue(struct kn_channel *kcp)
{
	struct kn_queue *qp = &queues[kcp->qid];

	if (qp->head == kcp) {
		/*
		 * On the head of the queue. Do this the easy way.
		 */
		if ((qp->head = kcp->next) == NULL) {
			qp->tail = NULL;
			return;
		}
	} else {
		struct kn_channel *nkcp;

		for (nkcp = qp->head; nkcp->next != NULL; nkcp = nkcp->next) {
			if (nkcp->next == kcp) {
				nkcp->next = kcp->next;
				break;
			}
		}
	}
	/*
	 * Make sure the tail pointer is valid.
	 */
	for (qp->tail = qp->head; qp->tail->next != NULL; qp->tail = qp->tail->next)
		;
}

/*
 * Dump all the queues...
 */
void
kn_dump_queues(void)
{
	int i;
	struct kn_channel *kcp;

	for (i = 0; i < 3; i++) {
		for (kcp = kn_get_queue_head(i); kcp != NULL; kcp = kcp->next)
			printf("Q%d> CH%d: %d %d\n", i, kcp->channo, kcp->fd, kcp->type);
	}
}
