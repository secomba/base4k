/*   Base4K encoding for C
 *   Copyright (c) 2014 Secomba GmbH
 *   Licensed under the MIT license (https://raw.githubusercontent.com/secomba/base4k/master/LICENSE)
 *   Version: 1.0.0
 */

#ifndef __BASE4K_H__
#define __BASE4K_H__

#include <stdint.h>

#define B4K_IN		const	/* an input parameter */
#define B4K_OUT				/* an output parameter */
#define B4K_IN_OUT			/* an input and output parameter */
#define B4K_NEEDS_FREE		/* the caller is responsible to free() this value after usage */
#define B4K_AUTO	0xffffffff	/* string is 0 terminated */
#define B4K_SUCCESS	0		/* successful caluculation */
#define B4K_ERROR	1		/* calculation failed */
typedef int	b4kStatus;

/*	Encode an uint8_t array to an unicode uint16_t representation
 *  Parameters:
 *		* cData:	The data to encode as uint8_t array
 *		* ccData:	Contains the amount of bytes in cData. After returning successfully, contains the amount of
 *					unicode code points in cEncoded. If not successful, ccData remains untouched.
 *		* cEncoded:	A pointer to an array variable that will receive an uint16_t array containing the unicode
 *					code points. The array is allocated with calloc(), the caller is responsible to free() them
 *					if they are no longer required.
 *
 *	Result:	B4K_SUCCESS if successful, B4K_ERROR otherwise.
 */
b4kStatus base4kEncode(B4K_IN uint8_t* cData, B4K_IN_OUT uint32_t* ccData, B4K_OUT B4K_NEEDS_FREE uint16_t** cEncoded);

/*	Decode an uint16_t array of code points to its original uint8_t representation
 *	Parameters:
 *		* cEncoded:		An array containing the unicode code points of the base4k encoded string
 *		* ccEncoded:	Contains the amount of code points in cEncoded. Set this value to B4K_AUTO if the array has a
 *						0 termination. After returning successfully, contains the amount of bytes in cDecoded.
 *						If not successful, ccEncoded remains untouched.
 *		* cDecoded:		A pointer to an array variable that will receive an uint8_t array containing the decoded data.
 *						The array is allocated with calloc(), the caller is responsible to free() them if they are
 *						no longer required.
 *
 *	Result: B4K_SUCCESS if successful, B4K_ERROR if a decoding error occured.
 */
b4kStatus base4KDecode(B4K_IN uint16_t* cEncoded, B4K_IN_OUT uint32_t* ccEncoded, B4K_OUT B4K_NEEDS_FREE uint8_t** cDecoded);

#endif
