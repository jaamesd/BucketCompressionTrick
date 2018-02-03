// #include <bits/stdc++.h>
#include <array>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <thread>
#include <chrono>

using namespace std;

class BucketCompressor
{
private:
    static bool mInitialized;
    static uint16_t mMaxIndex;
    static array<uint16_t, 1 << 16> mFwdLut;
    static array<uint16_t, 1 << 12> mRevLut;

    BucketCompressor()
    {
        if (mInitialized)
        {
            return;
        }
        for (uint8_t i = 0; i < 1 << 4; i++)
        {
            for (uint8_t j = 0; j <= i; j++)
            {
                for (uint8_t k = 0; k <= j; k++)
                {
                    for (uint8_t l = 0; l <= k; l++)
                    {
                        uint16_t sortedValues = i << 0 * 4 |
                                                j << 1 * 4 |
                                                k << 2 * 4 |
                                                l << 3 * 4;
                        /*  0                       16
                         *  |<-----  16 bits  ----->|
                         *  |  i  |  j  |  k  |  l  |
                         *  where i >= j >= k >= l, all are 4 bit values
                         *  the largest value is stored in the least significant
                         *  bits so it's monotonically increasing just like index
                         */
                        mFwdLut[sortedValues] = mMaxIndex;
                        mRevLut[mMaxIndex] = sortedValues;
                        cout << "mapping 0x" << hex << setfill('0') << setw(4)
                            << mMaxIndex << " <----> " << "0x" << hex <<
                            setfill('0') << setw(4) << sortedValues << endl;
                        mMaxIndex++;
                    }
                }
            }
        }
        mInitialized = true;
    }

    static BucketCompressor& Instance()
    {
        static BucketCompressor instance;
        return instance;
    }

public:
    static void Compress(uint16_t &code, array<uint8_t, 4>dataIn)
    {
        for (const auto &data : dataIn)
        {
            assert(data < 1 << 5);
        }

        // Sort to descending order
        sort(dataIn.begin(), dataIn.end(), greater<uint8_t>());

        /*  0                                5
         *  |<----------  5 bits  ---------->|
         *  |   value & 1   |   value >> 1   |
         *  |             value              |
         *  split value into most significant bits and least significant bit
         */

        /*  0                                   16
         *  |<-----------  16 bits  ----------->|
         *  | i >> 1 | j >> 1 | k >> 1 | l >> 1 |
         *  |           sorted values           |
         *  where i >= j >= k >= l, all are 5 bit values
         *  use the most significant bits for each value to form the sorted word
         *  the largest value is again stored in the least significant bits to
         *  match our lookup tables
         */

        uint16_t sortedValues = (dataIn[0] >> 1) << 0 * 4 |   // i
                                (dataIn[1] >> 1) << 1 * 4 |   // j
                                (dataIn[2] >> 1) << 2 * 4 |   // k
                                (dataIn[3] >> 1) << 3 * 4;    // l

        /*  0                               4               16
         *  |<---------  4  bits  --------->|<-  12 bits  ->|
         *  | i & 1 | j & 1 | k & 1 | l & 1 |   lut index   |
         *  |                     code                      |
         *  where i >= j >= k >= l
         *  look up the index of sorted value in the lookup table we created and
         *  store in the least significant 4 bits of the code, the least
         *  significant bits of the values in sorted order so we can retrieve it
         */
        code = Instance().mFwdLut[sortedValues] << 4 |
                (dataIn[0] & 1) << 0 |
                (dataIn[1] & 1) << 1 |
                (dataIn[2] & 1) << 2 |
                (dataIn[3] & 1) << 3;

        cout << "Compressed:\t{ ";

        for (auto &data : dataIn)
        {
            cout << setfill('0') << setw(2) << +data << " ";
        }

        cout << "} Into: 0x" << hex << setfill('0') << setw(4) << code << endl;
    }

    static void Decompress(const uint16_t &code, array<uint8_t, 4>&dataOut)
    {
        assert(code >> 4 < mMaxIndex);

        // use the lut index to retrieve the sorted values
        uint16_t sortedValues = Instance().mRevLut[code >> 4];

        // piece them back together
        dataOut[0] = (sortedValues >> (0 * 4) & 0xf) << 1 | (code & (1 << 0)) >> 0 ;
        dataOut[1] = (sortedValues >> (1 * 4) & 0xf) << 1 | (code & (1 << 1)) >> 1 ;
        dataOut[2] = (sortedValues >> (2 * 4) & 0xf) << 1 | (code & (1 << 2)) >> 2 ;
        dataOut[3] = (sortedValues >> (3 * 4) & 0xf) << 1 | (code & (1 << 3)) >> 3 ;

        cout << "Decompressed:\t{ ";

        for (auto &data : dataOut)
        {
            cout << setfill('0') << setw(2) << +data << " ";
        }

        cout << "} From: 0x" << hex << setfill('0') << setw(4) << code << endl;
    }

    static void Compress(uint16_t &code, uint32_t dataIn)
    {
        assert(dataIn <= ((1 << 20) - 1));
        std::array<uint8_t, 4> buffer =
        {
            static_cast<uint8_t>(dataIn >> (0 * 5) & 0x1f),
            static_cast<uint8_t>(dataIn >> (1 * 5) & 0x1f),
            static_cast<uint8_t>(dataIn >> (2 * 5) & 0x1f),
            static_cast<uint8_t>(dataIn >> (3 * 5) & 0x1f),
        };
        Compress(code, buffer);
    }

    static void Decompress(const uint16_t &code, uint32_t &dataOut)
    {
        std::array<uint8_t, 4> buffer;
        Decompress(code, buffer);

        dataOut = (buffer[0] & 0x1f) << (0 * 5) |
                  (buffer[1] & 0x1f) << (1 * 5) |
                  (buffer[2] & 0x1f) << (2 * 5) |
                  (buffer[3] & 0x1f) << (3 * 5);
    }
};

bool BucketCompressor::mInitialized;
uint16_t BucketCompressor::mMaxIndex;
array<uint16_t, 1 << 16> BucketCompressor::mFwdLut;
array<uint16_t, 1 << 12> BucketCompressor::mRevLut;

int main(int argc, char const *argv[])
{
    uint16_t code;
    std::array<uint8_t, 4> dataIn;
    std::array<uint8_t, 4> dataOut;

    cout << "--------------------------------------------" << endl;
    cout << "test all sets of four 5-bit numbers in order" << endl;
    for (uint8_t i = 0; i < 1 << 5; i++)
    {
        for (uint8_t j = 0; j <= i; j++)
        {
            for (uint8_t k = 0; k <= j; k++)
            {
                for (uint8_t l = 0; l <= k; l++)
                {
                    cout << "--------------------------------------------" << endl;
                    dataIn[0] = i;
                    dataIn[1] = j;
                    dataIn[2] = k;
                    dataIn[3] = l;
                    BucketCompressor::Compress(code, dataIn);
                    BucketCompressor::Decompress(code, dataOut);
                    assert(dataIn == dataOut);
                }
            }
        }
    }

    cout << "test random sets of four 5-bit numbers" << endl;
    srand(time(nullptr));
    while(1) {
        cout << "--------------------------------------------" << endl;
        uint32_t x = rand() % (1 << 20);
        BucketCompressor::Compress(code, x);
        BucketCompressor::Decompress(code, x);
    }

    return 0;
}
