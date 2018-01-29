# Bucket Compression Trick

### What is it?
It's a neat little trick to efficiently squeeze four 5-bit values into a single 16-bit value when we don't care about the order of those 5-bit values and we want to get real close to that shannon limit to save space. 

This is good for storing a series of order independent options in an entry or entry ids in a very large hash table. Note that even for values > 5 bits, we can still use this trick if we're willing to move the rest of the data is moved to match the sorted order of those 5 bits but obviously you wouldn't want to shuffle around hundreds of bytes just to save 4 bits.

### How does it work?
This trick works by trading control over the ordering of the numbers for exra precision in the four numbers. There are only 3876 possible unique values for a **set** of 4 4-bit values whereas there are 65536 possible unique values for a 16 bit integer. log2(3876) = 11.92 so if we can map all possible values to a 12 bit representation, it leaves us with 4 bits to store the 5th bits for the extra numbers. 65536 / 3876 = 16.9, and log2(16.9) = 4.08 and 4.08 + 11.92 = 16.

### Further reading:
* More information on multisets: [Wikipedia](https://en.wikipedia.org/wiki/Multiset#Counting_multisets)
* More general methods: [Compressing Sets and Multisets of Sequences](https://arxiv.org/pdf/1401.6410.pdf)
* Practical applications: [Cuckoo Filters](https://www.cs.cmu.edu/~dga/papers/cuckoo-conext2014.pdf), [Compressing Genomes](https://hips.seas.harvard.edu/blog/2013/06/05/compressing-genomes/)
