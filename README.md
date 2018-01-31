# Bucket Compression Trick

### What is it?
It's a neat little trick to efficiently squeeze four 5-bit values into a single 16-bit value when we don't care about the order of those 5-bit values and we want to get real close to that Shannon limit. 

This is good for storing things like a series of order independent options in an entry or entry identifiers in a very large hash table. Note that even for values > 5 bits, we can still apply this trick if we're willing to move the rest of the to match the sorted order of those 5 bits but obviously you wouldn't want to shuffle around hundreds of bytes just to save 4 bits.

### How does it work?
This trick works by trading control over the ordering of the numbers for extra precision in the four numbers. 

There are multichoose(2<sup>4</sup>, 4) = 3876 possible unique values for a **multiset** of 4 4-bit values, whereas there are 65536 possible unique values for a 16 bit integer. log2(multichoose(2<sup>4</sup>, 4)) = 11.92 so if we can map all possible values to a 12 bit representation, it leaves us with 4 bits to play around with and that's enough to store the extra bit in our four 5-bit numbers. Theoretically we have a bit more than 4 bits to play around with as log2(2<sup>16</sup> / 3876) = 4.08 or from 16 - 11.92 but in practice that fraction is a bit harder to extract without a larger or more indirect lookup table.

More generally, a multiset of m n-bit numbers cannot hold more than m * n bits of information and it is possible to leverage that difference to store m' n'-bit numbers in the same m * n bits as long as log2(multichoose(m', n')) <= m * n. log2(multichoose(2<sup>5</sup>, 4)) = 15.67 bits < 16 bits. 

### Further reading:
* More information on multisets: [Wikipedia](https://en.wikipedia.org/wiki/Multiset#Counting_multisets)
* More general methods: [Compressing Sets and Multisets of Sequences](https://arxiv.org/pdf/1401.6410.pdf)
* Practical applications: [Cuckoo Filters](https://www.cs.cmu.edu/~dga/papers/cuckoo-conext2014.pdf), [Compressing Genomes](https://hips.seas.harvard.edu/blog/2013/06/05/compressing-genomes/)
