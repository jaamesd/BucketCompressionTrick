// #include <bits/stdc++.h>
#include <array>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <thread>
#include <chrono>

using namespace std;

template <class T, std::size_t N>
ostream& operator<<(ostream& oss, const array<T, N>& arrays)
{
    copy(arrays.cbegin(), arrays.cend(), ostream_iterator<T>(oss, " "));
    return oss;
}


class BucketCompressor
{
private:
    static bool mInitialized;
    static uint16_t mMaxCodeValue;
    static array<uint16_t, UINT16_MAX> mFwdLut;
    static array<uint16_t, UINT16_MAX / 4> mRevLut;

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
                        uint16_t sortedWord = i << 0 * 4 |
                                              j << 1 * 4 |
                                              k << 2 * 4 |
                                              l << 3 * 4;
                        /*  0 |<--------------------->| 32
                         *    |  i  |  j  |  k  |  l  |
                         *  The largest nibble is stored on the least
                         *  significant bits so we
                         */
                        mFwdLut[sortedWord] = mMaxCodeValue;
                        mRevLut[mMaxCodeValue] = sortedWord;
                        cout << "mapping 0x" << hex << setfill('0') << setw(4)
                            << mMaxCodeValue << " <----> " << "0x" << hex <<
                            setfill('0') << setw(4) << sortedWord << endl;
                        mMaxCodeValue++;
                    }
                }
            }
        }
    }

    static BucketCompressor& Instance()
    {
        static BucketCompressor instance;
        return instance;
    }

public:
    static void Compress(uint16_t &code, array<uint16_t, 4>dataIn)
    {
        for (const auto &data : dataIn)
        {
            assert(data < 1 << 5);
        }

        sort(dataIn.begin(), dataIn.end());

        uint16_t sortedWord = (dataIn[0] >> 1) << 3 * 4 |
                              (dataIn[1] >> 1) << 2 * 4 |
                              (dataIn[2] >> 1) << 1 * 4 |
                              (dataIn[3] >> 1) << 0 * 4;

        code = Instance().mFwdLut[sortedWord] << 4 |
                (dataIn[0] & 1) << 3 |
                (dataIn[1] & 1) << 2 |
                (dataIn[2] & 1) << 1 |
                (dataIn[3] & 1) << 0;

        cout << "Compressed:\t" << dataIn << endl;
        cout << "Into: " << code << endl;
    }

    static void Decompress(const uint16_t &code, array<uint16_t, 4>&dataOut)
    {
        assert(code >> 4 < mMaxCodeValue);

        uint16_t sortedWord = Instance().mRevLut[code >> 4];

        dataOut[0] = (sortedWord >> (3 * 4) & 0xf) << 1 | (code & (1 << 3)) >> 3 ;
        dataOut[1] = (sortedWord >> (2 * 4) & 0xf) << 1 | (code & (1 << 2)) >> 2 ;
        dataOut[2] = (sortedWord >> (1 * 4) & 0xf) << 1 | (code & (1 << 1)) >> 1 ;
        dataOut[3] = (sortedWord >> (0 * 4) & 0xf) << 1 | (code & (1 << 0)) >> 0 ;

        cout << "Decompressed:\t" << dataOut << endl;
        cout << "From: " << code << endl;
    }

    static void Compress(uint16_t &code, uint32_t dataIn)
    {
        assert(dataIn <= ((1 << 20) - 1));
        std::array<uint16_t, 4> buffer =
        {
            static_cast<uint16_t>(dataIn >> (0 * 4) & 0xf),
            static_cast<uint16_t>(dataIn >> (1 * 4) & 0xf),
            static_cast<uint16_t>(dataIn >> (2 * 4) & 0xf),
            static_cast<uint16_t>(dataIn >> (3 * 4) & 0xf),
        };
        Compress(code, buffer);
    }

    static void Decompress(const uint16_t &code, uint32_t &dataOut)
    {
        std::array<uint16_t, 4> buffer;
        Decompress(code, buffer);

        dataOut = (buffer[0] & 0xf) << (0 * 4) |
                  (buffer[1] & 0xf) << (1 * 4) |
                  (buffer[2] & 0xf) << (2 * 4) |
                  (buffer[3] & 0xf) << (3 * 4);
    }
};

bool BucketCompressor::mInitialized;
uint16_t BucketCompressor::mMaxCodeValue;
array<uint16_t, UINT16_MAX> BucketCompressor::mFwdLut;
array<uint16_t, UINT16_MAX / 4> BucketCompressor::mRevLut;

int main(int argc, char const *argv[])
{
    uint16_t code;
    srand(time(nullptr));
    std::array<uint16_t, 4> dataIn;
    std::array<uint16_t, 4> dataOut;

    for (auto &data : dataIn)
    {
        data = static_cast<uint16_t>(rand() % (1 << 5));
    }

    BucketCompressor::Compress(code, dataIn);
    BucketCompressor::Decompress(code, dataOut);
    sort(dataIn.begin(), dataIn.end());
    assert(dataIn == dataOut);

    while(1) {
        cout << "----------------------------" << endl;
        uint32_t x = rand() % (1 << 20);
        BucketCompressor::Compress(code, x);
        BucketCompressor::Decompress(code, x);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }



    return 0;
}