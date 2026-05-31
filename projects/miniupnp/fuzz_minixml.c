#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "src/minixml.h"

/* Callback that collects element names and values */
static void xml_start_elt(void *data, const char *name, int namelen)
{
	(void)data; (void)name; (void)namelen;
}

static void xml_end_elt(void *data, const char *name, int namelen)
{
	(void)data; (void)name; (void)namelen;
}

static void xml_data(void *data, const char *d, int len)
{
	(void)data; (void)d; (void)len;
}

/* Fuzz the minixml SAX parser with arbitrary XML input. */
int LLVMFuzzerTestOneInput(const uint8_t *in, size_t insize)
{
	struct xmlparser parser;

	/* parsexml expects a null-terminated string */
	char *buf = malloc(insize + 1);
	if (!buf)
		return 0;
	memcpy(buf, in, insize);
	buf[insize] = '\0';

	memset(&parser, 0, sizeof(parser));
	parser.xmlstart = buf;
	parser.xmlsize = (int)insize;
	parser.data = NULL;
	parser.starteltfunc = xml_start_elt;
	parser.endeltfunc = xml_end_elt;
	parser.datafunc = xml_data;
	parser.attfunc = NULL;

	parsexml(&parser);

	free(buf);
	return 0;
}
