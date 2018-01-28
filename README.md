# CompressionTrick

Cool trick to store 4 5-bit values in one 16 bit value.

This works because there are 3876 possible unique values for a set of 4 4 bit values whereas there are 65536 possible unique values for a 16 bit integer. log2(3876) = 11.92 so if we can map all possible values to a 12 bit representation which leaves us with 4 bits to store the 5th bit for the extra numbers. This makes sense because 65536 / 3876 = 16.9, and log2(16.9) = 4.08 and 4.08 + 11.92 = 16.

Another way to think about it is

