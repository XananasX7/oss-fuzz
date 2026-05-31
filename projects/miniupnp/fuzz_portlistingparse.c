#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "src/portlistingparse.h"

/*
 * Fuzz GetListOfPortMappings response parser.
 * Response XML comes from the router and is network-attacker-controllable.
 */
int LLVMFuzzerTestOneInput(const uint8_t *in, size_t insize)
{
	struct PortMappingParserData data;

	char *buf = malloc(insize + 1);
	if (!buf)
		return 0;
	memcpy(buf, in, insize);
	buf[insize] = '\0';

	memset(&data, 0, sizeof(data));
	ParsePortListing(buf, (int)insize, &data);
	FreePortListing(&data);

	free(buf);
	return 0;
}
