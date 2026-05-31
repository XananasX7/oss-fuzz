#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "src/igd_desc_parse.h"
#include "src/minixml.h"

/*
 * Fuzz IGD description parser — parses the XML device description returned
 * by UPnP IGD routers. This is fetched over HTTP from the router and is
 * fully attacker-controlled in a network-adjacent threat model.
 */
int LLVMFuzzerTestOneInput(const uint8_t *in, size_t insize)
{
	struct IGDdatas data;
	struct xmlparser parser;

	char *buf = malloc(insize + 1);
	if (!buf)
		return 0;
	memcpy(buf, in, insize);
	buf[insize] = '\0';

	memset(&data, 0, sizeof(data));
	memset(&parser, 0, sizeof(parser));

	parser.xmlstart = buf;
	parser.xmlsize = (int)insize;
	parser.data = &data;
	parser.starteltfunc = IGDstartelt;
	parser.endeltfunc = IGDendelt;
	parser.datafunc = IGDdata;
	parser.attfunc = NULL;

	parsexml(&parser);

	free(buf);
	return 0;
}
