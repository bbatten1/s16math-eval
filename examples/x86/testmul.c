/* vi:set ts=4: <-- vi tabstop
   testmul.c - call fixed point multiply function and check for overflow.
   Copyright (c) 2014 Bryan Batten <bjbatten@acm.org>
 */
#include <stdio.h>		/* for printf */
#include <stdlib.h>		/* for atoi () */
#include <getopt.h>		/* for getopt () */
#include <time.h>		/* for clock_gettime () */
#include <string.h>		/* for GNU strrchr () */
#include "s16math.h"	/* for 16 bit fixed point math library functions */

/* set program name */
#define	setpn(p)							\
		if (!((p) = strrchr(argv[0], '/')))	\
			(p) = argv[0];					\
		else (p)++

static char *pname;

static void usage ()
{
	fprintf(stderr, "Usage:\t\"%s[ -t][ --] multiplicand multiplier where:\n"
		"\t-t gets execution time of multiply function.\n"
		"\t-- stops option scan. Allows following negative numbers.\n"
		"\tSpecify parameters as 16 bit decimal integers.\n"
		, pname);
}

/* Format Qm.2 number */
static void formatqm2(char *d, s16 x)
{
	s16 qm,qn;

	if (isneg(x)) {
		x=-x;
		/* happens when x == 32768: avoid two minus signs. */
		if (!isneg(x)) *d++='-';
	}
	qm=x/QN;
	qn=abs(x%QN);

	d+=sprintf(d,"%.1d",qm);
	*d++='.';
	sprintf(d,"%.*d",NWIDTH,qn);

} /* End formatqm2 () */

int main(int argc, char *argv[]) {
	int	chr,t=0;
	struct timespec tb, te;
	qm_n a, b;		/* multiplicand, multiplier */
	qm_n p;			/* product */
	char abuf[8];
	char bbuf[8];
	char pbuf[8];
	char tbuf[24];

	setpn(pname);
	tbuf[0]=0;

	/* Get program options. */
	while ((chr = getopt( argc, argv, "th?")) != EOF ) {
		switch (chr) {
		case 't':
			t='t';
			break;
		case 'h':
		case '?':
			usage ();
			exit (0);
		default :
			fprintf( stderr, "%s: Unknown option: '%c'\n", pname, chr );
			usage ();
			exit (1);
		}
	} /* End option scan */

	if (argc - optind < 2) {
		fprintf(stderr, "need integer values multiplicand and multiplier\n");
		return -1;
	}
	a=atoi(argv[optind++]);
	formatqm2(abuf,a);
	b=atoi(argv[optind]);
	formatqm2(bbuf,b);
	if (t=='t') {
		clock_getres(CLOCK_THREAD_CPUTIME_ID, &tb);
		te=tb;
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tb);
	}
	p=s16_mul(a,b);
	if (t=='t') {
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &te);
		sprintf(tbuf, ",t=%ld.%09ld sec",
				te.tv_sec-tb.tv_sec,te.tv_nsec - tb.tv_nsec);
	}
	formatqm2(pbuf,p);
	printf("%s*%s=%s%s\n",abuf,bbuf,pbuf,tbuf);
	if (s16_mulov(p,a,b))
		printf("overflow\n");
	return 0;

} /* End main () */
