base4k
======

### Summary

Base4k is an encoding system originally developed for boxcryptor's filename encryption.
It uses approx. 4000 Asian unicode characters to encode an array of bytes.

### Target area of application

The encoding system is best used when you have to store binary data in an encoded format, and there is a space limitation on unicode characters. This is the case for many third-party websites, as well as for filenames. If encoded with base64, the amount of bytes would be expanded approx. by the factor 4/3. Base4k, on the other hand,
"shrinks" the bytes to an amount of approx. 2/3 unicode characters.

### Encoding algorithm

For the most part, the algorithm encodes 3 bytes to 2 unicode characters.

``[-byte 1-]---------------[-byte 2-]---------------[-byte 3-]``  
``[-----unicode character 1----][-----unicode character 2----]``

This procedure is simple: We concatenate one and a half bytes, interpret it as an integer and add the value 0x6000. The result is an unicode code point, encoding 1.5 bytes. For three bytes, this results in two unicode characters. Note that the actual memory amount necessary to store the two unicode characters is much larger, if utf-8 is used, the amount of bytes doubles.

If this algorithm is continued, we might end up with either one or half a byte at the end that couldn't be encoded. In this case, we add 0x4000 to it, and interpret the result as the final code point.

### Mapping

Base4k maps bytes (0x00-0xff) to unicode points in the areas 0x4000-0x40ff and 0x6000-0x6fff. The reasons for this area are as follows:  
<ul>
<li>There are no control characters in this area</li>
<li>All characters are printable</li>
<li>There are no similiar characters that might get converted to the same character on the (remote) platform</li>
</ul>

### Legacy

The first version of Base4k used a slightly different character range from 0x4000-0x400ff and 0x5000-0x5fff. The range was subsequently changed to exclude some unwanted characters from the encoding set. The provided implementations are able to handle both versions automatically on decoding, and can be configured to use the deprecated character set for encoding as well.

### Building the examples
##### C/C++

>**Command:** gcc -o example.exe *.c

##### Javascript

>**Command:** just open example.htm

##### Typescript

>**Command:** tsc base4k.ts

>open example.htm

##### Java

>**Command:** javac Example.java com/secomba/base4k/Base4K.java com/secomba/base4k/DecodingFailedException.java com/secomba/base4k/Base4KVersion.java

##### C&#35;

>**Command:** csc /out:example.exe *.cs

### Test vectors

encode("") = ""; <br />
encode("f") = "䁦"; <br />
encode("fo") = "晦䀏"; <br />
encode("foo") = "晦潯"; <br />
encode("foob") = "晦潯䁢"; <br />
encode("fooba") = "晦潯昦䀁"; <br />
encode("foobar") = "晦潯昦慲"; <br />
encode("please encode me!") = "朆汥昗捥戆敮昶潤晒恭晒䀁"