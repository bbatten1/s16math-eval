/* vi:set ts=4: <-- vi tabstop
   s16math.h
   Copyright (c) 2014 Bryan Batten <bjbatten@acm.org>
   See README for more details.
   
   Machine level integers are treated as virtual fixed point[1] numbers with
   sign, integer, and fractional parts specified in either decimal or
   Qm.n[2] format. The most significant bit is the sign bit, leaving 15 bits
   to be allocated between the integer and the fractional components. Numbers
   are a ratio of two integers: the numerator is kept in storage, the
   denominator is implied and is equal to either a power of 10 or a power of 2.

   In decimal format, the integer and fractional parts are given by the
   integer division of the whole number by the denominator: the integer
   portion is the quotient and the fractional part is the remainder. In this
   library, denominators of 10, 100, and 1000 are used.
   
   In Qm.n format 'm' is the number of bits in the integer portion of the
   number, and 'n' is the number of bits in the fractional part. The
   denominator is a constant equal to 2^n[2]. In this library, Q11.4, Q8.7,
   and Q5.10 formats are used.

   The number pi is handled as having the equivalent of approximately three
   decimal digits of resolution - 3.142. To handle the needed range of
   degree units, a Qm.n format equivalent to approximately a single decimal
   digit of resolution is used - 360.0.

   Numbers are displayed with a point (".") format to separate the integer
   and fractional parts. The fractional part is the numerator of a fraction
   of which the denominator is the chosen decimal or binary resolution.
   
   The following table summarizes number formats used in s16math:

    Type	Format  		Range		 Notes
    -----   ------	-------------------- -----
	Degrees	Dec  1	-3276.8  <-> 3276.7	 Otherwise would be max of 327.67
			Q11.4	-2048.00 <-> 2047.15
	Real	Dec  2	-327.68  <-> 327.67
			Q8.7	-256.000 <-> 255.127
	Radians	Dec  3	-32.768  <-> 32.767	 Otherwise would be max of 327.67
			Q5.10	-64.000  <-> 63.511

	Observe the following possible gotchas:

	Decimal	   Q8.7		because
	-------    ----		-------
	  2.5   != 2.5		2 50/100 != 2 50/128
	  2.5   == 2.64		2 50/100 == 2 64/128

   [1] Fixed-point arithmetic
       http://en.wikipedia.org/wiki/Fixed-point_arithmetic

   [2] Q (number format)
       http://en.wikipedia.org/wiki/Q_%28number_format%29
 */
#ifndef S16MATH_H
#define S16MATH_H
#include <stdint.h>	/* for int8_t and friends */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* omit if SDCC <= 3.10 or we have a Keil compiler */
#if !defined(SDCC) && !defined(__C51__) && !defined(__CX51__)
#pragma pack(push,1)
#endif

/* our storage units */
typedef int8_t		s8;
typedef uint8_t		u8;
typedef int16_t		s16;
typedef uint16_t	u16;
typedef int32_t		s32;
typedef uint32_t	u32;

#ifdef USE_BINARY_POINT
/* degrees */
#define DSHIFT		4				/* Qm.4 */
#define DSCALE		(1<<DSHIFT)		/* scaling factor is implicit denominator */
#define DMASK		(DSCALE-1)		/* fraction mask */
#define DWIDTH		2				/* for test fixture printf */
/* numbers other than radians */
#define NSHIFT		7				/* Qm.7 */
#define NSCALE		(1<<NSHIFT)		/* scaling factor is implicit denominator */
#define NMASK		((NSCALE)-1)	/* fraction mask */
#define NWIDTH		3				/* for test fixture printf */
#define MMASK		(~NMASK)		/* integer fraction mask */
/* rounding field */
#define FSHIFT		3
#define FSCALE		(1<<FSHIFT)
#define FMASK		(FSCALE-1)
/* radians */
#define CRSHIFT		13				/* Q2.13 - CORDIC radian */
#define RSHIFT		10				/* Q5.10 */
#define RSCALE		(1<<RSHIFT)		/* scaling factor is implicit denominator */
#define CRSCALE		(1<<CRSHIFT)	/* scaling factor is implicit denominator */
#define RMASK		(RSCALE-1)		/* rad fraction mask */
#define CRMASK		(CRSCALE-1)		/* cordic rad fraction mask */
#define RWIDTH		4				/* for test fixture printf */
#define RF			RSHIFT			/* unit radian factor */
#define UF			NSHIFT			/* unit integer factor */
#define SCALE_INTEGRAL	(DSHIFT)	/* scaling factor */
#define SCALE_RAD	(RSHIFT-NSHIFT)	/* scaling factor num <-> rad */
#define SCALE_CORA	(CRSHIFT-RSHIFT)/* scaling factor cordic rad <-> rad */
#define SCALE_CORAN	(CRSHIFT-NSHIFT)/* scaling factor cordic rad <-> num */
#define MIN_ATAN_FRACT	584			/* 584/1024 approx. 570/1000 */

/* CORDIC table constants */
/* 3141593/10^6*2^13 */
#define NUM_CORDIC_ENTRIES	14		/* max num of non-zero entries */
#define S16_CPI		25736			/* 3141593/10^6*2^13 rounded */
#define CORDIC_LC	((qm_cora)4974)	/* 6072/10^4=x/2^13 */
#define CORDIC_K	((qm_cora)13491)/* magnitude scaling 16468/10^4=x/2^13 */

#define fvalue(x)		(((x)&NMASK)<<UF)	/* fraction as integer value */
#define ivalue(x)		((x)&MMASK)			/* integer as integer value */
#define frac_part(x)	((x)&NMASK)			/* fractional value as integer */
#define int_part(x)		((x)>>UF)			/* integer value as integer */
#define lsdigit(x)		((x)&FMASK)			/* least significant digit */
#define oddfloor(x)		((x)&FSCALE)		/* rounding floor is odd */
#define is_odd(x)		(((x)&(UI|NMASK))!=0)
#define prescale(x)		((s32)abs(x)<<UF)	/* for divide */
#define prerscale(x)	((s32)abs(x)<<RF)	/* for radian divide */
#define fixscale(x)		((x)>>UF)			/* for multiply */
#define fixrscale(x)	((x)>>RF)			/* for radian multiply */
#define scaledown(n,s)	((n)>>(s))
#define scaleup(n,s)	((n)<<(s))
#define dround(x)	(isneg(x)?-((abs(x)-KD)):(x)+KD)/* degree multiply */
#define nround(x)	(isneg(x)?-((abs(x)-KN)):(x)+KN)/* real number multiply */
#define rround(x)	(isneg(x)?-((abs(x)-KR)):(x)+KR)/* radian multiply */

#else /* (don't) USE_BINARY_POINT */
/* degrees/digits */
#define DSCALE		10				/* resolution 1/10, etc. */
#define DWIDTH		1				/* for test fixture printf */
/* numbers other than radians */
#define NSCALE		100	
#define NWIDTH		2				/* for test fixture printf */
/* rounding field */
#define FSCALE		DSCALE			/* rounding field */
/* radians */
#define RSCALE		1000
#define CRSCALE		10000
#define RWIDTH		3				/* for test fixture printf */
#define RF			RSCALE			/* unit radian factor */
#define UF			NSCALE			/* unit integer factor */
#define SCALE_INTEGRAL	(RSCALE/NSCALE)	/* scaling factor */
#define SCALE_RAD	(RSCALE/NSCALE)		/* scaling factor num <-> rad */
#define SCALE_CORA	(CRSCALE/RSCALE)	/* scaling factor cordic rad <-> rad */
#define SCALE_CORAN	(CRSCALE/NSCALE)	/* scaling factor cordic rad <-> num */
#define MIN_ATAN_FRACT	570			/* 570/1000 approx. 584/1024 */

/* CORDIC table constants */
/* 3141593/10^6*10^4 */
#define NUM_CORDIC_ENTRIES	15		/* max num of non-zero entries */
#define S16_CPI		31416			/* 3141593/10^6*10^4 rounded */
#define CORDIC_LC	((qm_cora)6072)	/* collective length correction 0.6072 */
#define CORDIC_K	((qm_cora)16468)/* magnitude scaling 1.6468 */

#define fvalue(x)		(((x)%UF)*UF)	/* fraction as integer value */
#define ivalue(x)		(((x)/UF)*UF)	/* integer as integer value */
#define frac_part(x)	((x)%UF)		/* fractional part as integer */
#define int_part(x)		((x)/UF)		/* integer part as integer */
#define lsdigit(x)		((x)%FSCALE)	/* least significant digit */
#define oddfloor(x)		(((x)/FSCALE)&1)/* rounding floor is odd */
#define is_odd(x)		(((x)%(2*UI))!=0)
#define prescale(x)		((s32)abs(x)*UF)
#define prerscale(x)	((s32)abs(x)*RF)
#define fixscale(x)		((x)/UF)
#define fixrscale(x)	((x)/RF)
#define scaledown(n,s)	((n)/(s))
#define scaleup(n,s)	((n)*(s))
#define dround(x)		((x)+(isneg(x)?-KD:KD))/* degree multiply */
#define nround(x)		((x)+(isneg(x)?-KN:KN))/* real number multiply */
#define rround(x)		((x)+(isneg(x)?-KR:KR))/* radian multiply */
#endif /* (don't) USE_BINARY_POINT */

#define S16_NAN		(-99*QN)
#define QD			DSCALE
#define QN			NSCALE
#define QR			RSCALE
#define KD   		(QD>>1)
#define KN   		(QN>>1)
#define KR   		(QR>>1)

#define DI			QD			/* degree unit integer */
#define UI			QN			/* number unit integer */
#define RI			QR			/* radian unit integer */

#define dbl(n)		((n)<<1)
#define half(n)		(((n)+1)>>1)

/* Q formats 			   Binary   Decimal */
#define qm_n	s16		/* Q8.7		Q3_2 */
#define qm_deg	s16		/* Q11.4	Q4_1 */
#define qm_rad	s16		/* Q5.10	Q2_3 */
#define qm_cora	s16		/* Q2.13 	Q1_4 cordic angle table entry precision */
#define q1_6	s8		/* Q1.6 - +- 1.63 byte wide sin/cos values */
#define Q1_2	s8		/* Q1.2 - +- 2.55: byte wide sin/cos values */
#define	Q0_5	s16		/* Q0.5 - fraction only */
#define	Q1_4	s16		/* Q1.4 - precision to four decimal places */
#define	Q5_0	s16		/* Q5.0 - integer only */

#ifndef abs			/* avr-gcc (maybe others) defines this in it's stdlib.h */
#define	abs(x)		((x)<0?-(x):(x))
#endif
#define	isneg(x)	((x)<0)
#define	min(x,y)	((x)<(y)?(x):(y))
#define	max(x,y)	((x)>(y)?(x):(y))

/* convert from decimal point to binary point & vice versa */
#define to_binary_pt(x)		(((x)*100+64)/128)	/* cf. [2] */
#define to_decimal_pt(x)	(((x)*128+50)/100)

/* some limits */
/* TODO remove reference to WOF100 */
#define	WOF100	328			/* would overflow or change sign if multiplied
							   by 100 */
#define	MOD16	32768u		/* signed modulus for 16 bit values */

/* reduce chances of name pollution */
/* 2.7182 rounded up to 2.72 */
/* "warning: integer overflow in expression", even though final result fits. */
#if defined(AVR) || defined(SDCC) || defined(XC16) && (UI==128)
#define S16_E		((qm_n)(348))
#else
#define	S16_E		((qm_n)(272*UI/100))
#endif
/* natural log of 10 ~ 2.3025851
   About 4% of results using the mathematically "best" expansion of the
   S16_LN10 macro differ by 0.01 or more from those of the reference
   implementation when compiled with binary resolution, so we fudge it. */
#if (UI==128)
#define S16_LN10	((qm_n)((230*UI/100)+1))
#else
#define S16_LN10	((qm_n)(230*UI/100))
#endif
/* 3.1416 rounded up to 3.142 - casts keep AVR, SDCC happy */
#define	S16_PI		((qm_rad)((s32)((s32)3142*RI/(s32)1000)))
#define	S16_RAD		(2*S16_PI)	/* 2*pi radians in the unit circle */
#define S16_DEG		(360*DI)	/* 360.0 degrees in the unit circle */

/* atan(-327.38), atan(-256) */
/* #define MIN_ATAN	((qm_rad)(-(RI+(570*RI)/1000)))
   can't be used because of limitations on intermediate results in the sdcc,
   avr, and xc16 compilers. */
#define MIN_ATAN	((qm_rad)(-(RI+MIN_ATAN_FRACT)))

/* some conversion factors */
/* 180/pi ~ 57.3 deg in Q11.4 format per rad */
#define	DEG_PER_RAD	((573*DI)/10)
/* pi/180 ~ 0.017453.. rad in Q5.10 format per deg */
#define	RAD_PER_DEG	((0017*RI)/1000)

extern const qm_cora s16_cordictab[NUM_CORDIC_ENTRIES];
/* ^^^ needed to keep microchip xc16-gcc from complaining about
   "multiple definition of `_s16_cordictab'" */
/*
   Polar coordinates
   cf. Trigonometric functions
   http://en.wikipedia.org/wiki/Trigonometric_functions
   Function                        Description */
qm_n s16_cos(qm_rad phi);		/* cosine given phi in radians */
qm_n s16_sin(qm_rad phi);		/* sine given phi in radians */
typedef union {
	struct {
		qm_n	cos;
		qm_n	sin;
	}s;
	s32		sincos;
} sincos_t;
s32  s16_sincos(qm_rad phi);	/* cos 0:15; sin 15:31 qm_n */
qm_n s16_tan(qm_rad phi);		/* tangent given phi in radians */

s8 s16_quadrant(qm_rad phi);	/* quadrant of phi */
#define s16_unwind(phi)	((phi)%(2*S16_PI))	/* unwind phi into unit circle */
qm_n s16_cot(qm_rad phi);		/* 1/tan */
qm_n s16_csc(qm_rad phi);		/* 1/sin */
qm_n s16_sec(qm_rad phi);		/* 1/cos */

qm_rad s16_acos(qm_n cos);		/* arc cosine in radians */
qm_rad s16_asin(qm_n sin);		/* arc sine in radians */
qm_rad s16_atan(qm_n tan);		/* arc tangent in radians */
qm_rad s16_atan2(qm_n y,qm_n x);/* arc tangent in radians from y/x */
qm_n s16_hypot(qm_n x,qm_n y);	/* hypoteneuse */
qm_n s16_ln(qm_n n);			/* power to which e would have to be
								   raised to equal n. */
qm_n s16_log(qm_n a,qm_n n);	/* power to which a would have to be
								   raised to equal n. */
qm_n s16_log10(qm_n n);			/* power to which 10 would have to be raised
								   to equal n. */
u16 s16_gcd(u16 u, u16 v);		/* returns greatest common divisor u,v */
qm_n s16_den(qm_n pwr);			/* Integer denominator of Q3.2 fraction */
qm_n s16_div(qm_n a, qm_n b);	/* Q3.2 quotient a3.2/b3.2 */
qm_n s16_mul(qm_n a, qm_n b);	/* Q3.2 product a3.2*b3.2 */
qm_n s16_pow(qm_n a,qm_n n);	/* returns a^n */
s16 s16_reduce(qm_n k);			/* evaluate k/100=x/y for x and y and return x
								   in bits 8-15, y in bits 0-7. */
qm_n s16_round(qm_n x);			/* round to nearest, ties to even */
qm_n s16_root(qm_n a,qm_n n);	/* returns n'th root nearest to a^(1/n) */
#define s16_sqr(a) (s16_mul(a,a))/* returns a^2 */
qm_n s16_sqrt(qm_n a);			/* fixed point principle square root */
qm_n s16_fact(qm_n n);			/* returns n! */

/* convert rad --> deg.
   rad is supplied as implicit rational number with three digits to the right
   of an implied decimal point.
   0 <= rad <= 6283 - i.e. 2*pi, or 6.283

   returns degrees as implicit rational number: 0.0 <= deg <= 360.0

   360 deg    x deg
   -------- = -----
   2*pi rad   n rad

           360 deg*n rad
   x deg = -------------
             2*pi rad
 */
qm_deg s16_todeg(qm_rad rad);	/* convert from radians to degrees */

/* Convert degrees to radians.
   Convert  degrees  with one decimal digit - or 4 bits - of resolution to
   radians with three digits - or 10 bits - of resolution: 0.0 <= deg <= 360.0

   returns radians as implicit rational number with three digits to the right
   of an implied decimal point.
   0 <= rad <= 2*pi, or 6.283

   2*pi rad   x rad
   -------- = -----
   360 deg    n deg

           2*pi rad*n deg   pi rad*n deg
   x rad = -------------- = ------------
              360 deg         180 deg
 */
qm_rad s16_torad(qm_deg deg);	/* convert from degrees to radians */

/* overflow checks for a = b <op> c, where <op> is +, -, *, /, power, root. */
s16 s16_addov(qm_n a, qm_n b, qm_n c);
#define s16_logov(a)	((a)==S16_NAN)			/* ln, log, log10 overflow check */
s16 s16_subov(qm_n a, qm_n b, qm_n c);
s16 s16_divov(qm_n a, qm_n b, qm_n c);
s16 s16_mulov(qm_n a, qm_n b, qm_n c);
s16 s16_powov(qm_n a, qm_n b, qm_n c);
s16 s16_rootov(qm_n a, qm_n b, qm_n c);
#define s16_sqrov(a)	(isneg(a))			/* squaring overflow check */
#define s16_sqrtov(a,b)	(s16_rootov(a,b,2*UI))/* square root overflow check */

/* omit if SDCC <= 3.10 or we have a Keil compiler */
#if !defined(SDCC) && !defined(__C51__) && !defined(__CX51__)
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif
#endif /* S16MATH_H */
