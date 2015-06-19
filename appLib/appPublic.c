#include <stdio.h>
#if  1
const static char table64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void Base64Encode2(const char *InStr, char *OutStr)
{
    unsigned char ibuf[3] = {0};
    unsigned char obuf[4] = {0};
    int i = 0;
    int inputparts = 0;

    while(*InStr) 
    {   
        for (i =  0; i < 3; i++) 
        {   
            if(*InStr) 
            {   
                inputparts++;
                ibuf[i] = *InStr;
                InStr++;
            }   
            else
            {
                ibuf[i] = 0;
            }
        }

        obuf [0] = (ibuf [0] & 0xFC) >> 2;
        obuf [1] = ((ibuf [0] & 0x03) << 4) | ((ibuf [1] & 0xF0) >> 4);
        obuf [2] = ((ibuf [1] & 0x0F) << 2) | ((ibuf [2] & 0xC0) >> 6);
        obuf [3] = ibuf [2] & 0x3F;

          switch(inputparts)
          {
              case 1: /* only one byte read */
                  sprintf(OutStr, "%c%c==",
                      table64[obuf[0]],
                      table64[obuf[1]]);
                  break;
              case 2: /* two bytes read */
                  sprintf(OutStr, "%c%c%c=",
                      table64[obuf[0]],
                      table64[obuf[1]],
                      table64[obuf[2]]);
                  break;
              default:
                  sprintf(OutStr, "%c%c%c%c",
                      table64[obuf[0]],
                      table64[obuf[1]],
                      table64[obuf[2]],
                      table64[obuf[3]] );
              break;
        }
        OutStr += 4;
    }
    *OutStr=0;
}
#endif

