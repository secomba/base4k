#include "base4k.h"
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
        uint8_t data[10];
        uint32_t ccData = 10;
        uint16_t* cEncoded;
        uint8_t* cDecoded;
        int i = 0;

        for(i=0;i<10;i++)
                data[i] = i;
                
        printf("## plain ##\n");
        for(i=0;i<ccData;i++)
                printf("%d ",data[i]);
        printf("\n");

        base4kEncode(data, &ccData, &cEncoded);
        
        printf("## encoded ##\n");
        for(i=0;i<ccData;i++)
                printf("%4x ",cEncoded[i]);
        printf("\n");

        printf("## decoded ##\n");
        ccData = B4K_AUTO;
        if(base4KDecode(cEncoded, &ccData, &cDecoded)==B4K_SUCCESS) {
                for(i=0;i<ccData;i++)
                        printf("%d ",cDecoded[i]);
                printf("\n");
                free(cDecoded);
        } else {
                printf("decoding failed!\n");
        }

        free(cEncoded);
        return 0;
}