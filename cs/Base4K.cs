#region

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

#endregion

namespace Secomba
{
    public static class Base4K
    {
        // Base addresses for mapping regions
        //
        private const int BASE_FLAG_START = 0x04000;
        private const int BASE1_START = 0x05000;

        // Sizes of each mapping region
        //
        private const int BASE_FLAG_SIZE = 0x100;
        private const int BASE1_SIZE = 0x01000;

        /// <summary>
        ///     Encodes the specified raw data as Base4k.
        /// </summary>
        /// <param name="raw">The raw.</param>
        /// <returns></returns>
        public static string Encode(byte[] raw)
        {
            Encoding enc = new UTF8Encoding(true, true);

            using(var buffer = new MemoryStream()) {
                int offset;
                for(var i = 0; i < raw.Length*2 - 2; i += 3) {
                    if(i%2 == 0) {
                        offset = ((raw[i/2] << 4) | ((raw[i/2 + 1] >> 4) & 0x0f)) & 0x0fff; // In Java original >>
                    } else {
                        offset = ((raw[i/2] << 8) | (raw[i/2 + 1] & 0xff)) & 0x0fff;
                    }

                    offset += BASE1_START;

                    // now that offset is a valid unicode character: code it to utf-8
                    var utfByets = ToUtf8(offset);
                    buffer.Write(utfByets, 0, utfByets.Length);
                }

                if((raw.Length*2)%3 == 2) {
                    offset = (raw[raw.Length - 1] & 0xff) + BASE_FLAG_START;
                    var utfByets = ToUtf8(offset);
                    buffer.Write(utfByets, 0, utfByets.Length);
                } else if((raw.Length*2)%3 == 1) {
                    offset = ((raw[raw.Length - 1] & 0x0f)) + BASE_FLAG_START;
                    var utfByets = ToUtf8(offset);
                    buffer.Write(utfByets, 0, utfByets.Length);
                }

                return enc.GetString(buffer.ToArray());
            }
        }


        /// <summary>
        ///     Decodes the specified encoded from Base65 - returns null if decoding failed.
        /// </summary>
        /// <param name="encoded">The encoded.</param>
        /// <returns></returns>
        public static byte[] Decode(string encoded)
        {
            int code;
            int nrOfBytes;

            Encoding enc = new UTF8Encoding(true, true);
            byte[] encBytes = enc.GetBytes(encoded);

            using(var buffer = new MemoryStream()) {
                var intCollector = new List<int>();

                for(var i = 0; i < encBytes.Length;) {
                    if((encBytes[i] & 0x80) == 0) {
                        // 1 byte
                        nrOfBytes = 1;
                    } else if((encBytes[i] & 0x20) == 0) {
                        // 2 bytes
                        nrOfBytes = 2;
                    } else if((encBytes[i] & 0x10) == 0) {
                        // 3 bytes
                        nrOfBytes = 3;
                    } else {
                        // 4 bytes
                        nrOfBytes = 4;
                    }

                    code = ToCode(encBytes, i, nrOfBytes);
                    i += nrOfBytes;

                    if(!(code >= BASE1_START && code < BASE1_START + BASE1_SIZE)) {
                        if(i < encBytes.Length || !(code >= BASE_FLAG_START && code < BASE_FLAG_START + BASE_FLAG_SIZE)) {
                            return null;
                        }
                    }
                    intCollector.Add(code);
                }

                var tempCodeBuffer = intCollector.ToArray();

                for(var i = 0; i < tempCodeBuffer.Length; i++) {
                    if(tempCodeBuffer[i] >= BASE1_START) {
                        tempCodeBuffer[i] -= BASE1_START;
                    } else {
                        tempCodeBuffer[i] -= BASE_FLAG_START;
                        if(i%2 == 0) {
                            buffer.WriteByte((byte)tempCodeBuffer[i]);
                        } else {
                            buffer.WriteByte((byte)(((tempCodeBuffer[i - 1] << 4) | ((tempCodeBuffer[i] & 0x0f)) & 0xff)));
                        }
                        break;
                    }
                    if(i%2 == 0) {
                        buffer.WriteByte((byte)(tempCodeBuffer[i] >> 4)); // In Java original >>>
                    } else {
                        buffer.WriteByte((byte)(((tempCodeBuffer[i - 1] << 4) | ((tempCodeBuffer[i] & 0x0f00) >> 8)) & 0xff)); // In Java original >>>
                        buffer.WriteByte((byte)(tempCodeBuffer[i] & 0xff));
                    }
                }

                return buffer.ToArray();
            }
        }

        /// <summary>
        ///     Converts a code to the UTF8.
        /// </summary>
        /// <param name="code">The code.</param>
        /// <returns></returns>
        public static byte[] ToUtf8(int code)
        {
            byte[] result;

            // test for big codes first, since small ones generally don't occur.
            if(code > 0xffff) {
                result = new byte[4];
                result[0] = (byte)(0xf0 | ((code >> 18) & 0x07));
                result[1] = (byte)(0x80 | ((code >> 12) & 0x3f));
                result[2] = (byte)(0x80 | ((code >> 6) & 0x3f));
                result[3] = (byte)(0x80 | (code & 0x3f));
            } else if(code > 0x7ff) {
                result = new byte[3];
                result[0] = (byte)(0xe0 | ((code >> 12) & 0x0f));
                result[1] = (byte)(0x80 | ((code >> 6) & 0x3f));
                result[2] = (byte)(0x80 | (code & 0x3f));
            } else if(code > 0x7f) {
                result = new byte[2];
                result[0] = (byte)(0xc0 | ((code >> 6) & 0x1f));
                result[1] = (byte)(0x80 | (code & 0x3f));
            } else {
                result = new byte[1];
                result[0] = (byte)(0x00 | (code & 0x7f));
            }

            return result;
        }

        /// <summary>
        ///     Converts a Utf8Char to the code.
        /// </summary>
        /// <param name="utf8Char">The UTF8 char.</param>
        /// <param name="offset">The offset.</param>
        /// <param name="length">The length.</param>
        /// <returns></returns>
        private static int ToCode(IList<byte> utf8Char, int offset, int length)
        {
            var result = 0;

            switch(length) {
                case 1:
                    result |= utf8Char[offset];
                    break;
                case 2:
                    result |= (utf8Char[offset + 0] & 0x1f) << 6;
                    result |= (utf8Char[offset + 1] & 0x3f);
                    break;
                case 3:
                    result |= (utf8Char[offset + 0] & 0x0f) << 12;
                    result |= (utf8Char[offset + 1] & 0x3f) << 6;
                    result |= (utf8Char[offset + 2] & 0x3f);
                    break;
                case 4:
                    result |= (utf8Char[offset + 0] & 0x07) << 18;
                    result |= (utf8Char[offset + 1] & 0x3f) << 12;
                    result |= (utf8Char[offset + 2] & 0x3f) << 6;
                    result |= (utf8Char[offset + 3] & 0x3f);
                    break;
            }

            return result;
        }
    }
}