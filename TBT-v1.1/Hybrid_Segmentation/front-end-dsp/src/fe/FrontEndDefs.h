/*-------------------------------------------------------------------------
 *  front-end-defs.h - A list of definitions required for processing data
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	Macros for complex arithmetic, definition of PI and so on.
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Mon 10-Sep-2001 15:32:28
 *  Last modified:  Thu 11-Oct-2007 11:22:49 by hema
 *  $Id: front-end-defs.h,v 1.1 2001/10/25 12:28:05 hema Exp $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

#ifndef FRONT_END_DEFS
#define FRONT_END_DEFS
#define PI            3.1415926535898
/* definitions for complex number arithmetic */

/* complex number type */

 typedef struct cmplx { float re,im ; }complex;

/* temporary variable defenitions for complex arithmetic */

 float  rp_a,im_a,rp_b,im_b;

/* add complex no's a and b and store the result in c */

# define cadd(c,a,b) rp_a = a.re; im_a = a.im; \
                     rp_b = b.re; im_b = b.im; \
                     c.re = rp_a + rp_b;       \
                     c.im = im_a + im_b

/* conjugate f complex number a stored in c */

# define conjg(c,a) rp_a = a.re; im_a = a.im; \
                   c.re = rp_a; \
                   c.im = -im_a
 
/* subtract b from a and store the result in c */ 

# define csub(c,a,b) rp_a = a.re; im_a = a.im; \
                     rp_b = b.re; im_b = b.im; \
                     c.re = rp_a - rp_b;       \
                     c.im = im_a - im_b

/* multiply a and b and store in c */

# define cmul(c,a,b) rp_a = a.re; im_a = a.im;     \
                     rp_b = b.re; im_b = b.im;     \
                     c.re = rp_a*rp_b - im_a*im_b; \
                     c.im = rp_a*im_b + im_a*rp_b

/* divide a by b and store the result in c */

# define cdiv(c,a,b) rp_a = a.re; im_a = a.im; \
                     rp_b = b.re; im_b = b.im; \
                     c.re = ( rp_a*rp_b + im_a*im_b ) \
                           /( rp_b*rp_b + im_b*im_b );\
                     c.im = ( im_a*rp_b - rp_a*im_b ) \
                           /( rp_b*rp_b + im_b*im_b )

# define cabs(b) ((float)sqrt((double)(b.re*b.re+b.im*b.im)))

# define cabs2(b) (float) (b.re*b.re+b.im*b.im)
#define LOG_ZERO  (-1.0E20)
#define LOG_ONE  (0.0)
#define LOG_SMALL (-0.5E10)
#define MINLOGARG (-708.3)
#endif














/*-------------------------------------------------------------------------
 * $Log: front-end-defs.h,v $
 * Revision 1.1  2001/10/25 12:28:05  hema
 * Initial revision
 *
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of front-end-defs.h
 -------------------------------------------------------------------------*/
