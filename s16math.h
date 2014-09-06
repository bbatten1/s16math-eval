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
   and Q6.9 formats are used.

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
			Q6.9	-64.000  <-> 63.511

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

#ifndef SDCC			/* omit if SDCC <= 3.10 */
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
#define NMASK		(NSCALE-1)		/* fraction mask */
#define NWIDTH		3				/* for test fixture printf */
#define MMASK		(~NMASK)		/* integer mask */
/* radians */
#define RSHIFT		10				/* Qm.10 */
#define RSCALE		(1<<RSHIFT)		/* scaling factor is implicit denominator */
#define RMASK		(RSCALE-1)		/* fraction mask */
#define RWIDTH		4				/* for test fixture printf */
#define UF			NSHIFT
#define SCALE_INTEGRAL	(DSHIFT)		/* scaling factor */
#define SCALE_SINCOS	(NSHIFT-DSHIFT)	/* scaling factor */

#define fvalue(x)		(((x)&NMASK)<<UF)	/* fraction as integer value */
#define ivalue(x)		((x)&MMASK)			/* integer as integer value */
#define frac_part(x)	((x)&NMASK)			/* fractional value as integer */
#define int_part(x)		((x)>>UF)			/* integer value as integer */
#define prescale(x)		((s32)abs(x)<<UF)	/* for divide */
#define fixscale(x)		((x)>>UF)			/* for multiply */
#define scaledown(n,s)	((n)>>(s))
#define scaleup(n,s)	((n)<<(s))
#define dround(x)		(KD)				/* degree multiply */
#define nround(x)		(KN)				/* real number multiply */
#define rround(x)		(KR)				/* radian multiply */

#else /* (don't) USE_BINARY_POINT */

#define DSCALE		10				/* resolution 1/10, etc. */
#define DWIDTH		1				/* for test fixture printf */
#define NSCALE		100	
#define NWIDTH		2				/* for test fixture printf */
#define RSCALE		1000
#define RWIDTH		3				/* for test fixture printf */
#define UF			NSCALE
#define SCALE_INTEGRAL	(RSCALE/NSCALE)	/* scaling factor */
#define SCALE_SINCOS	(RSCALE/NSCALE)	/* scaling factor */

#define fvalue(x)		(((x)%UF)*UF)	/* fraction as integer value */
#define ivalue(x)		(((x)/UF)*UF)	/* integer as integer value */
#define frac_part(x)	((x)%UF)		/* fractional part as integer */
#define int_part(x)		((x)/UF)		/* integer part as integer */
#define prescale(x)		((s32)abs(x)*UF)
#define fixscale(x)		((x)/UF)
#define scaledown(n,s)	((n)/(s))
#define scaleup(n,s)	((n)*(s))
#define dround(x)		(isneg(x)?-KD:KD)/* degree multiply */
#define nround(x)		(isneg(x)?-KN:KN)/* real number multiply */
#define rround(x)		(isneg(x)?-KR:KR)/* radian multiply */
#endif /* (don't) USE_BINARY_POINT */

#ifdef NAN	/* needed for mbed compile */
#undef NAN
#endif
#define NAN			(-99*QN)
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
#define qm_rad	s16		/* Q6.9		Q2_3 */
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
/* TODO remove reference to WOF100, WOF16 */
#define	WOF100	328			/* would overflow or change sign if multiplied
							   by 100 */
#define	WOF16	32768u		/* result would overflow or change sign for
							   16 bit values */

/* reduce chances of name pollution */
/* 2.7182 rounded up to 2.72 */
/* "warning: integer overflow in expression", even though final result fits. */
#if defined(AVR) || defined(SDCC) && (UI==128)
#define S16_E		((qm_n)(348))
#else
#define	S16_E		((qm_n)(272*UI/100))
#endif
/* natural log of 10 ~ 2.3025851 */
#define S16_LN10	((qm_n)(230*UI/100))
/* 3.1416 rounded up to 3.142 - casts keep AVR, SDCC happy */
#define	S16_PI		((qm_rad)((s32)((s32)3142*RI/(s32)1000)))
#define	S16_RAD		(2*S16_PI)	/* 2*pi radians in the unit circle */
#define S16_DEG		(360*DI)	/* 360.0 degrees in the unit circle */

/* some conversion factors */
/* 180/pi ~ 57.3 deg in Q11.4 format per rad */
#define	DEG_PER_RAD	((573*DI)/10)
/* pi/180 ~ 0.017453.. rad in Q6.9 format per deg */
#define	RAD_PER_DEG	((0017*RI)/1000)

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
s32  s16_sincos(qm_rad phi);	/* cos 0:7; sin 8:15 Q1_2 */
qm_n s16_tan(qm_rad phi);		/* tangent given phi in radians */

s8 s16_quadrant(qm_rad phi);	/* quadrant of phi */
qm_n s16_cot(qm_rad phi);		/* 1/tan */
qm_n s16_csc(qm_rad phi);		/* 1/sin */
qm_n s16_sec(qm_rad phi);		/* 1/cos */

qm_rad s16_acos(qm_n cos);		/* arc cosine in radians */
qm_rad s16_asin(qm_n sin);		/* arc sine in radians */
qm_rad s16_atan(qm_n tan);		/* arc tangent in radians */
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
qm_n s16_pow(qm_n a,qm_n n);/* returns a^n */
s16 s16_reduce(qm_n k);			/* evaluate k/100=x/y for x and y and return x
								   in bits 8-15, y in bits 0-7. */
qm_n s16_root(qm_n a,qm_n n);	/* returns n'th root nearest to a^(1/n) */
#define	s16_sqrt(x)	s16_root((qm_n)(x),2*UI)
								/* fixed point principle square root */
qm_n s16_factorial(qm_n n);		/* returns n! */
s16 s16_scale(s16 n,s16 s);		/* Downscale number n by factor s. */

/* convert rad --> deg.
   rad is supplied as percentage - i.e. with four digits to the right of
   an implied decimal point.
   0 <= rad <= 628 - i.e. 2*pi, or 6.2832

   returns degrees as integer: 0 <= deg <= 360

   360 deg    x deg
   -------- = -----
   2*pi rad   n rad

           360 deg*n rad
   x deg = -------------
             2*pi rad
 */
qm_deg s16_todeg(qm_rad rad);	/* convert from radians to degrees */

/* Convert degrees to radians.
   degrees is supplied as integer: 0 <= deg <= 360

   returns radians as implicit rational number with four digits to the right
   of an implied decimal point.
   0 <= rad <= 2*pi, or 6.2831

   2*pi rad   x rad
   -------- = -----
   360 deg    n deg

           2*pi rad*n deg   pi rad*n deg
   x rad = -------------- = ------------
              360 deg         180 deg
 */
qm_rad s16_torad(qm_deg deg);	/* convert from degrees to radians */

/* overflow checks for a = b <op> c, where <op> is +, -, *, /. */
s16 s16_addov(qm_n a, qm_n b, qm_n c);
s16 s16_subov(qm_n a, qm_n b, qm_n c);
s16 s16_divov(qm_n a, qm_n b, qm_n c);
s16 s16_mulov(qm_n a, qm_n b, qm_n c);

#ifndef SDCC			/* omit if SDCC <= 3.10 */
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif
#endif /* S16MATH_H */
