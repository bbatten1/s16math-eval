/* vi:set ts=4: <-- vi tabstop
   testmul.c - measure time needed for a function with two parameters.
   Copyright (c) 2018 Bryan Batten <bjbatten@acm.org>
 */
#include <stdio.h>		/* putchar () & friends */
#include <stdlib.h>		/* atoi () & friends */
#include "mcc.h"		/* SYSTEM_Initialize () */
#include "s16math.h"

void timer0_start(void);
void timer0_stop(void);
u16  timer0_read(void);

#define EOT 0x04
#define BS	0x08
#define DEL	0x7f

static const char args[]="args> ";
//static const char format[]="%s+%s=%s,t=%u us\n";
//static const char format[]="%s-%s=%s,t=%u us\n";
static const char format[]="%s*%s=%s,t=%u us\n";
//static const char format[]="%s/%s=%s,t=%u us\n";
static const char toolong[]=" too long\n";
static const char overflow[]="overflow\n";

static char input[16];
static char abuf[12];
static char bbuf[12];
static char pbuf[12];

static s8 readline(void)
{
    s8 i,c;
	i=0;

	do {
cloop:	c=getchar();
		if (c==DEL) {	/* picocom maps BS to DEL & ignores DEL */
			if (i>0) {
				putchar(BS);
				putchar(' ');
				putchar(BS);
				i-=1;
			}
			goto cloop;
		}
	    putchar(c);
	    if (c=='\n') {	/* use preceding CR as EOL */
	        break;
	    }
	    input[i++]=c;
	} while (i<sizeof(input) && c!=EOT);
	if (c==EOT)
		putchar('\n');
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
/* "volatile" needed to avoid optimization side effect of Microchip xc16
   "-O1" compile option. */
static void formatqmn(char *d, volatile s16 x)
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
	sprintf(d,"%.*d",(int)(NWIDTH),qn);

} /* End formatqmn () */

int main(void)
{
	//Timer timer;
	char *ip, *q;
	u16 time;
	s8	len;
	qm_n p,a,b;

	SYSTEM_Initialize();

	do {
		printf(args);
		len=readline();
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

		timer0_start();		//timer.start();
//		p=a+b;
//		p=a-b;
//		p=a*b;
//		p=a/b;
		p=s16_mul(a,b);
//		p=s16_div(a,b);
		timer0_stop();		//timer.read_us();
		time = timer0_read();
		formatqmn(pbuf,p);

		printf(format,abuf,bbuf,pbuf,time);
//		if (s16_addov(p,a,b))
//		if (s16_subov(p,a,b))
		if (s16_mulov(p,a,b))
//		if (s16_divov(p,a,b))
			printf(overflow);
	} while (1);

	while (1){};

} /* End main () */
