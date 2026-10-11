# FileCompressor — Technical Documentation

## 1. Purpose

FileCompressor is a C++17 command-line program for lossless file compression using Huffman coding. The decoder reconstructs the original byte stream without changing its contents.

## 2. Component responsibilities

| Component | Responsibility |
|---|---|
| `FrequencyTable` | Counts the frequency of each byte (0–255) and tracks the input size. |
| `HuffmanNode` | Represents a leaf or internal node in the Huffman tree. |
| `HuffmanTree` | Builds the tree with a min-priority queue and produces byte-to-code mappings. |
| `BitWriter` | Packs individual code bits into output bytes, most-significant bit first. |
| `BitReader` | Reads packed bits in the same order used by the writer. |
| `Encoder` | Reads the source and writes its Huffman code sequence. |
| `FileHeader` | Serializes/deserializes format metadata and validates header information. |
| `Compressor` | Coordinates frequency counting, tree construction, header writing, and encoding. |
| `Decoder` | Traverses the Huffman tree from encoded bits and emits original bytes. |
| `Decompressor` | Reads the header, rebuilds the tree, selects the decoding path, and writes the restored file. |
| `main.cpp` | Parses the command-line operation and input/output paths. |
| `benchmark.cpp` | Measures elapsed time and output size and checks restored data. |

## 3. Huffman coding

Huffman coding is a prefix-code algorithm. No assigned code is a prefix of another assigned code, so the decoder can identify symbols unambiguously by traversing the tree bit by bit.

### Tree construction

1. Create a leaf for every byte with a non-zero frequency.
2. Insert the leaves into a min-priority queue.
3. Remove the two nodes with the smallest frequencies.
4. Create a parent whose frequency is the sum of the two child frequencies.
5. Insert the parent back into the queue.
6. Repeat until one root remains.

A deterministic tie-breaker is used for equal frequencies so the same frequency table produces a consistent tree.

For a tree with one unique byte value, the implementation assigns a one-bit code (`0`) to that symbol. The decoder must handle this case explicitly because there are no branch decisions in a one-leaf tree.

For an empty input, the original size is zero and the decoder should produce an empty output without attempting to decode a symbol.

## 4. Bit-level I/O

Huffman codes are sequences of bits, not necessarily whole bytes. `BitWriter` accumulates bits in a byte buffer and writes the byte once it is full. The implementation writes bits from most-significant to least-significant position.

If the final byte is only partially filled, the remaining positions are padded with zero bits. The stored original file size tells the decoder how many output bytes to produce, so padding bits are not interpreted as extra source data.

`BitReader` uses the same bit order. A failed read before the expected number of output bytes have been reconstructed should be treated as a truncated or invalid compressed file.

## 5. File format

### HUF1 (legacy format)

The original format has a fixed 2,060-byte header:

| Field | Size |
|---|---:|
| Magic bytes (`HUF1`) | 4 bytes |
| Original file size | 8 bytes |
| Frequency of each of 256 byte values | 256 × 8 = 2,048 bytes |
| **Total** | **2,060 bytes** |

The frequency table lets the decompressor rebuild the Huffman tree without storing every code separately.

### HUF2 (newer format under verification)

The newer format adds a one-byte mode field, making its current header 2,061 bytes:

| Field | Size |
|---|---:|
| Magic bytes (`HUF2`) | 4 bytes |
| Mode | 1 byte |
| Original file size | 8 bytes |
| Frequency table | 2,048 bytes |
| **Total** | **2,061 bytes** |

Current intended modes:

- `0`: raw payload; frequency entries are zero-filled.
- `1`: Huffman-coded payload; frequencies rebuild the Huffman tree.

**Important:** The raw-mode path has shown a byte mismatch in a random-binary round-trip test. Treat HUF2 raw mode as unverified until a fresh rebuild and repeated hash/byte comparisons pass. Do not publish a claim that all modes pass correctness tests until this is resolved.

### Portability note

The current implementation writes `uint64_t` values using their in-memory representation. This is straightforward on the tested machine but does not define a portable endianness. A future format revision should serialize integers in a specified byte order and version that behavior.

## 6. Compression pipeline

1. Validate that input and output paths differ.
2. Build a frequency table from the input file.
3. Build the Huffman tree and code map.
4. Write format metadata.
5. Encode the source bytes using the code map.
6. Flush the final partial byte.
7. For the HUF2 implementation, compare the Huffman output size with the expected raw-mode size and choose a mode.

The size-based fallback is intended to avoid Huffman encoding when it does not reduce payload size. Because raw mode currently retains the full fixed header, it may still produce an output larger than the source.

## 7. Decompression pipeline

1. Open the compressed file in binary mode.
2. Validate the magic bytes and read the mode and original size.
3. Read and validate frequencies for Huffman mode.
4. Rebuild the Huffman tree.
5. Decode bits and emit bytes until the original file size is reached; for raw mode, copy exactly the declared number of bytes.
6. Check stream status and reject truncated data.

For defensive handling, decompression should also guard against output paths that overwrite the input, invalid modes, inconsistent frequency totals, incomplete headers, and truncated payloads.

## 8. Complexity analysis

Let:
- `n` be the number of input bytes,
- `k` be the number of distinct byte values, where `k ≤ 256`,
- `b` be the number of encoded bits.

### Frequency counting

Every input byte is read once:

- Time: `O(n)`
- Extra space: `O(256) = O(1)` for the fixed byte-frequency table.

### Huffman tree construction

There are at most 256 leaves. Building and combining nodes with a min-priority queue takes `O(k log k)` time and `O(k)` tree space. Since `k ≤ 256`, this part is bounded by a small constant for byte-oriented input.

### Encoding

Each source byte is looked up in the code map and its code bits are written:

- Time: `O(n + b)` when accounting for emitted bits and I/O.
- Additional memory: code table plus a small bit buffer, excluding the tree.

### Decoding

Each encoded bit follows a tree edge, and each decoded symbol is emitted:

- Time: `O(b + n)`.
- Additional memory: tree plus a small bit buffer, excluding I/O buffers.

### Overall

The file passes are linear in input/output size. The fixed alphabet makes tree construction negligible compared with reading and writing large files. Real elapsed time depends on storage speed, buffering, input entropy, and implementation details.

## 9. Benchmark observations

Recorded local benchmark results:

| Input size | Compressed size | Ratio | Saving | Compress | Decompress |
|---:|---:|---:|---:|---:|---:|
| 100 MiB | 59,712,585 B | 56.95% | 43.05% | 11.09 s | 5.85 s |
| ~300 MiB | 191,185,490 B | 60.77% | 39.23% | 40.29 s | 24.33 s |
| ~1 GiB | 653,627,120 B | 60.87% | 39.13% | 107.39 s | 61.72 s |

These figures describe the tested files and machine only. They should not be presented as expected performance for every kind of input.

## 10. Release validation checklist

- [ ] Rebuild the CLI from a clean state.
- [ ] Test empty, one-symbol, text, all-byte-values, random-binary, and large inputs.
- [ ] Compare every restored file with its source using `fc.exe /b` or SHA-256 hashes.
- [ ] Confirm malformed and truncated files fail with a useful error instead of silently producing output.
- [ ] Confirm input and output paths cannot be identical.
- [ ] Resolve the HUF2 raw-mode mismatch before marking HUF2 as stable.
- [ ] Ensure generated binaries, temporary files, test data, and generated output are ignored by Git.
- [ ] Update the README to reflect the actual tested behavior.
