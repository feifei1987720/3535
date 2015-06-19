#include "boa.h"
void print_content_type(request * req);
void print_content_length(request * req);
void print_last_modified(request * req);
void print_last_modified(request * req);
void print_ka_phrase(request * req);
void print_http_headers(request * req);
void send_r_request_ok(request * req);
void send_request_ok_no_cache(request * req);
void send_request_ok_sdget(request * req);
void send_request_ok_api(request * req);
void send_request_ok_jpeg(request * req);
void print_mjpeg_headers(request * req);
void write_time_string(char *buf);
void print_mpeg4_headers(request * req);
void print_avc_headers(request * req);
void print_audio_headers(request * req);
void send_request_ok_mjpeg(request * req);
void send_request_ok_mpeg4(request * req);
void send_request_ok_avc(request * req);
void send_r_moved_perm(request * req, char *url);
void send_r_moved_temp(request * req, char *url, char *more_hdr);
void send_r_not_modified(request * req);
void send_r_bad_request(request * req);
void send_r_unauthorized(request * req, char *realm_name);
void send_r_forbidden(request * req);
void send_r_not_found(request * req);
void send_r_error(request * req);
void send_r_not_implemented(request * req);
void send_r_bad_gateway(request * req);
void send_r_service_unavailable(request * req);
void send_r_bad_version(request * req);
void send_r_psia_get(request * req);
void send_r_psia_not_implemented(request * req);
void send_r_psia_datetime(request * req,char *local_time);
void send_r_psia_timezone(request * req,char *timezone);
void  end_r_psia_not_implemented(request * req);
void send_r_psia_ok_mjpeg(request * req);
void send_r_psia_ok_mpeg4(request * req);
void send_r_psia_supportReport(request * req);
void send_r_psia_ok_avc(request * req);
	



