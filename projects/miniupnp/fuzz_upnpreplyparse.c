#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "src/upnpreplyparse.h"

/*
 * Fuzz UPnP SOAP reply parsing.
 * UPnP SOAP responses from IGD routers are attacker-controlled in a
 * network-adjacent threat model — a rogue router can send crafted replies.
 */
int LLVMFuzzerTestOneInput(const uint8_t *in, size_t insize)
{
	struct NameValueParserData data;

	/* Parser needs null-terminated input */
	char *buf = malloc(insize + 1);
	if (!buf)
		return 0;
	memcpy(buf, in, insize);
	buf[insize] = '\0';

	ParseNameValue(buf, (int)insize, &data);
	ClearNameValueList(&data);

	free(buf);
	return 0;
}
