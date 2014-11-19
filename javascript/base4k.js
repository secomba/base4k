/*   Base4K encoding for JavaScript
 *   Copyright (c) 2014 Secomba GmbH
 *   Licensed under the MIT license (https://raw.githubusercontent.com/secomba/base4k/master/LICENSE)
 *   Version: 1.0.0
 */

//-----CLASS Base4K---------------------------------------------------------
Base4K = function() {

}

Base4K.BASE_FLAG_START = 0x04000;
Base4K.BASE1_START     = 0x05000;
Base4K.BASE_FLAG_SIZE  = 0x100;
Base4K.BASE1_SIZE      = 0x01000;

// raw=Uint8Array, ret=String
Base4K.encode = function(raw)
{
        var offset = 0;
        var result = "";

        for(var i=0;i<raw.length*2-2;i+=3) {
                if(i%2==0)
                        offset = ((raw[i/2]<<4)|((raw[i/2+1]>>>4)&0x0f))&0x0fff;
                else
                        offset = ((raw[(i-1)/2]<<8)|(raw[(i+1)/2]&0xff))&0x0fff;

                offset += Base4K.BASE1_START;

                result += String.fromCharCode(offset);
        }

        if((raw.length*2) % 3 == 2) {
                offset = (raw[raw.length-1]&0xff)+Base4K.BASE_FLAG_START;
                result += String.fromCharCode(offset);
        } else if((raw.length*2) % 3 == 1) {
                offset = ((raw[raw.length-1]&0x0f))+Base4K.BASE_FLAG_START;
                result += String.fromCharCode(offset);
        }

        return result;
}

// encoded=String, ret=Uint8Array
Base4K.decode = function(encoded)
{
        var code = 0;

        var buffer = new Array();
        var intCollector = new Array();

        for(var i=0;i<encoded.length;i++) {

                code = encoded.charCodeAt(i);

                if(!(code>=Base4K.BASE1_START && code<Base4K.BASE1_START+Base4K.BASE1_SIZE)) {
                   if(i<encoded.length-1 || !(code>=Base4K.BASE_FLAG_START && code<Base4K.BASE_FLAG_START+Base4K.BASE_FLAG_SIZE)) {
                           throw "invalid Base4K encoding";
                   }
                }
                intCollector.push(code);
        }

        for(var i=0;i<intCollector.length;i++) {
                if(intCollector[i]>=Base4K.BASE1_START) {
                        intCollector[i] -= Base4K.BASE1_START;
                } else {
                        intCollector[i] -= Base4K.BASE_FLAG_START;
                        if(i%2==0)
                                buffer.push(intCollector[i]&0xff);
                        else
                                buffer.push(((intCollector[i-1]<<4)|((intCollector[i]&0x0f))&0xff));
                        break;
                }
                if(i%2==0) {
                        buffer.push((intCollector[i]>>>4)&0xff);
                } else {
                        buffer.push(((intCollector[i-1]<<4)|((intCollector[i]&0x0f00)>>>8))&0xff);
                        buffer.push(intCollector[i]&0xff);
                }
        }

        return new Uint8Array(buffer);
}