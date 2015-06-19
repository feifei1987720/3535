// sps.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <math.h>
#include "codeLib.h"
#include "commonLib.h"
#include "spsParse.h"

static UINT Ue(BYTE *pBuff, UINT nLen, UINT *nStartBit)
{	
	//0bit	
	UINT nZeroNum = 0;	
	UINT startBit = *nStartBit;
	while (startBit < nLen * 8)	
	{		
		if (pBuff[startBit / 8] & (0x80 >> (startBit % 8))) //&:%		
		{			break;		
		}	
		nZeroNum++;	
		startBit++;
	}
	
	startBit++;
	
	//	
	UINT dwRet = 0;	
	UINT i = 0;
	for (i = 0; i < nZeroNum; i++)
	{	
		dwRet <<= 1;		
		if (pBuff[startBit / 8] & (0x80 >> (startBit % 8))) {			
			dwRet += 1;		
		}
	
		startBit++;
	}	
	
	*nStartBit = startBit;
	
	return (1 << nZeroNum) - 1 + dwRet;
}

static int Se(BYTE *pBuff, UINT nLen, UINT *nStartBit)
{
	int UeVal=Ue(pBuff,nLen, nStartBit);
	double k=UeVal;	
	int nValue = ceil(k/2); //ceilceilceil(2)=ceil(1.2)=cei(1.5)=2.00	
	if (UeVal % 2 == 0 )	
		nValue=-nValue;	
	return nValue;
}
					
static UINT u(UINT BitCount,BYTE * buf,UINT *nStartBit)
{
	UINT dwRet = 0;	
	
	UINT startBit = *nStartBit;
	UINT i = 0;
	for (i = 0; i < BitCount; i++)
	{		
		dwRet <<= 1;		
		if (buf[startBit / 8] & (0x80 >> (startBit % 8))) {			
			dwRet += 1;	
		}			
		startBit++;
	}	
	
	*nStartBit = startBit;
	
	return dwRet;
}
#if 1 //ssf
typedef struct h264_sps_s
{
	int profile_idc;			
	int constraint_set0_flag;//(buf[1] & 0x80)>>7;	
	int constraint_set1_flag;//(buf[1] & 0x40)>>6;
	int constraint_set2_flag;//(buf[1] & 0x20)>>5;	
	int constraint_set3_flag;//(buf[1] & 0x10)>>4;	
	int reserved_zero_4bits;	
	int level_idc;	
	int seq_parameter_set_id;
	int chroma_format_idc;			
	int residual_colour_transform_flag;		
	int bit_depth_luma_minus8;			
	int bit_depth_chroma_minus8;		
	int qpprime_y_zero_transform_bypass_flag;	
	int seq_scaling_matrix_present_flag;	
	int seq_scaling_list_present_flag[8];	
	int delta_scale;
	int log2_max_frame_num_minus4;
	int pic_order_cnt_type;			
	int log2_max_pic_order_cnt_lsb_minus4;				
	int delta_pic_order_always_zero_flag;		
	int offset_for_non_ref_pic;	
	int offset_for_top_to_bottom_field;	
	int num_ref_frames_in_pic_order_cnt_cycle;	
	int num_ref_frames;		
	int gaps_in_frame_num_value_allowed_flag;		
	int pic_width_in_mbs_minus1;	
	int pic_height_in_map_units_minus1;	
	int frame_mbs_only_flag;
	int mb_adaptive_frame_field_flag;
	int direct_8x8_inference_flag;
	int frame_cropping_flag;
	int frame_crop_left_offset;
	int frame_crop_right_offset;
	int frame_crop_top_offset;
	int frame_crop_bottom_offset;
	int vui_parameters_present_flag;
	int aspect_ratio_info_present_flag;		
	int aspect_ratio_idc;
	int sar_width;
	int sar_height;
	int overscan_info_present_flag;
	int overscan_appropriate_flag;				
	int video_signal_type_present_flag;
	int video_format;
	int video_full_range_flag;
	int colour_description_present_flag;
	int colour_primarie;
	int transfer_characteristics;
	int matrix_coefficients;
	int chroma_loc_info_present_flag;
	int chroma_sample_loc_type_top_field;				
	int chroma_sample_loc_type_bottom_field;
	int timing_info_present_flag;
	unsigned int num_units_in_tick;
	unsigned int time_scale;
	int fixed_frame_rate_flag;
}H264_SPS_S;

static int h264_parse_sps(BYTE * buf, unsigned int nLen, H264_SPS_S *sps)
{	
	UINT StartBit = 0;
	int forbidden_zero_bit;
	int nal_ref_idc;
	int nal_unit_type;
	int i = 0;
	int j = 0;
	int sizeOfScalingList;       
	int lastScale;       
	int nextScale; 
	
	forbidden_zero_bit = u(1, buf, &StartBit);
	
	nal_ref_idc = u(2, buf, &StartBit);	
	
	nal_unit_type = u(5, buf, &StartBit);	

	if (nal_unit_type != 7 ) {
		return -1;
	}
	sps->profile_idc = u(8, buf, &StartBit);	

	sps->constraint_set0_flag = u(1, buf, &StartBit);//(buf[1] & 0x80)>>7;	

	sps->constraint_set1_flag = u(1, buf, &StartBit);//(buf[1] & 0x40)>>6;

	sps->constraint_set2_flag = u(1, buf, &StartBit);//(buf[1] & 0x20)>>5;	

	sps->constraint_set3_flag = u(1, buf, &StartBit);//(buf[1] & 0x10)>>4;	

	sps->reserved_zero_4bits = u(4, buf, &StartBit);	

	sps->level_idc = u(8, buf, &StartBit);	

	sps->seq_parameter_set_id = Ue(buf, nLen, &StartBit);				

	if ( sps->profile_idc == 100 || sps->profile_idc == 110 || sps->profile_idc == 122 || sps->profile_idc == 144 )	
	{			
		sps->chroma_format_idc = Ue(buf, nLen, &StartBit);			

		if( sps->chroma_format_idc == 3 ) {					
			sps->residual_colour_transform_flag = u(1, buf, &StartBit);		
		}						

		sps->bit_depth_luma_minus8 = Ue(buf, nLen, &StartBit);			

		sps->bit_depth_chroma_minus8 = Ue(buf, nLen, &StartBit);		

		sps->qpprime_y_zero_transform_bypass_flag = u(1, buf, &StartBit);	

		sps->seq_scaling_matrix_present_flag = u(1, buf, &StartBit);	

		sps->seq_scaling_list_present_flag[8];		

		if ( sps->seq_scaling_matrix_present_flag )			
		{						
			for ( i = 0; i < 8; i++ ) {	
				sps->seq_scaling_list_present_flag[i] = u(1, buf, &StartBit);		

				if ( sps->seq_scaling_list_present_flag[i] ) {

					sizeOfScalingList = (i < 6) ? 16 : 64;       
					lastScale = 8;       
					nextScale = 8; 
					for ( j = 0; j < sizeOfScalingList; j++) {    
						if (nextScale != 0) {          
							sps->delta_scale = Se(buf, nLen, &StartBit);
							nextScale = (lastScale + sps->delta_scale + 256) % 256;    
						} 
			
						lastScale = (nextScale == 0) ? lastScale : nextScale;        
					}         
				}
			}
		}		

	}	
		
	sps->log2_max_frame_num_minus4 = Ue(buf, nLen, &StartBit);

	sps->pic_order_cnt_type = Ue(buf, nLen, &StartBit);			

	if ( sps->pic_order_cnt_type == 0 ) {					

		sps->log2_max_pic_order_cnt_lsb_minus4 = Ue(buf, nLen, &StartBit);				

	} else if ( sps->pic_order_cnt_type == 1 ) {		

		sps->delta_pic_order_always_zero_flag = u(1, buf, &StartBit);		

		sps->offset_for_non_ref_pic = Se(buf, nLen, &StartBit);	

		sps->offset_for_top_to_bottom_field = Se(buf, nLen, &StartBit);	

		sps->num_ref_frames_in_pic_order_cnt_cycle = Ue(buf, nLen, &StartBit);	

		for( i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++ )			
			Se(buf, nLen, &StartBit);						

	}		 

	sps->num_ref_frames = Ue(buf, nLen, &StartBit);		

	sps->gaps_in_frame_num_value_allowed_flag = u(1, buf, &StartBit);		
	
	sps->pic_width_in_mbs_minus1 = Ue(buf, nLen, &StartBit);	
	
	sps->pic_height_in_map_units_minus1 = Ue(buf, nLen, &StartBit);	
	
	sps->frame_mbs_only_flag = u(1, buf, &StartBit);

	if ( !sps->frame_mbs_only_flag) {
		sps->mb_adaptive_frame_field_flag = u(1, buf, &StartBit);
	}	

	sps->direct_8x8_inference_flag = u(1, buf, &StartBit);

	sps->frame_cropping_flag = u(1, buf, &StartBit);

	if ( sps->frame_cropping_flag ) {
		sps->frame_crop_left_offset = Ue(buf, nLen, &StartBit);
		sps->frame_crop_right_offset = Ue(buf, nLen, &StartBit);
		sps->frame_crop_top_offset = Ue(buf, nLen, &StartBit);
		sps->frame_crop_bottom_offset = Ue(buf, nLen, &StartBit);
	}

	sps->vui_parameters_present_flag = u(1, buf, &StartBit);

	if ( sps->vui_parameters_present_flag ) {	
				
		sps->aspect_ratio_info_present_flag = u(1, buf, &StartBit);		
				
		if ( sps->aspect_ratio_info_present_flag ) { 
				
			sps->aspect_ratio_idc = u(8, buf, &StartBit);
				
			if ( sps->aspect_ratio_idc ) {
					
				sps->sar_width = u(16, buf, &StartBit);
					
				sps->sar_height = u(16, buf, &StartBit);
					
			}
		}

		sps->overscan_info_present_flag =  u(1, buf, &StartBit);
			
		if ( sps->overscan_info_present_flag ) {				
			sps->overscan_appropriate_flag = u(1, buf, &StartBit);				
		}

		sps->video_signal_type_present_flag = u(1, buf, &StartBit);

		if ( sps->video_signal_type_present_flag ) { 
				
			sps->video_format = u(3, buf, &StartBit);
				
			sps->video_full_range_flag =  u(1, buf, &StartBit);
				
			sps->colour_description_present_flag = u(1, buf, &StartBit);
				

			if ( sps->colour_description_present_flag ) {

				sps->colour_primarie =	u(8, buf, &StartBit);
					
				sps->transfer_characteristics = u(8, buf, &StartBit);
					
				sps->matrix_coefficients = u(8, buf, &StartBit);
					
			}				
		}

		sps->chroma_loc_info_present_flag = u(1, buf, &StartBit);
			
		if ( sps->chroma_loc_info_present_flag ) { 				
			sps->chroma_sample_loc_type_top_field = Ue(buf, nLen, &StartBit);				
			sps->chroma_sample_loc_type_bottom_field = Ue(buf, nLen, &StartBit);
				
		}

		sps->timing_info_present_flag =  u(1, buf, &StartBit);

		if ( sps->timing_info_present_flag == 1 ) { 
				
			sps->num_units_in_tick =  u(32, buf, &StartBit);
			
			sps->time_scale = u(32, buf, &StartBit);
			
			sps->fixed_frame_rate_flag = u(1, buf, &StartBit);
		}
	}

	return 1;					

}


int h264_sps(const char *pBase64Sps, int *width, int *height, int *framerate)
{
	H264_SPS_S sps = {0};
	unsigned int inLen = 250;
	
	if((pBase64Sps[4] & 0x1F) == 0x7)//sps
	{
		h264_parse_sps(pBase64Sps+4, inLen, &sps);
		*width = (sps.pic_width_in_mbs_minus1+1)*16;
		*height = (sps.pic_height_in_map_units_minus1+1)*16;
		if(sps.timing_info_present_flag)
		{
			*framerate = sps.time_scale/(2*sps.num_units_in_tick);
		}
		//printf("############ sps width = %d , height = %d\n", *width, *height);
	}
	return 0;
}
#endif
static int h264_decode_sps(BYTE * buf, unsigned int nLen, int *width, int *height)
{	
	UINT StartBit = 0; 
	int forbidden_zero_bit = u(1, buf, &StartBit);
	int nal_ref_idc = u(2, buf, &StartBit);	
	int nal_unit_type = u(5, buf, &StartBit);		

	int i = 0;	
//	printf("\n\n===========buf %02x \n", buf[0]);	

	if (nal_unit_type != 7 ) {
		return -1;
	}
		
	int profile_idc = u(8, buf, &StartBit);			
	//printf("\n\n=profile_idc %d \n", profile_idc);	

	int constraint_set0_flag = u(1, buf, &StartBit);//(buf[1] & 0x80)>>7;	

	int constraint_set1_flag = u(1, buf, &StartBit);//(buf[1] & 0x40)>>6;

	int constraint_set2_flag = u(1, buf, &StartBit);//(buf[1] & 0x20)>>5;	

	int constraint_set3_flag = u(1, buf, &StartBit);//(buf[1] & 0x10)>>4;	

	int reserved_zero_4bits = u(4, buf, &StartBit);	

	int level_idc = u(8, buf, &StartBit);	

	int seq_parameter_set_id = Ue(buf, nLen, &StartBit);				

	if ( profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 144 )	
	{			

		int chroma_format_idc = Ue(buf, nLen, &StartBit);			

		if( chroma_format_idc == 3 ) {					
			int residual_colour_transform_flag = u(1, buf, &StartBit);		
		}						

		int bit_depth_luma_minus8 = Ue(buf, nLen, &StartBit);			

		int bit_depth_chroma_minus8 = Ue(buf, nLen, &StartBit);		

		int qpprime_y_zero_transform_bypass_flag = u(1, buf, &StartBit);	

		int seq_scaling_matrix_present_flag = u(1, buf, &StartBit);	

		int seq_scaling_list_present_flag[8];		

		if ( seq_scaling_matrix_present_flag )			
		{						

			for ( i = 0; i < 8; i++ ) {	
			
				seq_scaling_list_present_flag[i] = u(1, buf, &StartBit);		

				if ( seq_scaling_list_present_flag[i] ) {

					int sizeOfScalingList = (i < 6) ? 16 : 64;       
					int lastScale = 8;       
					int nextScale = 8; 
					int j = 0;
					for ( j = 0; j < sizeOfScalingList; j++) {    
						if (nextScale != 0) {          
							int delta_scale = Se(buf, nLen, &StartBit);
							nextScale = (lastScale + delta_scale + 256) % 256;    
						} 
			
						lastScale = (nextScale == 0) ? lastScale : nextScale;        
					}         
				}
			}
		}		

	}	
		
	int log2_max_frame_num_minus4 = Ue(buf, nLen, &StartBit);

	int pic_order_cnt_type = Ue(buf, nLen, &StartBit);			

	if ( pic_order_cnt_type == 0 ) {					

		int log2_max_pic_order_cnt_lsb_minus4 = Ue(buf, nLen, &StartBit);				

	} else if ( pic_order_cnt_type == 1 ) {		

		int delta_pic_order_always_zero_flag = u(1, buf, &StartBit);		

		int offset_for_non_ref_pic = Se(buf, nLen, &StartBit);	

		int offset_for_top_to_bottom_field = Se(buf, nLen, &StartBit);	

		int num_ref_frames_in_pic_order_cnt_cycle = Ue(buf, nLen, &StartBit);	

		for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )			
			Se(buf, nLen, &StartBit);						

	}		 

	int num_ref_frames = Ue(buf, nLen, &StartBit);		

	int gaps_in_frame_num_value_allowed_flag = u(1, buf, &StartBit);		

	int pic_width_in_mbs_minus1 = Ue(buf, nLen, &StartBit);	

	int pic_height_in_map_units_minus1 = Ue(buf, nLen, &StartBit);	

	*width = (pic_width_in_mbs_minus1+1)*16;	

	*height = (pic_height_in_map_units_minus1+1)*16;			

	//printf(" \n width = %d height = %d \n", *width, *height);

	int frame_mbs_only_flag = u(1, buf, &StartBit);

	if ( !frame_mbs_only_flag) {
		int mb_adaptive_frame_field_flag = u(1, buf, &StartBit);
	}	

	int direct_8x8_inference_flag = u(1, buf, &StartBit);

	int frame_cropping_flag = u(1, buf, &StartBit);

	if ( frame_cropping_flag ) {

		int frame_crop_left_offset = Ue(buf, nLen, &StartBit);
		int frame_crop_right_offset = Ue(buf, nLen, &StartBit);
		int frame_crop_top_offset = Ue(buf, nLen, &StartBit);
		int frame_crop_bottom_offset = Ue(buf, nLen, &StartBit);
	}

	int vui_parameters_present_flag = u(1, buf, &StartBit);

	if ( vui_parameters_present_flag ) {	
				
		int aspect_ratio_info_present_flag = u(1, buf, &StartBit);		
				
		if ( aspect_ratio_info_present_flag ) { 
				
			int aspect_ratio_idc = u(8, buf, &StartBit);
				
			if ( aspect_ratio_idc ) {
					
				int sar_width = u(16, buf, &StartBit);
					
				int sar_height = u(16, buf, &StartBit);
					
			}
		}

		int overscan_info_present_flag =  u(1, buf, &StartBit);
			
		if ( overscan_info_present_flag ) {				
			int overscan_appropriate_flag = u(1, buf, &StartBit);				
		}

		int video_signal_type_present_flag = u(1, buf, &StartBit);

		if ( video_signal_type_present_flag ) { 
				
			int video_format = u(3, buf, &StartBit);
				
			int video_full_range_flag =  u(1, buf, &StartBit);
				
			int colour_description_present_flag = u(1, buf, &StartBit);
				

			if ( colour_description_present_flag ) {

				int colour_primarie =	u(8, buf, &StartBit);
					
				int transfer_characteristics = u(8, buf, &StartBit);
					
				int matrix_coefficients = u(8, buf, &StartBit);
					
			}				
		}

		int chroma_loc_info_present_flag = u(1, buf, &StartBit);
			
		if ( chroma_loc_info_present_flag ) { 				
			int chroma_sample_loc_type_top_field = Ue(buf, nLen, &StartBit);				
			int chroma_sample_loc_type_bottom_field = Ue(buf, nLen, &StartBit);
				
		}

		int timing_info_present_flag =  u(1, buf, &StartBit);

		if ( timing_info_present_flag ) { 
				
			unsigned int num_units_in_tick =  u(32, buf, &StartBit);
			
			unsigned int time_scale = u(32, buf, &StartBit);
			
			int fixed_frame_rate_flag = u(1, buf, &StartBit);

			if (  num_units_in_tick ) {					
				int framerate = 0.5 * time_scale/num_units_in_tick;
//				Printf("============ framerate =%d time_scale %d num_units_in_tick %d\n", framerate, time_scale, num_units_in_tick);
			}
		}
	}

	return 1;					

}


int procSPS(const char *pBase64Sps, unsigned int inLen, int *width, int *height)
{
	char sps[256] = { 0 };
	int outLen = 255;

//	int ret = base64decode(pBase64Sps, sps, outLen );
	int ret = codeBase64Dec(sps, pBase64Sps, inLen );

	if ( ret > 0 ) {
		return h264_decode_sps(sps, ret, width, height);
	}

	return -1;
}


#if 0

FILE* pFile = NULL;

char FrameBuff[1024*1024] = { 0 };

int NAL_Search(int offsetNow)
{
	unsigned long testflg = 0;
	int IsFail = 0;

	for(;;)
	{
		fseek(pFile, offsetNow, SEEK_SET);
		if( fread(&testflg, sizeof(testflg), 1, pFile) <=  0 )
		{
			IsFail = -1;
			break;
		}

		//printf("testflg=0x%x \n",(int)testflg );

		if( testflg == 0x01000000 )
		{
			break;
		}


		offsetNow++;

	}
	if( IsFail != 0 )
		return IsFail;

	return offsetNow;

}

void *GetFileFrame(int *pSize,int IsReset)
{
	static int offset = 0;
	int offset1 = 0;
	int offset2 = 0;
	int framesize = 0;

	*pSize = 0;

	if( pFile == NULL )
		return NULL;

	if( IsReset == 1 )
	{
		offset = 0;
		fseek(pFile, 0, SEEK_SET);
	}

	if( pFile )
	{
		fseek(pFile, offset, SEEK_SET);

		offset1 = NAL_Search(offset);
		offset2 = NAL_Search(offset1+4);


		framesize = offset2 - offset1;

		/*reset position*/
		fseek(pFile, offset1, SEEK_SET);
		fread(FrameBuff, framesize, 1, pFile);

		offset = offset2;

		*pSize = framesize;
	}	

	return FrameBuff;
}



void ReadStreamFile()
{

	pFile = fopen("test.h264", "rb");
//		pFile = fopen("sender.h264", "rb");


	int frameSize = 0;
	


	void *pframe = NULL;
	int framesize = 0;
	int ret = 0;
	int offset = 0;

	long totalRead = 0;

	while ( 1 ) {

		pframe = GetFileFrame(&framesize, 0);

		printf("framesize %d \n", framesize);

		if ( 7 == (FrameBuff[4] & 0x1f) ) {
			break;
		}

	}



	if ( pframe ) {
	
		printf(" nal framesize %d \n", framesize);

		int k = framesize;
		for (k = 0; k < framesize; k++ ) {
			printf("%x", FrameBuff[k]);		
		
		}

		 	int w = 0, h = 0;

			h264_decode_sps((BYTE*)(FrameBuff+4), framesize-4, &w, &h );

			printf(" \n w = %d h = %d \n", w, h);
	}

}


int main(int argc, char* argv[])
{
 
	ReadStreamFile();


	return 0;
}


#endif

    
