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
#define KN_READSIZE	4096

struct	kn_channel	{
	struct kn_channel	*next;
	struct kn_buffer	*bhead;
	int					fd;
	int					type;
	int					state;
	int					channo;
	int					qid;
	int					(*rdfunc)(struct kn_channel *);
	int					(*wrfunc)(struct kn_channel *);
	int					total_read;
	time_t				last_read;
};

#define KN_CHANNEL_TYPE_UNKNOWN		0
#define KN_CHANNEL_TYPE_UDP			1
#define KN_CHANNEL_TYPE_TCP_ACCEPT	2
#define KN_CHANNEL_TYPE_TCP_CONNECT	3

#define KN_CHANNEL_STATE_NEW		0
#define KN_CHANNEL_STATE_ACTIVE		1
#define KN_CHANNEL_STATE_DEAD		2

#define KN_IO_READ				01
#define KN_IO_WRITE				02

/*
 * We maintain three queues. A free queue (for new channels), an
 * idle queue for channels which aren't busy and a busy queue.
 */
struct	kn_queue	{
	struct kn_channel	*head;
	struct kn_channel	*tail;
};

#define KN_FREE_Q		0
#define KN_IDLE_Q		1
#define KN_BUSY_Q		2
#define KN_MAX_Q		3

/*
 * All reads and writes are from the buffer queue.
 */
struct	kn_buffer	{
	struct kn_buffer	*next;
	int					size;
	char				data[KN_READSIZE];
};

/*
 * Prototypes
 */
void				kn_init(void);
void				kn_queue_init(void);
void				kn_loop_init(void);
void				kn_main_loop(void);
struct kn_channel	*kn_tcp_listen(char *, int);
int					kn_tcp_accept(struct kn_channel *kcp);
struct kn_channel	*kn_channel_alloc(void);
void				kn_channel_close(struct kn_channel *);
void				kn_enable_channel_fd(struct kn_channel *, int);
void				kn_disable_channel_fd(struct kn_channel *, int);
struct kn_channel	*kn_get_queue_head(int);
void				kn_qmove(struct kn_channel *, int);
void				kn_enqueue(struct kn_channel *, int);
void				kn_dequeue(struct kn_channel *);
void				kn_dump_queues(void);
void				kn_parse_host(char *, void *);
int					kn_is_active(struct kn_channel *);
