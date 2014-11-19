/*   Base4K encoding for C
 *   Copyright (c) 2014 Secomba GmbH
 *   Licensed under the MIT license (https://raw.githubusercontent.com/secomba/base4k/master/LICENSE)
 *   Version: 1.0.0
 */

#include "base4k.h"

#include <stdlib.h>
#include <malloc.h>

b4kStatus base4kEncode(B4K_IN uint8_t* cData, B4K_IN_OUT uint32_t* ccData, B4K_OUT B4K_NEEDS_FREE uint16_t** cEncoded)
{
        static const unsigned int BASE_FLAG_START = 0x04000;
        static const unsigned int BASE1_START = 0x05000;
        static const unsigned int BASE_FLAG_SIZE = 0x100;
        static const unsigned int BASE1_SIZE = 0x05000;

        unsigned int offset = 0;
        unsigned int i = 0;
        unsigned int outputRunner = 0;
        
        *cEncoded = (uint16_t*)calloc(*ccData+1,sizeof(uint16_t));
        if(*cEncoded==NULL) {
                return B4K_ERROR;
        }

        for (i = 0; i < *ccData*2-2; i+=3) {
                if (i % 2 == 0)
                        offset = ((cData[i >> 1] << 4) | ((cData[(i >> 1) + 1] >> 4) & 0x0f)) & 0x0fff;
                else
                        offset = ((cData[(i - 1) >> 1] << 8) | (cData[(i + 1) >> 1] & 0xff)) & 0x0fff;

                (*cEncoded)[outputRunner++] = offset + BASE1_START;
        }

        if ((*ccData << 1) % 3 == 2) {
                offset = (cData[*ccData - 1] & 0xff) + BASE_FLAG_START;
                (*cEncoded)[outputRunner++] = offset;
        }
        else if ((*ccData << 1) % 3 == 1) {
                offset = (cData[*ccData - 1] & 0x0f) + BASE_FLAG_START;
                (*cEncoded)[outputRunner++] = offset;
        }

        (*cEncoded)[outputRunner] = '\0';
        *ccData = outputRunner;
        
        return B4K_SUCCESS;
}

b4kStatus base4KDecode(B4K_IN uint16_t* cEncoded, B4K_IN_OUT uint32_t* ccEncoded, B4K_OUT B4K_NEEDS_FREE uint8_t** cDecoded)
{
        static const unsigned int BASE_FLAG_START = 0x04000;
        static const unsigned int BASE1_START = 0x05000;
        static const unsigned int BASE_FLAG_SIZE = 0x100;
        static const unsigned int BASE1_SIZE = 0x05000;

        unsigned int code = 0;
        unsigned int prevCode = 0;
        unsigned int nrOfBytes = 0;
        unsigned int i = 0;
        unsigned int outputRunner = 0;
        unsigned int encodedLen = 0;

        if(*ccEncoded == B4K_AUTO) {
                for(;cEncoded[encodedLen] != 0; encodedLen++);
        }
        else {
                encodedLen = *ccEncoded;
        }
        *cDecoded = calloc(encodedLen*2, sizeof(uint8_t));
        if(*cDecoded==NULL)
                return B4K_ERROR;
                
        for (i = 0; i < encodedLen; i++) {
                prevCode = code;
                code = cEncoded[i];

                // check for valid encoding
                if (!(code >= BASE1_START && code < BASE1_START + BASE1_SIZE)) {
                        if (i < encodedLen - 1 || !(code >= BASE_FLAG_START && code < BASE_FLAG_START + BASE_FLAG_SIZE))
                                return B4K_ERROR;
                }

                if (code >= BASE1_START) {
                        code -= BASE1_START;
                }
                else {
                        code -= BASE_FLAG_START;
                        if (i % 2 == 0)
                                (*cDecoded)[outputRunner++] = code & 0xff;
                        else
                                (*cDecoded)[outputRunner++] = ((prevCode << 4) | (code & 0x0f)) & 0xff;
                        break;
                }
                if (i % 2 == 0) {
                        (*cDecoded)[outputRunner++] = (code >> 4) & 0xff;
                }
                else {
                        (*cDecoded)[outputRunner++] = ((prevCode << 4) | ((code & 0x0f00) >> 8)) & 0xff;
                        (*cDecoded)[outputRunner++] = code & 0xff;
                }
        }

        *ccEncoded = outputRunner;

        return B4K_SUCCESS;
}
