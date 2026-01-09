# LEB128 Compression Library

## Goal

Implement a library for **Little Endian Base 128 (LEB128)** encoding and decoding.

[LEB128](https://en.wikipedia.org/wiki/LEB128) is a variable-length compression algorithm used heavily in systems programming (DWARF debug info, WebAssembly, Android Dex files) to store arbitrarily large integers efficiently. Small numbers should take up fewer bytes than large numbers.

You must implement the logic in `lib.c` to compress an array of 64-bit integers into a stream of bytes, and conversely, decompress that stream back into integers.

---

## The LEB128 Algorithm

LEB128 represents integers using **7 bits per byte**. 
* **Bits 0-6**: Actual data.
* **Bit 7 (MSB)**: Continuation flag.
    * `1`: There is another byte following this one.
    * `0`: This is the last byte of the integer.

**Example:**
* Number: `128` (Binary `10000000`)
* **Group 1 (Least Significant 7 bits)**: `0000000`. More bits remain? Yes. -> Byte `0x80` (`1` + `0000000`).
* **Group 2 (Next 7 bits)**: `0000001`. More bits remain? No. -> Byte `0x01` (`0` + `0000001`).
* **Result**: `0x80 0x01`

---

## Core API

### Encoding
* **`leb128_encode`**: 
    * Takes an array of `uint64_t` values.
    * Encodes them sequentially into the `output` byte buffer.
    * Updates `output_size` with the total number of bytes written.

### Decoding
* **`leb128_decode`**: 
    * Takes a raw byte buffer `input` of size `input_size`.
    * Decodes the bytes back into `uint64_t` values.
    * Updates `values_count` with the number of integers successfully recovered.
    * Processing stops when `input_size` bytes have been consumed.

---

## Testing Your Code

The provided test suite covers:
* **Boundary Values**: 0, 127, 128, 16383, 16384.
* **Maximums**: Encoding `UINT64_MAX` (requires 10 bytes).
* **Sequences**: Encoding multiple integers into a single contiguous stream.
* **Round Trip**: Ensuring that `Decode(Encode(x)) == x`.

To run the tests:

```bash
make
./test
```

or

```bash
make run
```

You should see:

```text
* Suite suite_encoding:
..........
* Suite suite_decoding:
..........

20 tests - 20 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement the encoding and decoding logic here.

## Files Provided

* **`lib.h`**: Function prototypes and documentation.
* **`test.c`**: The unit testing suite (do not modify).
* **`Makefile`**: Build instructions.
