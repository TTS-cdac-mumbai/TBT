#ifndef CONSTANTS_H
#define CONSTANTS_H
#define SAMPLINGFREQ  11025
#define MAXWAVESIZE   SAMPLINGFREQ*18  /* 18 secs buffer length */
#define MAXFRAMESIZE  512 
#define MAXLPORDER    25
#define PI            3.1415926535898
#define PI2           PI*2
#define MAXFRAMES     MAXWAVESIZE/64   /* Assuming a lowest shift of 64 */

/* Approximate formant ranges for formant extraction algo */
/* The exact values used in a program depends on the FFT resolution */
#define F1_LOW   300.0
#define F1_HIGH  900.0
#define F2_LOW   900.0
#define F2_HIGH  2500.0
#define F3_LOW   2500.0
#define F3_HIGH  3500.0
#endif
