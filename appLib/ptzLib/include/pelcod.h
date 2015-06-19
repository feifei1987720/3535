/**
 * 
 * pelco-d –≠“È
 */

#ifndef __PELCOD_H__
#define __PELCOD_H__

int pelcodPtzStop(char u8Addr);
int pelcodPtzAuto(char u8Addr,  int s32Speed);
int pelcodPtzAutoStop(char u8Addr,  int s32Speed);

int pelcodPtzLeft(char u8Addr, int s32Speed);
int pelcodPtzRight(char u8Addr, int s32Speed);
int pelcodPtzUp(char u8Addr,  int s32Speed);
int pelcodPtzDown(char u8Addr,  int s32Speed);
int pelcodPtzUpRight(char u8Addr,  int s32Speed);
int pelcodPtzUpLeft(char u8Addr,  int s32Speed);
int pelcodPtzDownLeft(char u8Addr,  int s32Speed);
int pelcodPtzDownRight(char u8Addr,  int s32Speed);

int pelcodPtzIrisInc(char u8Addr);///iris +
int pelcodPtzIrisDec(char u8Addr);///iris -
int pelcodPtzZoomInc(char u8Addr);///Zoom+
int pelcodPtzZoomDec(char u8Addr);///Zoom-
int pelcodPtzFocusInc(char u8Addr);///Focus+
int pelcodPtzFocusDec(char u8Addr);///Focus-

int pelcodPtzGotoPoint(char u8Addr, int s32Point);
int pelcodPtzSetPoint(char u8Addr, int s32Point);
int pelcodPtzClearPoint(char u8Addr, int s32Point);

#endif
