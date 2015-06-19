/**
 * pelco-d –≠“È
 */

#ifndef __PELCOP_H__
#define __PELCOP_H__

int pelcopPtzStop(char u8Addr);
int pelcopPtzAuto(char u8Addr,  int s32Speed);
int pelcopPtzAutoStop(char u8Addr,  int s32Speed);

int pelcopPtzLeft(char u8Addr, int s32Speed);
int pelcopPtzRight(char u8Addr, int s32Speed);
int pelcopPtzUp(char u8Addr,  int s32Speed);
int pelcopPtzDown(char u8Addr,  int s32Speed);
int pelcopPtzUpRight(char u8Addr,  int s32Speed);
int pelcopPtzUpLeft(char u8Addr,  int s32Speed);
int pelcopPtzDownLeft(char u8Addr,  int s32Speed);
int pelcopPtzDownRight(char u8Addr,  int s32Speed);

int pelcopPtzIrisInc(char u8Addr);
int pelcopPtzIrisDec(char u8Addr);
int pelcopPtzZoomInc(char u8Addr);
int pelcopPtzZoomDec(char u8Addr);
int pelcopPtzFocusInc(char u8Addr);
int pelcopPtzFocusDec(char u8Addr);

int pelcopPtzGotoPoint(char u8Addr, int s32Point);
int pelcopPtzSetPoint(char u8Addr, int s32Point);
int pelcopPtzClearPoint(char u8Addr, int s32Point);

#endif
