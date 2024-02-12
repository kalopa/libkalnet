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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "libkalnet.h"

/*
 * Listen for a TCP connection on the named addr:port combo
 */
struct kn_channel *
kn_tcp_listen(char *src_host, int src_port)
{
	char *cp;
	struct kn_channel *kcp = kn_channel_alloc();
	struct sockaddr_in sin;

	/*
	 * Did the caller specify a host:port sequence?
	 */
	if ((cp = strchr(src_host, ':')) != NULL) {
		*cp++ = '\0';
		src_port = atoi(cp);
	}
	/*
	 * Open a TCP port for listening...
	 */
	if ((kcp->fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("kn_tcp_listen: socket");
		exit(1);
	}
	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	kn_parse_host(src_host, &sin.sin_addr.s_addr);
	sin.sin_port = htons(src_port);
	if (bind(kcp->fd, (const struct sockaddr *)&sin, sizeof(struct sockaddr_in)) < 0) {
		perror("kn_tcp_listen: bind");
		exit(1);
	}
	listen(kcp->fd, 32);
	kcp->type = KN_CHANNEL_TYPE_TCP_ACCEPT;
	kcp->state = KN_CHANNEL_STATE_ACTIVE;
	kn_enable_channel_fd(kcp, KN_IO_READ);
	kn_dump_queues();
	return(kcp);
}

/*
 * Accept a connection on the specified channel.
 */
int
kn_tcp_accept(struct kn_channel *kcp)
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);
	struct kn_channel *newkcp;

	newkcp = kn_channel_alloc();
	if ((newkcp->fd = accept(kcp->fd, (struct sockaddr *)&sin, &len)) < 0) {
		perror("kn_tcp_accept: accept");
		return(-1);
	}
	newkcp->type = KN_CHANNEL_TYPE_TCP_CONNECT;
	newkcp->state = KN_CHANNEL_STATE_ACTIVE;
	newkcp->rdfunc = kcp->rdfunc;
	newkcp->wrfunc = kcp->wrfunc;
	kn_enable_channel_fd(newkcp, KN_IO_READ);
	return(newkcp->fd);
}
