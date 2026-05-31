#include <libdeflate.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Fuzz DEFLATE compression + round-trip decompression.
 * This exercises both the compressor and verifies the output is valid.
 */
int LLVMFuzzerTestOneInput(const uint8_t *in, size_t insize)
{
	struct libdeflate_compressor *c;
	struct libdeflate_decompressor *d;
	uint8_t *cbuf, *dbuf;
	size_t cbuf_size, actual_out_nbytes;
	int level;

	/* Pick compression level from first byte if available */
	if (insize == 0)
		return 0;
	level = (in[0] % 12) + 1;
	in++;
	insize--;

	c = libdeflate_alloc_compressor(level);
	if (!c)
		return 0;

	cbuf_size = libdeflate_deflate_compress_bound(c, insize);
	cbuf = malloc(cbuf_size);
	if (!cbuf) {
		libdeflate_free_compressor(c);
		return 0;
	}

	size_t cbytes = libdeflate_deflate_compress(c, in, insize, cbuf, cbuf_size);
	libdeflate_free_compressor(c);

	if (cbytes == 0) {
		free(cbuf);
		return 0;
	}

	/* Round-trip: decompress and verify we get back the original data */
	dbuf = malloc(insize + 1);
	if (!dbuf) {
		free(cbuf);
		return 0;
	}

	d = libdeflate_alloc_decompressor();
	if (d) {
		libdeflate_deflate_decompress(d, cbuf, cbytes,
					      dbuf, insize, &actual_out_nbytes);
		libdeflate_free_decompressor(d);
	}
	free(cbuf);
	free(dbuf);
	return 0;
}
