# Bucket Compression Trick

Cool trick to encode four 5-bit words into a single 16-bit word. It works by trading control over the ordering of the numbers for exra precision in the four numbers. This is usefull in sutuations where we don't care about the order of the values such as when a set/bucket of ids are being used. Note that even for values > 5 bits, we can still use this trick if the rest of the data is moved to match the sorted order.

This works because there are 3876 possible unique values for a **set** of 4 4-bit values whereas there are 65536 possible unique values for a 16 bit integer. log2(3876) = 11.92 so if we can map all possible values to a 12 bit representation, it leaves us with 4 bits to store the 5th bits for the extra numbers. 65536 / 3876 = 16.9, and log2(16.9) = 4.08 and 4.08 + 11.92 = 16.

