/*   Base4K encoding for TypeScript
 *   Copyright (c) 2014-2018 Secomba GmbH
 *   Licensed under the MIT license (https://raw.githubusercontent.com/secomba/base4k/master/LICENSE)
 *   Version: 1.0.0
 */


// tslint:disable:no-bitwise

const BASE_FLAG_START = 0x04000;
const BASE1_START = 0x05000;
const BASE_FLAG_SIZE = 0x100;
const BASE1_SIZE = 0x01000;

class Base4kDecodingError extends Error {
  constructor(message?: string) {
    super(message);
  }
}

function encode(raw: Uint8Array): string {
  let offset = 0;
  let result = '';

  for (let i = 0; i < raw.length * 2 - 2; i += 3) {
    // tslint:disable-next-line:prefer-conditional-expression
    if (i % 2 === 0) {
      offset = ((raw[i / 2] << 4) | ((raw[i / 2 + 1] >>> 4) & 0x0f)) & 0x0fff;
    } else {
      offset = ((raw[(i - 1) / 2] << 8) | (raw[(i + 1) / 2] & 0xff)) & 0x0fff;
    }

    offset += BASE1_START;

    result += String.fromCharCode(offset);
  }

  if ((raw.length * 2) % 3 === 2) {
    offset = (raw[raw.length - 1] & 0xff) + BASE_FLAG_START;
    result += String.fromCharCode(offset);
  } else if ((raw.length * 2) % 3 === 1) {
    offset = (raw[raw.length - 1] & 0x0f) + BASE_FLAG_START;
    result += String.fromCharCode(offset);
  }

  return result;
}

function decode(encoded: string): Uint8Array {
  let code = 0;

  const buffer = new Array();
  const intCollector = new Array();

  for (let i = 0; i < encoded.length; i++) {
    code = encoded.charCodeAt(i);

    if (!(code >= BASE1_START && code < BASE1_START + BASE1_SIZE)) {
      if (i < encoded.length - 1 || !(code >= BASE_FLAG_START && code < BASE_FLAG_START + BASE_FLAG_SIZE)) {
        throw new Base4kDecodingError('invalid Base4K encoding');
      }
    }
    intCollector.push(code);
  }

  for (let i = 0; i < intCollector.length; i++) {
    if (intCollector[i] >= BASE1_START) {
      intCollector[i] -= BASE1_START;
    } else {
      intCollector[i] -= BASE_FLAG_START;
      if (i % 2 === 0) {
        buffer.push(intCollector[i] & 0xff);
      } else {
        buffer.push((intCollector[i - 1] << 4) | (intCollector[i] & 0x0f & 0xff));
      }
      break;
    }
    if (i % 2 === 0) {
      buffer.push((intCollector[i] >>> 4) & 0xff);
    } else {
      buffer.push(((intCollector[i - 1] << 4) | ((intCollector[i] & 0x0f00) >>> 8)) & 0xff);
      buffer.push(intCollector[i] & 0xff);
    }
  }

  return new Uint8Array(buffer);
}
