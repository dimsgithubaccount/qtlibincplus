#include "cbitarray.h"
#include "core/src/cobjectdef.h"


const char CBitArray::lZero[8] = {char(0xFF), char(0x7F), char(0x3F), char(0x1F), char(0x0F), char(0x07), char(0x03), char(0x01) };
const char CBitArray::rZero[8] = {char(0x00), char(0x80), char(0xC0), char(0xE0), char(0xF0), char(0xF8), char(0xFC), char(0xFE) };
const char CBitArray::cZero[8] = {char(0x7F), char(0xBF), char(0xDF), char(0xEF), char(0xF7), char(0xFB), char(0xFD), char(0xFE) };

int CBitArray::bit_copy(char *to, int tByteLen,
                        const char *from,
                        int fByteLen,
                        const int bitCopyLen, int tofs, int fofs)
{
    VERIFY_RETVAL(bitCopyLen > 0 && tByteLen > 0 && fByteLen > 0, 0);
    VERIFY_RETVAL(to && from, 0);
    int tByteOfs, tBitOfs;
    count_bit_size(tofs, tByteOfs, tBitOfs);
    int fByteOfs, fBitOfs;
    count_bit_size(fofs, fByteOfs, fBitOfs);
    int ret = 0;
    return ret;
    VERIFY_RETVAL(tByteOfs < tByteLen && fByteOfs < fByteLen, ret);
}

CBitArray::CBitArray()
{

}
