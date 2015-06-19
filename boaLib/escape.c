/*

 *
 */

/* $Id: escape.c,v 1.7 2002/03/22 04:24:09 jnelson Exp $ */


#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#else
#include "boa.h"
#endif

#include "escape.h"

unsigned long _needs_escape[(NEEDS_ESCAPE_BITS+NEEDS_ESCAPE_WORD_LENGTH-1)/NEEDS_ESCAPE_WORD_LENGTH];

void build_needs_escape(void)
{
	unsigned int a, b;
	const unsigned char special[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"-_.!~*'():@&=+$,/?";
	/* 21 Mar 2002 - jnelson - confirm with Apache 1.3.23 that '?'
	 * is safe to leave unescaped.
	 */
	unsigned short i, j;

	b = 1;
	for (a=0; b!=0; a++) b=b<<1;
	/* I found $a bit positions available in an unsigned long. */
	if (a < NEEDS_ESCAPE_WORD_LENGTH) {
		fprintf(stderr,
				"NEEDS_ESCAPE_SHIFT configuration error -- "\
				"%d should be <= log2(%d)\n",
				NEEDS_ESCAPE_SHIFT, a);
		exit(1);
	} else if (a >= 2*NEEDS_ESCAPE_WORD_LENGTH) {
		/* needs_escape_shift configuration suboptimal */
	} else {
		/* Ahh, just right! */;
	}
	memset(_needs_escape, ~0, sizeof(_needs_escape));
	for(i = 0; i < sizeof(special) - 1; ++i) {
		j=special[i];
		if (j>=NEEDS_ESCAPE_BITS) {
			/* warning: character $j will be needlessly escaped. */
		} else {
			_needs_escape[NEEDS_ESCAPE_INDEX(j)]&=~NEEDS_ESCAPE_MASK(j);
		}
	}
}

#ifdef TEST
int main(void)
{
	int i;
	build_needs_escape();
	for(i = 0; i <= NEEDS_ESCAPE_BITS; ++i) {
		if (needs_escape(i)) {
			fprintf(stdout, "%3d needs escape.\n", i);
		}
	}
	return(0);
}
#endif

