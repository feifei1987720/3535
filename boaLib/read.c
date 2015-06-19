

#include "boa.h"
#include "upgrade.h"


int read_header(request * req, node_t * node)
{
	int bytes, buf_bytes_left;
	char *check, *buffer;

	check = req->client_stream + req->parse_pos;
	buffer = req->client_stream;
	bytes = req->client_stream_pos;

	//fprintf(stderr,"buffer=%s\n",buffer);
	while (check < (buffer + bytes)) {
		switch (req->status) {
			case READ_HEADER:
				if (*check == '\r') 
				{
					req->status = ONE_CR;
					req->header_end = check;
				} else if (*check == '\n') {
					req->status = ONE_LF;
					req->header_end = check;
				}
				break;

			case ONE_CR:
				if (*check == '\n')
				{
					req->status = ONE_LF;
				}else if (*check != '\r'){
					req->status = READ_HEADER;
				}
				break;

			case ONE_LF:
				/* if here, we've found the end (for sure) of a header */
				if (*check == '\r') /* could be end o headers */
					req->status = TWO_CR;
				else if (*check == '\n')
					req->status = BODY_READ;
				else
					req->status = READ_HEADER;
				break;

			case TWO_CR:
				if (*check == '\n')
					req->status = BODY_READ;
				else if (*check != '\r')
					req->status = READ_HEADER;
				break;

			default:
				break;
		}


		req->parse_pos++;       /* update parse position */
		check++;
		//fprintf(stderr,"req->status=%d file =%s line %d\n",req->status,__FILE__,__LINE__);
		if (req->status == ONE_LF)
		{
			*req->header_end = '\0';

			/* terminate string that begins at req->header_line */

			if (req->logline)
			{
				if (process_option_line(req) == 0)
				{
					return 0;
				}
			} else {
				if (process_logline(req) == 0)
				{
					return 0;
				}
				if (req->simple)
				{
					return process_header_end(req,node);
				}
			}
			/* set header_line to point to beginning of new header */
			req->header_line = check;
		} 
		else if (req->status == BODY_READ)
		{

			int retval = process_header_end(req,node);
			/* process_header_end inits non-POST cgi's */

			if (retval && req->method == M_POST) 
			{
				/* for body_{read,write}, set header_line to start of data,
				   and header_end to end of data */
				req->header_line = check;
				req->header_end =req->client_stream + req->client_stream_pos;

				req->status = BODY_WRITE;
			
				//fprintf(stderr,"req->content_length =%s  file %s line %d\n",req->content_length,__FILE__,__LINE__);
				if (req->content_length)
				{
					int content_length;

					content_length = boa_atoi(req->content_length);
					/* Is a content-length of 0 legal? */
					if (content_length <= 0) 
					{
						log_error_time();
						fprintf(stderr, "Invalid Content-Length [%s] on POST!\n",req->content_length);
						send_r_bad_request(req);
						return 0;
					}
					if (single_post_limit && content_length > single_post_limit) 
					{
						log_error_time();
						fprintf(stderr, "Content-Length [%d] > SinglePostLimit [%d] on POST!\n",content_length, single_post_limit);
						send_r_bad_request(req);
						return 0;
					}
					req->filesize = content_length;
					req->filepos = 0;
					if (req->header_end - req->header_line > req->filesize)
					{
						req->header_end = req->header_line + req->filesize;
					}
				} 
				else 
				{
					log_error_time();
					fprintf(stderr, "Unknown Content-Length POST!\n");
					send_r_bad_request(req);
					return 0;
				}
			}                   /* either process_header_end failed or req->method != POST */
			//fprintf(stderr,"cccccccccccc file %s line  %d\n",__FILE__,__LINE__);
			return retval;      /* 0 - close it done, 1 - keep on ready */
		}                       /* req->status == BODY_READ */
	}                           /* done processing available buffer */
	//fprintf(stderr,"aaaaaaaaaaaaaaaaaaaaaaaaaaa\n");

	if (req->status < BODY_READ)
	{
		/* only reached if request is split across more than one packet */

		buf_bytes_left = CLIENT_STREAM_SIZE - req->client_stream_pos;
		if (buf_bytes_left < 1)
		{
			log_error_time();
			fputs("buffer overrun - read.c, read_header - closing\n",stderr);
			req->status = DEAD;
			return 0;
		}
		//fprintf(stderr,"bbbbbbbbbbbbbbbbbbbbbbbb  %s %d\n",__FILE__,__LINE__);
		/*MN stuff to support ssl*/
		bytes = read(req->fd, buffer + req->client_stream_pos, buf_bytes_left);	
		if(bytes > 0 && req->client_stream_pos == 0)
		{ //<>
			//buffer[bytes]='\0';
			//fprintf(stderr, "buffer: %s\n", buffer);		
			if(strstr(buffer, UCTRL_HEAD) != NULL)
			{
				fprintf(stderr, "Upgrade~~~~\n");
				return RET_UPGRADE;
			}
		}


		if (bytes < 0)
		{
			if (errno == EINTR)
				return 1;
			if (errno == EAGAIN || errno == EWOULDBLOCK) /* request blocked */
				return -1;
			/*
			   else if (errno == EBADF || errno == EPIPE) {

			   req->status = DEAD;
			   return 0;
			 */
			log_error_doc(req);
			perror("header read");            /* don't need to save errno because log_error_doc does */
			return 0;
		}
		else if (bytes == 0) 
		{
			/*
			   log_error_time();
			   fputs("unexpected end of headers\n", stderr);
			 */
			return 0;
		}

		/* bytes is positive */
		req->client_stream_pos += bytes;

		return 1;
	}
	//fprintf(stderr,"cccccccccccccccccccccccccccc\n");
	return 1;
}

	
int read_body(request * req)
{
	int bytes_read, bytes_to_read, bytes_free;

	bytes_free = BUFFER_SIZE - (req->header_end - req->header_line);
	bytes_to_read = req->filesize - req->filepos;

	if (bytes_to_read > bytes_free)
		bytes_to_read = bytes_free;

	if (bytes_to_read <= 0)
	{
		req->status = BODY_WRITE; /* go write it */
		return 1;
	}


	bytes_read = read(req->fd, req->header_end, bytes_to_read);
	if (bytes_read == -1)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
		{
				/*
				   req->status = BODY_WRITE;
				   return 1;
				 */
			return -1;
		}else
		{
			boa_perror(req, "read body");
			return 0;
		}
	}
	else if(bytes_read == 0)
	{
		/* this is an error.  premature end of body! */
		log_error_time();
		fprintf(stderr, "%s:%d - Premature end of body!!\n",__FILE__, __LINE__);
		send_r_bad_request(req);
		return 0;
	}

	req->status = BODY_WRITE;
	req->header_end += bytes_read;

	return 1;
}

	

int write_body(request * req)
{
	int bytes_written, bytes_to_write = req->header_end - req->header_line;
	if (req->filepos + bytes_to_write > req->filesize)
		bytes_to_write = req->filesize - req->filepos;

	if (bytes_to_write == 0) 
	{  /* nothing left in buffer to write */
		req->header_line = req->header_end = req->buffer;
		if (req->filepos >= req->filesize)
			return init_cgi(req);
		/* if here, we can safely assume that there is more to read */
		req->status = BODY_READ;
		return 1;
	}
	bytes_written = write(req->post_data_fd,req->header_line, bytes_to_write);

	if (bytes_written == -1)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return -1;          /* request blocked at the pipe level, but keep going */
		else if (errno == EINTR)
			return 1;
		else if (errno == ENOSPC)
		{
			/* 20010520 - Alfred Fluckiger */
			/* No test was originally done in this case, which might  */
			/* lead to a "no space left on device" error.             */
			boa_perror(req, "write body"); /* OK to disable if your logs get too big */
			return 0;
		}
		else
		{
			boa_perror(req, "write body"); /* OK to disable if your logs get too big */
			return 0;
		}
	}

	req->filepos += bytes_written;
	req->header_line += bytes_written;

	return 1;                   /* more to do */
}
