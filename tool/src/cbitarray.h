#ifndef CBITARRAY_H
#define CBITARRAY_H


class CBitArray
{
public:
    static const char lZero[8];
    static const char rZero[8];
    static const char cZero[8];

    static int count_bit_size(const int bitlen) { return bitlen / 8 + (bitlen % 8 ? 1 : 0); }
    static void count_bit_size(const int bitlen, int &byteBegin, int &offset) { offset  = bitlen % 8; byteBegin = bitlen / 8; }

    static int bit_copy(char *to, int toByteLen,
                        const char *from, const int fromByteLen,
                        const int bitCopyLen, int sofs = 0, int dofs = 0);
public:
    CBitArray();
};

#endif // CBITARRAY_H
