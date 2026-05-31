#include <libdeflate.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Fuzz adler32 and crc32 checksum computation.
 * These are incremental APIs that should handle any input cleanly.
 */
int LLVMFuzzerTestOneInput(const uint8_t *in, size_t insize)
{
	uint32_t adler, crc;

	/* Test adler32 incremental update */
	adler = libdeflate_adler32(1, in, insize);
	/* Test split — first half then second half */
	if (insize >= 2) {
		adler = libdeflate_adler32(1, in, insize / 2);
		adler = libdeflate_adler32(adler, in + insize / 2,
					   insize - insize / 2);
	}

	/* Test crc32 incremental update */
	crc = libdeflate_crc32(0, in, insize);
	if (insize >= 2) {
		crc = libdeflate_crc32(0, in, insize / 2);
		crc = libdeflate_crc32(crc, in + insize / 2,
				       insize - insize / 2);
	}

	(void)adler;
	(void)crc;
	return 0;
}
