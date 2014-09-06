/* vi:set ts=4: <-- vi tabstop
   main.cpp - call fixed point multiply function and check for overflow.
   Copyright (c) 2014 Bryan Batten <bjbatten@acm.org>
 */
#include "mbed.h"
#include "s16math.h"

#define EOT 0x04
#define BS	0x08
#define DEL	0x7f

static const char args[]={"args> "};
static const char toolong[]={"too long\n"};

static s8 readline(char *input, s8 len)
{
	int c;
	s8 i=0;

	do {
cloop:	c=getc(stdin);
		if (c==DEL) {	/* picocom maps BS to DEL & ignores DEL */
			if (i>0) {
				putc(BS,stdout);
				putc(' ',stdout);
				putc(BS,stdout);
				input-=1;
				i-=1;
			}
			goto cloop;
		}
	    putc(c,stdout);
	    if (c=='\n')	/* use preceeding CR as EOL */
	        break;
	    *input++=c;
	} while (++i<len && c!=EOT);
	if (c==EOT)
		putc('\n',stdout);
	return i;

} /* End readline () */

static char *skipws(char *p, char *q)
{
	while (*p <'!' && p < q) {
		if (*p == EOT || *p == '\r')
			break;
		p+=1;
	}
	return p;

} /* End skipws () */

static char *skipnonws(char *p, char *q)
{
	while (*p >' ' && p < q)
		p+=1;
	return p;

} /* End skipnonws () */

/* Format Qm.n number */
static void formatqmn(char *d, s16 x)
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

} /* End formatqmn () */

int main()
{
	Timer timer;
	char input[16];
	char *ip,*q;
	s8	len;
	int begin,end;
	qm_n p,a,b;
	char abuf[9];
	char bbuf[9];
	char pbuf[9];

	do {
		printf(args);
		len=readline(input, sizeof(input));
		if (!(len < (s8)(sizeof(input)))) {
			printf(toolong);
			continue;
		}
		q=input+len;
		ip=skipws(input,q);
		if (*ip==EOT)
			break;
		if (*ip=='\r')
			continue;
		a=atoi(ip);
		formatqmn(abuf,a);
		ip=skipnonws(ip,q);
		ip=skipws(ip,q);
		if (*ip=='\r')
			continue;
		b=atoi(ip);
		formatqmn(bbuf,b);

		timer.start();
		begin = timer.read_us();
		p=s16_mul(a,b);
		end = timer.read_us();
		formatqmn(pbuf,p);

		printf("%s*%s=%s,t=%d us\n",abuf,bbuf,pbuf,end-begin);
		if (s16_mulov(p,a,b))
			printf("overflow\n");
	} while (1);

	return 0;

} /* End main () */
