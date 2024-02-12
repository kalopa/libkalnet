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

#include "libkalnet.h"

/*
 * This is the reader call-back function that will be called when data is
 * available on the TCP channel.
 */
int
my_reader(struct kn_channel *kcp)
{
	char buf[1024];
	int len;

	len = read(kcp->fd, buf, sizeof(buf) - 1);
	printf("MyReader returned %d.\n", len);
	if (len <= 0)
		return(-1);
	buf[len] = '\0';
	printf("Received:\n%s---\n", buf);
	return(len);
}

/*
 * Set up a simple TCP server on port 8080 just for testing purposes.
 */
int
main(int argc, char *argv[])
{
	struct kn_channel *kcp;

	kn_init();
	kcp = kn_tcp_listen("localhost", 8080);
	kcp->rdfunc = my_reader;
	while (kn_is_active(kcp)) {
		kn_main_loop();
		printf("Back!\n");
	}
	exit(0);
}
