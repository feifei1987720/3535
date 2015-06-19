/*
 * Device interface to retreive or update settings
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define INITIALIZE_TAGS

#include "sys_env_type.h"
#include "net_config.h"
#include "web_translate.h"
#include "file_list.h"
#include "psia.h"
#include "tags.h"
#include "httpstate.h"



/*******************************************************************/
/*                        Standard Response                        */
/*******************************************************************/
void add_response_status(request *req, node_t *node)
{
	#if 0
	char scstr[32];

	sprintf(scstr, "%d", req->status_code);
	xadd_stag_attr(req, TAG_ResponseStatus, RootAttrStr);
	xadd_elem(req, TAG_requestURL, req->request_uri);
	xadd_elem(req, TAG_statusCode, scstr);
	xadd_elem(req, TAG_statusString, StatusString[req->status_code]);
	xadd_etag(req, TAG_ResponseStatus);
	#endif
}

/* return the corresponsing HTTP status code */
int get_http_status(request *req)
{
	int http_status;

	http_status = STATUS_INVALID_OPERATION;
	switch(req->status_code) {
		case STATUS_OK:
			http_status = R_REQUEST_OK;
			break;
		case STATUS_DEVICE_BUSY:
			http_status = R_SERVICE_UNAV;
			break;
		case STATUS_DEVICE_ERROR:
			http_status = R_ERROR;
			break;
		case STATUS_INVALID_OPERATION:
			http_status = R_FORBIDDEN;
			break;
		case STATUS_INVALID_XML_FORMAT:
			http_status = R_BAD_REQUEST;
			break;
		case STATUS_INVALID_XML_CONTENT:
			http_status = R_BAD_REQUEST;
			break;
		case STATUS_REBOOT_REQUIRED:
			http_status = R_REQUEST_OK;
			break;
		case STATUS_NOT_IMPLEMENTED:
			http_status = R_SERVICE_UNAV;
			break;
	}
	req->response_complete = 1;
	return http_status;
}

