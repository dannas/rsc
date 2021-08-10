### Information storage
Computers process information as two-valued signals: bits. 
They are grouped as 8-bits: bytes. Expressing them using binary notation
becomes cumbersome: The byte `0b1011_1100` is more easily viewed in hexadecimal:
`0xbc`.

To make the bits understandable we need to understand their encoding. Positive
numbers, signed numbers, strings and floating point numbers uses different
encodings (and many more exists).

A computer has a certain register width, typically 32 bits or 64 bits for
desktop processors. The address space is the ordered set of addresses
reachable from a pointer. If the pointer is limited to 32 bits, then the
address space is 32 bits.

But data can be processed in smaller chunks. 8, 4, 2 or 1 bytes are common
sizes. The x86_64 architecture has shadowed register for different sizes: rax
is 8 bytes, eax is 4 bytes, ax is 2 bytes and ah and al are 1 byte. The C
programming language has types of different sizes, but the mapping between the
architecture word sizes and the C types are implementation defined. The
Windows 64-bits API uses 8 bytes for long, while Linux uses 4 for instance.

The bytes are stored in memory and can be fetched via their addresses. A RAM
transaction retrieves a few hundred bytes at a time. The cache between the
processor and RAM stores data in cache lines, 64 bytes long. Some systems
store data in big-endian form and others do it in little-endian.

A string is a sequence of characters. Each character is encoded as a number
and those numbers may be several bytes large. The ASCII subset uses 1 byte per
character. Unicode was invented to be able to encode larger alphabets. Utf-8
is the most widely used. Windows uses utf-16. There are many variations. The
length of the string can be encoded either with a stop byte. C uses '\0' or
with a length prefix.

George Boole figured out a way to map logical statements in text to math. He
used just three operations: AND, OR and NOT and just {0, 1} as the operand
set. Actually we just need two of those, since AND can be expressed with OR
and NOT and OR can be expressed with AND and NOT. Other commonly used
operations are exclusive-or and implication, both of which can be derived from
the other operations. Claude Shannon figured out that digital logic could be
used for encoding the boolean algebra.  The logical operations OR, AND,
EXCLUSIVE-OR and NOT can be defined to operate on bit vectors. 

### Integer Representations
Integral data types has a width and a signedness. Some programming languages
(Java) do not provide unsigned types. Each architecture has a word size (32 or
64 bits) but we often want to operate on smaller or larger sizes of data.

The 

### Integer Arithmetic

### Floating Point
