/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <paulp@go2net.com>
 *  Some changes Copyright (C) 1996,97 Larry Doolittle <ldoolitt@boa.org>
 *  Some changes Copyright (C) 1996-2002 Jon Nelson <jnelson@boa.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* $Id: request.c,v 1.112.2.3 2002/07/24 03:03:59 jnelson Exp $*/
#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include "boa.h"
#include <stddef.h> 
#include "httpstate.h"
#include "tags.h"
#include "globals.h"

#include "appro_api.h"
#include "upgrade.h"
#include "response.h"


int total_connections;
struct status status;

static int sockbufsize = SOCKETBUF_SIZE;

/* function prototypes located in this file only */
static void free_request(request ** list_head_addr, request * req);

typedef struct __URI_HASH_TABLE {
	HTTP_URI *entry;

} URI_HASH_TABLE;

URI_HASH_TABLE *uri_hash;

int uri_jpeg(request * req)
{
	printf(" BOA: JPEG\n");

	req->http_stream = URI_STREAM_JPEG;
	req->status = WRITE;
	req->data_mem = (char*)req->av_data.ptr;
	req->filesize = req->av_data.size+16;
	send_request_ok_jpeg(req);
	return 1;
}

int uri_mjpeg(request * req)
{
	printf(" BOA: MJPEG\n");

	req->http_stream = URI_STREAM_MJPEG;
	req->status = WRITE;
	req->data_mem = (char*)req->av_data.ptr;
	req->filesize = req->av_data.size+16;

	req->serial_lock = req->av_data.serial;
	req->serial_book = req->av_data.serial+1;

	if (strncasecmp(req->request_uri, "/PSIA", 5) == 0)
		send_r_psia_ok_mjpeg(req);
	else
		send_request_ok_mjpeg(req);

	return 1;
}

int uri_mpeg4(request * req)
{
	AV_DATA vol_data;
	memset(&vol_data,0,sizeof(vol_data));

	printf(" BOA: MPEG4\n");

	req->http_stream = URI_STREAM_MPEG4;
	req->status = WRITE;
	req->filesize = req->av_data.size+vol_data.size;

	if (strncasecmp(req->request_uri, "/PSIA", 5) == 0)
		send_r_psia_ok_mpeg4(req);
	else
		send_request_ok_mpeg4(req);

	req_copy(req, (char*)vol_data.ptr, vol_data.size);
	req->data_mem = (char*)req->av_data.ptr;
	req->filesize = req->av_data.size+16;

	req->serial_lock = req->av_data.serial;
	req->serial_book = req->av_data.serial+1;
	return 1;
}

int uri_mpeg4cif(request * req)
{
	AV_DATA vol_data;
	memset(&vol_data,0,sizeof(vol_data));

	printf(" BOA: MPEG4 CIF\n");

	req->http_stream = URI_STREAM_MPEG4CIF;
	req->status = WRITE;
	req->filesize = req->av_data.size+vol_data.size;
	if (strncasecmp(req->request_uri, "/PSIA", 5) == 0)
		send_r_psia_ok_mpeg4(req);
	else
		send_request_ok_mpeg4(req);

	req_copy(req, (char*)vol_data.ptr, vol_data.size);
	req->data_mem = (char*)req->av_data.ptr;
	req->filesize = req->av_data.size+16;

	req->serial_lock = req->av_data.serial;
	req->serial_book = req->av_data.serial+1;
	return 1;
}

int uri_avc(request * req)
{
	AV_DATA vol_data;
	memset(&vol_data,0,sizeof(vol_data));

	printf(" BOA: H264\n");

	req->http_stream = URI_STREAM_AVC;
	req->status = WRITE;
	req->filesize = req->av_data.size+vol_data.size;

	if (strncasecmp(req->request_uri, "/PSIA", 5) == 0)
		send_r_psia_ok_avc(req);
	else
		send_request_ok_avc(req);

	req_copy(req, (char*)vol_data.ptr, vol_data.size);
	req->data_mem = (char*)req->av_data.ptr;
	req->filesize = req->av_data.size+16;

	req->serial_lock = req->av_data.serial;
	req->serial_book = req->av_data.serial+1;
	return 1;
}

int uri_avccif(request * req)
{
	AV_DATA vol_data;
	memset(&vol_data,0,sizeof(vol_data));

	printf(" BOA: H264 CIF\n");

	req->http_stream = URI_STREAM_AVCCIF;
	req->status = WRITE;
	req->filesize = req->av_data.size+vol_data.size;

	if (strncasecmp(req->request_uri, "/PSIA", 5) == 0)
		send_r_psia_ok_avc(req);
	else
		send_request_ok_avc(req);

	req_copy(req, (char*)vol_data.ptr, vol_data.size);
	req->data_mem = (char*)req->av_data.ptr;
	req->filesize = req->av_data.size+16;

	req->serial_lock = req->av_data.serial;
	req->serial_book = req->av_data.serial+1;
	return 1;
}

int uri_vb_htm(request * req)
{
	//fprintf(stderr,"4444444444444444411111444444\n");
	req->is_cgi = CGI;
	if (uri_decoding(req, req->query_string) < 0) {
		send_r_bad_request(req);
		return 0;
	}
	SQUASH_KA(req);
	http_run_command(req, req->cmd_arg, req->cmd_count);
	//fprintf(stderr,"req%s\n",req);
	req->status = DONE;
	return 1;
}

int uri_aevt_htm(request * req)
{
	//fprintf(stderr,"444444444444444422222224444444\n");
	if (uri_decoding(req, req->query_string) >= 0) {
		http_run_command(req, req->cmd_arg, req->cmd_count);
		reset_output_buffer(req);
	}
	return -1;
}

int uri_ini_htm(request * req)
{
	//fprintf(stderr,"44444444444444433333344444444\n");
	char *addr = (char *)malloc(MAX_HTML_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	SQUASH_KA(req);
	req->mem_flag |= MFLAG_IS_MEMORY;
	req->data_mem = addr;
	req->filesize = html_ini_cmd(req->authority, addr, MAX_HTML_LENGTH);
	send_r_request_ok(req);     /* All's well */
	req->status = WRITE;
	return 1;
}

int uri_keyword_htm(request * req)
{
	char *addr = (char *)malloc(MAX_HTML_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	SQUASH_KA(req);
	req->mem_flag |= MFLAG_IS_MEMORY;
	req->data_mem = addr;
	req->filesize = html_keyword_cmd(req->authority, addr, MAX_HTML_LENGTH);
	send_r_request_ok(req);     /* All's well */
	req->status = WRITE;
	return 1;
}

int uri_cmdlist_htm(request * req)
{
	char *addr = (char *)malloc(MAX_HTML_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	SQUASH_KA(req);
	req->mem_flag |= MFLAG_IS_MEMORY;
	req->data_mem = addr;
	req->filesize = html_cmdlist_cmd(req->authority, addr, MAX_HTML_LENGTH);
	send_r_request_ok(req);     /* All's well */
	req->status = WRITE;
	return 1;
}

int uri_sdget_htm(request * req)
{
#if 0
	char *addr, sdpath[128] = {0};
	if (req->query_string && uri_decoding(req, req->query_string) < 0) {
		send_r_bad_request(req);
		return 0;
	}

	addr = (char *)malloc(MAX_SDLIST_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	req->filesize = http_sdget_cmd(req, req->cmd_arg, addr, MAX_SDLIST_LENGTH, sdpath);
	if (req->filesize > 0) {
		SQUASH_KA(req);
		req->mem_flag |= MFLAG_IS_MEMORY;
		req->data_mem = addr;
		send_request_ok_sdget(req);
		req->status = WRITE;
		return 1;
	}
	free(addr);
	if (req->pathname){
		free(req->pathname);
	}
	strcat(sdpath, "/");
	if(req->cmd_arg[0].value==NULL){
		send_r_bad_request(req);
		boa_dbg("req->cmd_arg[0].value is NULL, no SD card\n");
		return 0;
	}
	strcat(sdpath, req->cmd_arg[0].value);
	req->pathname = strdup(sdpath);
	if (!req->pathname) {
		send_r_error(req);
		WARN("unable to strdup buffer onto req->pathname");
		return 0;
	}
	strncpy(req->request_uri, req->cmd_arg[0].value, MAX_HEADER_LENGTH);
	return -1;
#endif
	return 0;

}

int uri_sddel_htm(request * req)
{
#if 0
	SQUASH_KA(req);
	if (uri_decoding(req, req->query_string) < 0) {
		send_r_bad_request(req);
		return 0;
	}
	http_sddel_cmd(req, req->cmd_arg);
	req->status = DONE;
	return 1;
#endif
	return 1;
}

int uri_sysget_htm(request * req)
{
	char *addr = (char *)malloc(MAX_HTML_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	SQUASH_KA(req);
	req->mem_flag |= MFLAG_IS_MEMORY;
	req->data_mem = addr;
	req->filesize = http_sysget_cmd(addr, MAX_HTML_LENGTH);
	send_r_request_ok(req);     /* All's well */
	req->status = WRITE;
	return 1;

}

int uri_accessget_htm(request * req)
{
	char *addr = (char *)malloc(MAX_HTML_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	SQUASH_KA(req);
	req->mem_flag |= MFLAG_IS_MEMORY;
	req->data_mem = addr;
	req->filesize = http_accessget_cmd(addr, MAX_HTML_LENGTH);
	send_r_request_ok(req);     /* All's well */
	req->status = WRITE;
	return 1;

}

int uri_currlistget_htm(request * req)
{
	char *addr = (char *)malloc(MAX_HTML_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	SQUASH_KA(req);
	req->mem_flag |= MFLAG_IS_MEMORY;
	req->data_mem = addr;
	req->filesize = http_actvconlistget_cmd(addr, MAX_HTML_LENGTH);
	send_r_request_ok(req);     /* All's well */
	req->status = WRITE;
	return 1;

}

int uri_paraget_htm(request * req)
{
#if 0
	char *addr = (char *)malloc(MAX_HTML_LENGTH);
	if (addr == NULL) {
		send_r_error(req);
		return 0;
	}
	SQUASH_KA(req);
	req->mem_flag |= MFLAG_IS_MEMORY;
	req->data_mem = addr;
	req->filesize = http_paraget_cmd(addr, MAX_HTML_LENGTH);
	send_r_request_ok(req);     /* All's well */
	req->status = WRITE;
	return 1;
#endif
return 1;

}

HTTP_URI HttpUri [] =
{
	{JPEG_FRAME_URI		,uri_jpeg		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/ipcam/mjpeg.cgi"		,uri_mjpeg		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/ipcam/mjpegcif.cgi"	,uri_mjpeg		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/ipcam/mpeg4.cgi"		,uri_mpeg4		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/ipcam/mpeg4cif.cgi"	,uri_mpeg4cif		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/ipcam/avc.cgi"		,uri_avc		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/ipcam/avccif.cgi"	,uri_avccif		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/vb.htm"			,uri_vb_htm		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/ini.htm"			,uri_ini_htm		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/keyword.htm"		,uri_keyword_htm	,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/cmdlist.htm"		,uri_cmdlist_htm	,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/sdget.htm"		, NULL/*uri_sdget_htm*/		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/sddel.htm"		,uri_sddel_htm		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },
	{"/sysget.htm"		,uri_sysget_htm		,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },   //rajiv
	{"/accessget.htm"		,uri_accessget_htm	,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },   //rajiv
	{"/actvlistget.htm"		,uri_currlistget_htm	,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },   //rajiv
	{"/paraget.htm"		,NULL/*uri_paraget_htm*/	,AUTHORITY_VIEWER   ,URI_FLAG_VIRTUAL_PAGE ,NULL },   //rajiv
	{"/"			,NULL			,AUTHORITY_VIEWER   ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/aenable.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/aevt.htm"                ,uri_aevt_htm		,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/armio.htm"               ,NULL			,AUTHORITY_ADMIN    ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/axtext.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/bwcntl.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_armio.htm"             ,NULL			,AUTHORITY_ADMIN    ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_axtext.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_bwcntl.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_dv840output.htm"       ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_help.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_lang.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_mcenter.htm"           ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/cms.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_ndhcpsvr.htm"          ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_nftphost.htm"          ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_nupnp.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_sccd.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_svideo.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_tailpage.htm"          ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/c_update.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/datetime.htm"            ,NULL			,AUTHORITY_VIEWER   ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/dv840output.htm"         ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/faq_cs_cz.html"          ,NULL			,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_de_de.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_en_us.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_es_es.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_fi_fi.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_fr_fr.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_hu_hu.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_it_it.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_nl_nl.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_pl_pl.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_pt_pt.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_ro_ro.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_sv_se.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_zh_cn.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/faq_zh_tw.html"          ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/help.htm"                ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/image.htm"               ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/imgdaynight.htm"         ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/img_h264.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/img.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/imgmask.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/imgtune.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/index.htm"               ,NULL			,AUTHORITY_VIEWER   ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/index.html"              ,NULL			,AUTHORITY_VIEWER   ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/K_dragon.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/K_fastrax.htm"           ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/K_lilin.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/K_seeku.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/K_sensor.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/lang.htm"                ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/logout.htm"		,NULL			,AUTHORITY_VIEWER   ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/main.htm"                ,NULL			,AUTHORITY_VIEWER   ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/mcenter.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/motion.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nddns.htm"               ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/ndhcpsvr.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/net.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nftphost.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nftp.htm"                ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nipfilter.htm"           ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/npppoe.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nsmtp.htm"               ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nsntp.htm"               ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/multicast.htm"           ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nupnp.htm"               ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/nwireless.htm"           ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/pda.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/pppoedes_cs_cz.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_de_de.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_en_us.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_es_es.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_fi_fi.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_fr_fr.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_hu_hu.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_it_it.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_nl_nl.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_pl_pl.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_pt_pt.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_ro_ro.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_sv_se.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_zh_cn.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/pppoedes_zh_tw.html"     ,NULL                   ,AUTHORITY_OPERATOR ,0 ,NULL },
	{"/p_pelcod.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/p_pelcop.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/ptz.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/ptz_left.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/renable.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/rsch.htm"                ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/saudio.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/sccd.htm"                ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/sdigital.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/sdt.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/setcard.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/setftp.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/setsmtp.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/setvid.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/srs.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/sseq.htm"                ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/sts.htm"                 ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/suser.htm"               ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/svideo.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/tailpage.htm"            ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/update.cgi"		,NULL 			,AUTHORITY_OPERATOR ,0                     ,NULL },
	{"/update.htm"              ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/version.htm"             ,NULL			,AUTHORITY_OPERATOR ,URI_FLAG_NEED_PARSE   ,NULL },
	{"/var.js"			,NULL			,AUTHORITY_VIEWER   ,URI_FLAG_NEED_PARSE   ,NULL },
};

#define URI_HASH_SIZE	(sizeof(HttpUri)/sizeof(HTTP_URI))

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
unsigned int uri_hash_cal_value(char *name)
{
	unsigned int value = 0;

	while (*name)
		value = value * 37 + (unsigned int)(*name++);
	return value;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void uri_hash_insert_entry(URI_HASH_TABLE *table, HTTP_URI *arg)
{
	if (table->entry) {
		arg->next = table->entry;
	}
	table->entry = arg;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int uri_hash_table_init(void)
{
	int i;

	if ( (uri_hash = (URI_HASH_TABLE *)calloc(sizeof(URI_HASH_TABLE), MAX_URI_HASH_SIZE)) == NULL) {
		boa_dbg("uri_hash_table_init: Allocate memory fail!!!\n");
		return -1;
	}
	for (i=0; i<URI_HASH_SIZE; i++) {
		uri_hash_insert_entry(uri_hash+(uri_hash_cal_value(HttpUri[i].name)%MAX_URI_HASH_SIZE), HttpUri+i);
	}
	return 0;
}

HTTP_URI *http_uri_search(char *arg)
{
	HTTP_URI *opt;
	//fprintf(stderr,"11111111111 %s %d \n",__FILE__,__LINE__);
	opt = uri_hash[uri_hash_cal_value(arg)%MAX_URI_HASH_SIZE].entry;

	while (opt) {
		if ( strcasecmp(opt->name, arg) == 0 )
			return opt;
		opt = opt->next;
	//	fprintf(stderr,"opt->name =%s  %s  :%d\n",opt->name,__FILE__,__LINE__);
	}
	return NULL;
}


/*
 * Name: new_request
 * Description: Obtains a request struct off the free list, or if the
 * free list is empty, allocates memory
 *
 * Return value: pointer to initialized request
 */

request *new_request(void)
{
	request *req;

	if (request_free) {
		req = request_free;     /* first on free list */
		dequeue(&request_free, request_free); /* dequeue the head */
	} else {
		req = (request *) malloc(sizeof (request));
		if (!req) {
			log_error_time();
			perror("malloc for new request");
			return NULL;
		}
	}

	memset(req, 0, offsetof(request, buffer) + 1);
	req->busy_flag = BUSY_FLAG_AUDIO|BUSY_FLAG_VIDEO;


	return req;
}

request *get_sock_request(int server_s)
{
	int fd;                     /* socket */
	struct SOCKADDR remote_addr; /* address */
	struct SOCKADDR salocal;
	socklen_t remote_addrlen = sizeof (struct SOCKADDR);
	request *conn;              /* connection */
	size_t len;
	static int system_bufsize = 0; /* Default size of SNDBUF given by system */

	remote_addr.S_FAMILY = 0xdead;
	fd = accept(server_s, (struct sockaddr *) &remote_addr,
			&remote_addrlen);

	if (fd == -1) {
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			/* abnormal error */
			WARN("accept");
		else
			/* no requests */
			pending_requests = 0;
		return NULL;
	}
	if (fd >= FD_SETSIZE) {
		WARN("Got fd >= FD_SETSIZE.");
		close(fd);
		return NULL;
	}
#ifdef DEBUGNONINET
	/* This shows up due to race conditions in some Linux kernels
	   when the client closes the socket sometime between
	   the select() and accept() syscalls.
	   Code and description by Larry Doolittle <ldoolitt@boa.org>
	 */
#define HEX(x) (((x)>9)?(('a'-10)+(x)):('0'+(x)))
	if (remote_addr.sin_family != AF_INET) {
		struct sockaddr *bogus = (struct sockaddr *) &remote_addr;
		char *ap, ablock[44];
		int i;
		close(fd);
		log_error_time();
		for (ap = ablock, i = 0; i < remote_addrlen && i < 14; i++) {
			*ap++ = ' ';
			*ap++ = HEX((bogus->sa_data[i] >> 4) & 0x0f);
			*ap++ = HEX(bogus->sa_data[i] & 0x0f);
		}
		*ap = '\0';
		fprintf(stderr, "non-INET connection attempt: socket %d, "
				"sa_family = %hu, sa_data[%d] = %s\n",
				fd, bogus->sa_family, remote_addrlen, ablock);
		return NULL;
	}
#endif

	/* XXX Either delete this, or document why it's needed */
	/* Pointed out 3-Oct-1999 by Paul Saab <paul@mu.org> */
#ifdef REUSE_EACH_CLIENT_CONNECTION_SOCKET
	if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
					sizeof (sock_opt))) == -1) {
		DIE("setsockopt: unable to set SO_REUSEADDR");
	}
#endif

	len = sizeof(salocal);

	if (getsockname(fd, (struct sockaddr *) &salocal, &len) != 0) {
		WARN("getsockname");
		close(fd);
		return NULL;
	}

	conn = new_request();
	if (!conn) {
		close(fd);
		return NULL;
	}
	conn->fd = fd;
	conn->status = READ_HEADER;
	conn->header_line = conn->client_stream;
	conn->time_last = current_time;
	conn->kacount = ka_max;

	ascii_sockaddr(&salocal, conn->local_ip_addr, NI_MAXHOST);

	/* nonblocking socket */
	if (set_nonblock_fd(conn->fd) == -1)
		WARN("fcntl: unable to set new socket to non-block");

	/* set close on exec to true */
	if (fcntl(conn->fd, F_SETFD, 1) == -1)
		WARN("fctnl: unable to set close-on-exec for new socket");

	/* Increase buffer size if we have to.
	 * Only ask the system the buffer size on the first request,
	 * and assume all subsequent sockets have the same size.
	 */
	if (system_bufsize == 0) {
		len = sizeof (system_bufsize);
		if (getsockopt
				(conn->fd, SOL_SOCKET, SO_SNDBUF, &system_bufsize, &len) == 0
				&& len == sizeof (system_bufsize)) {
			/*
			   fprintf(stderr, "%sgetsockopt reports SNDBUF %d\n",
			   get_commonlog_time(), system_bufsize);
			 */
			;
		} else {
			WARN("getsockopt(SNDBUF)");
			system_bufsize = 1;
		}
	}
	if (system_bufsize < sockbufsize) {
		if (setsockopt
				(conn->fd, SOL_SOCKET, SO_SNDBUF, (void *) &sockbufsize,
				 sizeof (sockbufsize)) == -1) {
			WARN("setsockopt: unable to set socket buffer size");
#ifdef DIE_ON_ERROR_TUNING_SNDBUF
			exit(errno);
#endif
		}
	}

	/* for log file and possible use by CGI programs */
	ascii_sockaddr(&remote_addr, conn->remote_ip_addr, NI_MAXHOST);

	/* for possible use by CGI programs */
	conn->remote_port = net_port(&remote_addr);

	status.requests++;

#ifdef USE_TCPNODELAY
	/* Thanks to Jef Poskanzer <jef@acme.com> for this tweak */
	{
		int one = 1;
		if (setsockopt(conn->fd, IPPROTO_TCP, TCP_NODELAY,
					(void *) &one, sizeof (one)) == -1) {
			DIE("setsockopt: unable to set TCP_NODELAY");
		}

	}
#endif

	total_connections++;
	enqueue(&request_ready, conn);
	return conn;
}

/*
 * Name: get_request
 *
 * Description: Polls the server socket for a request.  If one exists,
 * does some basic initialization and adds it to the ready queue;.
 */
void get_request(int server_s)
{
	get_sock_request(server_s);
}


/*
 * Name: free_request
 *
 * Description: Deallocates memory for a finished request and closes
 * down socket.
 */

static void free_request(request ** list_head_addr, request * req)
{
	int i;
	int ret=0;
	/* free_request should *never* get called by anything but
	   process_requests */

	if (req->buffer_end && req->status != DEAD) {
		req->status = DONE;
		return;
	}
	/* put request on the free list */
	dequeue(list_head_addr, req); /* dequeue from ready or block list */

	if (req->logline)           /* access log */
		log_access(req);
	else {

		if (req->mmap_entry_var)
			release_mmap(req->mmap_entry_var);
		else if (req->data_mem) {
			if (req->mmap_ptr)
				munmap(req->mmap_ptr, req->mmap_size);
			else

				munmap(req->data_mem, req->filesize);
		}

	}


	if (req->data_fd)
	{
		extern  int posix_fadvise(int fd, off_t  offset, off_t len, int advice);
		fdatasync(req->data_fd);
		posix_fadvise(req->data_fd, 0,0, POSIX_FADV_DONTNEED );
		close(req->data_fd);
	}

	if (req->post_data_fd)
		close(req->post_data_fd);

	if (req->response_status >= 400)
		status.errors++;

	for (i = COMMON_CGI_COUNT; i < req->cgi_env_index; ++i) {
		if (req->cgi_env[i]) {
			free(req->cgi_env[i]);
		} else {
			log_error_time();
			fprintf(stderr, "Warning: CGI Environment contains NULL value" \
					"(index %d of %d).\n", i, req->cgi_env_index);
		}
	}

	if (req->cmd_string)
		free(req->cmd_string);
	if (req->mem_flag & MFLAG_IS_MEMORY) {
		if (req->data_mem)
			free(req->data_mem);
	}

	if (req->pathname)
		free(req->pathname);
	if (req->path_info)
		free(req->path_info);
	if (req->path_translated)
		free(req->path_translated);
	if (req->script_name)
		free(req->script_name);

	if ((req->keepalive == KA_ACTIVE) &&
			(req->response_status < 500) && req->kacount > 0) {
		int bytes_to_move;

		request *conn = new_request();
		if (!conn) {
			/* errors already reported */
			enqueue(&request_free, req);
			close(req->fd);
			total_connections--;
			return;
		}
		conn->fd = req->fd;
		conn->status = READ_HEADER;
		conn->header_line = conn->client_stream;
		conn->kacount = req->kacount - 1;

		/* close enough and we avoid a call to time(NULL) */
		conn->time_last = req->time_last;

		/* for log file and possible use by CGI programs */
		memcpy(conn->remote_ip_addr, req->remote_ip_addr, NI_MAXHOST);
		memcpy(conn->local_ip_addr, req->local_ip_addr, NI_MAXHOST);

		/* for possible use by CGI programs */
		conn->remote_port = req->remote_port;

		status.requests++;

		/* we haven't parsed beyond req->parse_pos, so... */
		bytes_to_move = req->client_stream_pos - req->parse_pos;

		if (bytes_to_move) {
			memcpy(conn->client_stream,
					req->client_stream + req->parse_pos, bytes_to_move);
			conn->client_stream_pos = bytes_to_move;
		}
		enqueue(&request_block, conn);
		BOA_FD_SET(conn->fd, &block_read_fdset);

		enqueue(&request_free, req);
		return;
	}

	/*
	   While debugging some weird errors, Jon Nelson learned that
	   some versions of Netscape Navigator break the
	   HTTP specification.

	   Some research on the issue brought up:

http://www.apache.org/docs/misc/known_client_problems.html

As quoted here:

"
Trailing CRLF on POSTs

This is a legacy issue. The CERN webserver required POST
data to have an extra CRLF following it. Thus many
clients send an extra CRLF that is not included in the
Content-Length of the request. Apache works around this
problem by eating any empty lines which appear before a
request.
"

Boa will (for now) hack around this stupid bug in Netscape
(and Internet Exploder)
by reading up to 32k after the connection is all but closed.
This should eliminate any remaining spurious crlf sent
by the client.

Building bugs *into* software to be compatable is
just plain wrong
	 */

	if (req->method == M_POST) {
		char buf[32768];
		ret=read(req->fd, buf, 32768);
		if(ret<0)
		{
			fprintf(stderr,"read fail %s:%d\n",__FILE__,__LINE__);
		}
	}
	close(req->fd);
	total_connections--;

	enqueue(&request_free, req);

	return;
}

/*
 * Name: process_requests
 *
 * Description: Iterates through the ready queue, passing each request
 * to the appropriate handler for processing.  It monitors the
 * return value from handler functions, all of which return -1
 * to indicate a block, 0 on completion and 1 to remain on the
 * ready list for more procesing.
 */

void process_requests(int server_s)
{
	int retval = 0;
	request *current, *trailer;
	node_t *node = NULL;
	int busy;
	time_t current_time;



	if (pending_requests) {
		get_request(server_s);
	}


	current = request_ready;
	busy = 0;
	while (current) {
		time(&current_time);
		if (current->buffer_end && /* there is data in the buffer */
				current->status != DEAD && current->status != DONE) {
			retval = req_flush(current);
			/*
			 * retval can be -2=error, -1=blocked, or bytes left
			 */
			if (retval == -2) { /* error */
				current->status = DEAD;
				retval = 0;
			} else if (retval >= 0) {
				/* notice the >= which is different from below?
				   Here, we may just be flushing headers.
				   We don't want to return 0 because we are not DONE
				   or DEAD */

				retval = 1;
			}
		} else {
			//fprintf(stderr,"8888888888888888888  current->status=%d\n",current->status);
			switch (current->status) {
				case READ_HEADER:
				case ONE_CR:
				case ONE_LF:
				case TWO_CR:
					retval = read_header(current,node);
					if(retval == RET_UPGRADE){
						//UpgradeHandle(current->fd); //<>
						processUpgrade( current->fd );
						current->status = DEAD;
					}
					break;
				case BODY_READ:
					retval = read_body(current);
					break;
				case BODY_WRITE:
					retval = write_body(current);
					break;
				case WRITE:
					//fprintf(stderr,"9999999999999999999999  current->status=%d\n",current->status);
					retval = process_get(current);
					break;
				case PIPE_READ:
					retval = read_from_pipe(current);
					break;
				case PIPE_WRITE:
					retval = write_from_pipe(current);
					break;
				case DONE:
					/* a non-status that will terminate the request */
					retval = req_flush(current);
					/*
					 * retval can be -2=error, -1=blocked, or bytes left
					 */
					if (retval == -2) { /* error */
						current->status = DEAD;
						retval = 0;
					} else if (retval > 0) {
						retval = 1;
					}
					break;
				case DEAD:
					retval = 0;
					current->buffer_end = 0;
					SQUASH_KA(current);
					break;
				default:
					retval = 0;
					fprintf(stderr, "Unknown status (%d), "
							"closing!\n", current->status);
					current->status = DEAD;
					break;
			}

		}

		if (sigterm_flag)
			SQUASH_KA(current);

		/* we put this here instead of after the switch so that
		 * if we are on the last request, and get_request is successful,
		 * current->next is valid!
		 */
		if (pending_requests)
			get_request(server_s);

		busy |= current->busy_flag;
		//current->busy_flag = BUSY_FLAG_AUDIO|BUSY_FLAG_VIDEO;


		switch (retval) {
			case -1:               /* request blocked */
				trailer = current;
				current = current->next;
				block_request(trailer);
				break;
			case 0:                /* request complete */
				current->time_last = current_time;
				trailer = current;
				current = current->next;
				free_request(&request_ready, trailer);
				break;
			case 1:                /* more to do */
				current->time_last = current_time;
				current = current->next;
				break;
			default:
				log_error_time();
				fprintf(stderr, "Unknown retval in process requests - "
						"Status: %d, retval: %d\n", current->status, retval);
				current = current->next;
				break;
		}
	}
	if (!busy)
		usleep(10000);

}

/*
 * Name: process_logline
 *
 * Description: This is called with the first req->header_line received
 * by a request, called "logline" because it is logged to a file.
 * It is parsed to determine request type and method, then passed to
 * translate_uri for further parsing.  Also sets up CGI environment if
 * needed.
 */

int process_logline(request * req)
{
	char *stop, *stop2;
	static char *SIMPLE_HTTP_VERSION = "HTTP/0.9";

	req->logline = req->client_stream;
	if (!memcmp(req->logline, "GET ", 4))
		req->method = M_GET;
	else if (!memcmp(req->logline, "HEAD ", 5))
		/* head is just get w/no body */
		req->method = M_HEAD;
	else if (!memcmp(req->logline, "PUT ", 4))
		req->method = M_PUT;
	else if (!memcmp(req->logline, "POST ", 5))
		req->method = M_POST;
	else if (!memcmp(req->logline, "DELETE ", 7))
		req->method = M_DELETE;
	else {
		log_error_time();
		fprintf(stderr, "malformed request: \"%s\"\n", req->logline);
		send_r_not_implemented(req);
		return 0;
	}

	req->http_version = SIMPLE_HTTP_VERSION;
	req->simple = 1;

	/* Guaranteed to find ' ' since we matched a method above */
	if(req->method != M_DELETE)
		stop = req->logline + 3;
	else
		stop = req->logline + 5;

	if (*stop != ' ')
		++stop;

	/* scan to start of non-whitespace */
	while (*(++stop) == ' ');

	stop2 = stop;

	/* scan to end of non-whitespace */
	while (*stop2 != '\0' && *stop2 != ' ')
		++stop2;

	if (stop2 - stop > MAX_HEADER_LENGTH) {
		log_error_time();
		fprintf(stderr, "URI too long %d: \"%s\"\n", MAX_HEADER_LENGTH,
				req->logline);
		send_r_bad_request(req);
		return 0;
	}
	memcpy(req->request_uri, stop, stop2 - stop);
	req->request_uri[stop2 - stop] = '\0';

	if (*stop2 == ' ') {
		/* if found, we should get an HTTP/x.x */
		unsigned int p1, p2;

		/* scan to end of whitespace */
		++stop2;
		while (*stop2 == ' ' && *stop2 != '\0')
			++stop2;

		/* scan in HTTP/major.minor */
		if (sscanf(stop2, "HTTP/%u.%u", &p1, &p2) == 2 ) {
			/* HTTP/{0.9,1.0,1.1} */
			if (p1 == 1 && (p2 == 0 || p2 == 1)) {
				req->http_version = stop2;
				req->simple = 0;
			} else if (p1 > 1 || (p1 != 0 && p2 > 1)) {
				goto BAD_VERSION;
			}
		} else {
			goto BAD_VERSION;
		}
	}

	if (req->method == M_HEAD && req->simple) {
		send_r_bad_request(req);
		return 0;
	}
	req->cgi_env_index = COMMON_CGI_COUNT;

	return 1;

BAD_VERSION:
	log_error_time();
	fprintf(stderr, "bogus HTTP version: \"%s\"\n", stop2);
	send_r_bad_request(req);
	return 0;
}

/*
 * Name: process_header_end
 *
 * Description: takes a request and performs some final checking before
 * init_cgi or init_get
 * Returns 0 for error or NPH, or 1 for success
 */

int process_header_end(request * req,node_t * node)
{
	//fprintf(stderr,",req->client_stream=%s  %s :%d \n",req->client_stream,__FILE__,__LINE__);
	if (!req->logline) {
		fprintf(stderr," file %s line %d\n",__FILE__,__LINE__);
		send_r_error(req);
		return 0;
	}

	req->query_string="NONE"; //<>
	/* Percent-decode request */
	if (unescape_uri(req->request_uri, &(req->query_string)) == 0) {
		log_error_doc(req);
		fputs("Problem unescaping uri\n", stderr);
		send_r_bad_request(req);
		return 0;
	}

	/* clean pathname */
	clean_pathname(req->request_uri);

	if (req->request_uri[0] != '/') {
		send_r_bad_request(req);
		return 0;
	}

	if (translate_uri(req) == 0) { /* unescape, parse uri */
		SQUASH_KA(req);
		return 0;               /* failure, close down */
	}

#if   1    //xg  authority
	req->authority = AUTHORITY_NONE;
	//fprintf(stderr," %s  %s :%d\n",req->http_uri->name,__FILE__,__LINE__);
	if (req->http_uri) {
		if((strcmp(req->request_uri,"/index.html")!= 0) && (strcmp(req->request_uri,"/index.htm")!= 0) && (strcmp(req->request_uri,"/")!= 0) && (strcmp(req->request_uri,"/gIPNC-ro.gz")!= 0) && (strcmp(req->request_uri,"/IPNCWebDlls.CAB")!= 0) &&
				(strcmp(req->request_uri,"/sddel.htm")!= 0) && (strcmp(req->request_uri,"/sdget.htm")!= 0) && (strcmp(req->request_uri,"/sysget.htm")!= 0)
				&& (strcmp(req->request_uri,"/accessget.htm")!= 0) && (strcmp(req->request_uri,"/actvlistget.htm")!= 0) && (strcmp(req->request_uri,"/paraget.htm")!= 0)
				&& (strcmp(req->request_uri,"/paradefault.htm")!= 0)&& (strcmp(req->request_uri,"/actvlistclear.htm")!= 0) && (strcmp(req->request_uri,"/update.cgi")!= 0)
				&& (strcmp(req->query_string,"getweblanguage")!= 0)
		  ){
			if (req->http_uri->authority < req->authority) {
#if 1
				if (!auth_authorize(req)) {
					boa_dbg("auth_authorize request: %s\n", req->request_uri);
					return 0;
				}
#endif
				if (req->http_uri->authority < req->authority) {
					send_r_forbidden(req);
					return 0;
				}
			}
		}

		if (req->http_uri->handler) {
			int ret = (*req->http_uri->handler)(req);
			if (ret >= 0)
				return ret;
		}
	}
#endif


	if (req->method == M_POST) {
		req->post_data_fd = create_temporary_file(1, NULL, 0);
		if (req->post_data_fd == 0)
			return(0);
		return(1); /* success */
	}

	if (req->is_cgi) {
		return init_cgi(req);
	}

	req->status = WRITE;
	return init_get(req);       /* get and head */
}

/*
 * Name: process_option_line
 *
 * Description: Parses the contents of req->header_line and takes
 * appropriate action.
 */

int process_option_line(request * req)
{
	char c, *value, *line = req->header_line;

	/* Start by aggressively hacking the in-place copy of the header line */

	value = strchr(line, ':');
	if (value == NULL)
		return 0;
	*value++ = '\0';            /* overwrite the : */
	to_upper(line);             /* header types are case-insensitive */
	while ((c = *value) && (c == ' ' || c == '\t'))
		value++;

	if (!memcmp(line, "IF_MODIFIED_SINCE", 18) && !req->if_modified_since)
		req->if_modified_since = value;

	else if (!memcmp(line, "CONTENT_TYPE", 13) && !req->content_type){
		req->content_type = value;
		//fprintf(stderr,"\r\n Content Type: %s\n",req->content_type);
	}

	else if (!memcmp(line, "CONTENT_LENGTH", 15) && !req->content_length){
		req->content_length = value;
		//fprintf(stderr,"\r\n Content Length: %s\n",req->content_length);
	}

	else if (!memcmp(line,"AUTHORIZATION",14) && !req->authorization)
		req->authorization = value;


	else if (!memcmp(line, "CONNECTION", 11) &&
			ka_max && req->keepalive != KA_STOPPED) {
		req->keepalive = (!strncasecmp(value, "Keep-Alive", 10) ?
				KA_ACTIVE : KA_STOPPED);
	}
	/* #ifdef ACCEPT_ON */
	else if (!memcmp(line, "ACCEPT", 7))
		add_accept_header(req, value);
	/* #endif */

	/* Need agent and referer for logs */
	else if (!memcmp(line, "REFERER", 8)) {
		req->header_referer = value;
		if (!add_cgi_env(req, "REFERER", value, 1))
			return 0;
	} else if (!memcmp(line, "USER_AGENT", 11)) {
		req->header_user_agent = value;
		if (!add_cgi_env(req, "USER_AGENT", value, 1))
			return 0;
	} else {
		if (!add_cgi_env(req, line, value, 1))
			return 0;
	}
	return 1;
}

/*
 * Name: add_accept_header
 * Description: Adds a mime_type to a requests accept char buffer
 *   silently ignore any that don't fit -
 *   shouldn't happen because of relative buffer sizes
 */

void add_accept_header(request * req, char *mime_type)
{
#ifdef ACCEPT_ON
	int l = strlen(req->accept);
	int l2 = strlen(mime_type);

	if ((l + l2 + 2) >= MAX_HEADER_LENGTH)
		return;

	if (req->accept[0] == '\0')
		strcpy(req->accept, mime_type);
	else {
		req->accept[l] = ',';
		req->accept[l + 1] = ' ';
		memcpy(req->accept + l + 2, mime_type, l2 + 1);
		/* the +1 is for the '\0' */
		/*
		   sprintf(req->accept + l, ", %s", mime_type);
		 */
	}
#endif
}

void free_requests(void)
{
	request *ptr, *next;

	ptr = request_free;
	while (ptr != NULL) {
		next = ptr->next;
		free(ptr);
		ptr = next;
	}
	request_free = NULL;
}
