/*   Base4K encoding for Java
 *   Copyright (c) 2014 Secomba GmbH
 *   Licensed under the MIT license (https://raw.githubusercontent.com/secomba/base4k/master/LICENSE)
 *   Version: 1.0.0
 */

package com.secomba.base4k;

import java.util.ArrayList;

public class Base4K 
{
	private static final int BASE_FLAG_START = 0x04000;
	private static final int BASE1_START     = 0x06000;
	private static final int BASE1_START_LEGACY = 0x05000;
	private static final int BASE_FLAG_SIZE  = 0x100;
	private static final int BASE1_SIZE      = 0x01000;

	private Base4KVersion _version;

	public Base4K(Base4KVersion version) {
		_version = version;
	}
	
	public String encode(byte[] raw)
	{
		int offset = 0;
		StringBuffer result = new StringBuffer();
		
		for(int i=0;i<raw.length*2-2;i+=3) {
			if(i%2==0) {
				offset = ((raw[i/2]<<4)|((raw[i/2+1]>>>4)&0x0f))&0x0fff;
			} else { 
				offset = ((raw[(i-1)/2]<<8)|(raw[(i+1)/2]&0xff))&0x0fff;
			}

			offset += _version == Base4KVersion.V1 ? Base4K.BASE1_START_LEGACY : Base4K.BASE1_START;
			
			result.appendCodePoint(offset);
		}
		
		if((raw.length*2) % 3 == 2) {
			offset = (raw[raw.length-1]&0xff)+Base4K.BASE_FLAG_START;
			result.appendCodePoint(offset);
		} else if((raw.length*2) % 3 == 1) {
			offset = ((raw[raw.length-1]&0x0f))+Base4K.BASE_FLAG_START;
			result.appendCodePoint(offset);
		}

		return result.toString();
	}

	public byte[] decode(String encoded) throws DecodingFailedException
	{
		try {
			return decodeInternal(encoded, Base4K.BASE1_START);
		} catch (DecodingFailedException e) {
			return decodeInternal(encoded, Base4K.BASE1_START_LEGACY);
		}
	}
	
	private byte[] decodeInternal(String encoded, int base1Start) throws DecodingFailedException
	{
		int code = 0;
		ArrayList<Byte> buffer = new ArrayList<Byte>();
		ArrayList<Integer> intCollector = new ArrayList<Integer>();
		
		for(int i=0;i<encoded.length();i++) {
			code = encoded.codePointAt(i);
			
			if(!(code>=base1Start && code<base1Start+BASE1_SIZE)) {
				if(i<encoded.length()-1 || !(code>=BASE_FLAG_START && code<BASE_FLAG_START+BASE_FLAG_SIZE)) {
					throw new DecodingFailedException();
				}
			}
			intCollector.add(code);
		}
		
		for(int i=0;i<intCollector.size();i++) {
			if(intCollector.get(i)>=base1Start) {
				intCollector.set(i, intCollector.get(i) - base1Start);
			} else {
				intCollector.set(i, intCollector.get(i) - BASE_FLAG_START);
				if(i%2==0) {
					buffer.add((byte)(intCollector.get(i)&0xff));
				} else { 
					buffer.add((byte)(((intCollector.get(i-1)<<4)|((intCollector.get(i)&0x0f))&0xff)));
				}
				break;
			}
			if(i%2==0) {
				buffer.add((byte)((intCollector.get(i)>>>4)&0xff));
			} else {
				buffer.add((byte)(((intCollector.get(i-1)<<4)|((intCollector.get(i)&0x0f00)>>>8))&0xff));
				buffer.add((byte)(intCollector.get(i)&0xff));
			}
		}
			
		byte[] bResult = new byte[buffer.size()];
		for(int i=0;i<buffer.size();i++) {
			bResult[i] = buffer.get(i);
		}
		return bResult;
	}
}
