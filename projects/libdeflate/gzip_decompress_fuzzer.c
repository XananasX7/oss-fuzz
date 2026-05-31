#include <libdeflate.h>
#include <stdint.h>
#include <stdlib.h>

/* Fuzz gzip decompression. */
int LLVMFuzzerTestOneInput(const uint8_t *in, size_t insize)
{
	size_t outsize_avail = 3 * insize + 4096;
	uint8_t *out;
	struct libdeflate_decompressor *d;

	out = malloc(outsize_avail);
	if (!out)
		return 0;

	d = libdeflate_alloc_decompressor();
	if (d) {
		libdeflate_gzip_decompress(d, in, insize, out, outsize_avail, NULL);
		libdeflate_free_decompressor(d);
	}
	free(out);
	return 0;
}
