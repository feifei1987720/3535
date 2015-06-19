/*

 *
 */

/* $Id: select.c,v 1.1.2.2 2002/07/23 15:54:52 jnelson Exp $*/

#include "boa.h"

int g_boaRun = 0;
int g_updateParam = 0;

fd_set block_read_fdset;
fd_set block_write_fdset;

static void fdset_update(void)
{
	request *current, *next;

	for(current = request_block;current;current = next)
	{
		time_t time_since = current_time - current->time_last;
		next = current->next;

			/* hmm, what if we are in "the middle" of a request and not
			 * just waiting for a new one... perhaps check to see if anything
			 * has been read via header position, etc... */
		if (current->kacount < ka_max && (time_since >= ka_timeout) && !current->logline)  /* haven't read anything yet */
				current->status = DEAD; /* connection keepalive timed out */
			else if (time_since > REQUEST_TIMEOUT) {
				log_error_doc(current);
				fputs("connection timed out\n", stderr);
				current->status = DEAD;
			}
			if (current->buffer_end && current->status < DEAD)
			{
				if (FD_ISSET(current->fd, &block_write_fdset))
					ready_request(current);
				else {
					BOA_FD_SET(current->fd, &block_write_fdset);
				}
			} else {
				switch (current->status)
				{
					case WRITE:
					case PIPE_WRITE:
						if (FD_ISSET(current->fd, &block_write_fdset))
							ready_request(current);
						else {
							BOA_FD_SET(current->fd, &block_write_fdset);
						}
						break;
					case BODY_WRITE:
						if (FD_ISSET(current->post_data_fd, &block_write_fdset))
							ready_request(current);
						else {
							BOA_FD_SET(current->post_data_fd, &block_write_fdset);
						}
						break;
					case PIPE_READ:
						if (FD_ISSET(current->data_fd, &block_read_fdset))
							ready_request(current);
						else {
							BOA_FD_SET(current->data_fd, &block_read_fdset);
						}
						break;
					case DONE:
						if (FD_ISSET(current->fd, &block_write_fdset))
							ready_request(current);
						else {
							BOA_FD_SET(current->fd, &block_write_fdset);
						}
						break;
					case DEAD:
						ready_request(current);
						break;
					default:
						if (FD_ISSET(current->fd, &block_read_fdset))
							ready_request(current);
						else {
							BOA_FD_SET(current->fd, &block_read_fdset);
						}
						break;
				}
			}
			current = next;
	}
}

void select_loop(int server_s)
{
	struct timeval req_timeout;
	FD_ZERO(&block_read_fdset);
	FD_ZERO(&block_write_fdset);
	
	req_timeout.tv_sec = (ka_timeout ? ka_timeout : REQUEST_TIMEOUT);
	req_timeout.tv_usec = 0l;

	while (g_boaRun)
	{
		if(commonGetUpdataBoaParamFlag() == 1)
		{
			break;
		}

		/* reset max_fd */
		max_fd = -1;
		/* move selected req's from request_block to request_ready */
		if (request_block)
		{
			fdset_update();
		}
		/* any blocked req's move from request_ready to request_block */
		process_requests(server_s);

		//fflush(stdout);

		if (!sigterm_flag && total_connections < (max_connections - 10))
		{
			BOA_FD_SET(server_s, &block_read_fdset); /* server always set */
		}

		req_timeout.tv_sec = (request_ready ? 0 :(ka_timeout ? ka_timeout : REQUEST_TIMEOUT));
		req_timeout.tv_usec = 0l;    /* reset timeout */
		req_timeout.tv_sec = 0;
		req_timeout.tv_usec = 100000;   /* reset timeout */

		if (select(max_fd + 1, &block_read_fdset,&block_write_fdset, NULL,&req_timeout) == -1)
		{
			/* what is the appropriate thing to do here on EBADF */
			if (errno == EINTR)
			{
				continue;   /* while(1) */
			}
			else if (errno != EBADF)
			{
				DIE("select");
			}
		}

		time(&current_time);
		if (FD_ISSET(server_s, &block_read_fdset))
		{
			pending_requests = 1;
		}
	}
}

void boaUpdateParam()
{
	g_updateParam = 1;
}

