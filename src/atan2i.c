#include <stdint.h>
#include "i32sqrt.h"

//#define _ATAN2_TEST_ 

#define BRAD_PI_SHIFT	14
#define BRAD_PI			(1<<BRAD_PI_SHIFT)
#define BRAD_HPI		(BRAD_PI>>1)
#define BRAD_2PI		(BRAD_PI<<1)

// Get the octant a coordinate pair is in.
#define OCTANTIFY(_x, _y, _o)   do {                            \
    int _t; _o= 0;                                              \
    if(_y<  0)  {            _x= -_x;   _y= -_y; _o += 4; }     \
    if(_x<= 0)  { _t= _x;    _x=  _y;   _y= -_t; _o += 2; }     \
    if(_x<=_y)  { _t= _y-_x; _x= _x+_y; _y=  _t; _o += 1; }     \
} while(0);

// QDIV stands for the fixed-point division method most appropriate for
// your system. Modify where appropriate.
// This would be for NDS.
#if 0
static inline int QDIV(int num, int den, const int bits)
{
	int res;

	res = (num<<bits)/den;
  
    return res;
}
#else
#define QDIV(num, den, bits)	(((num)<<(bits))/(den))
#endif

static int32_t divideWithRounding( int32_t value, int32_t divisor )
{
    int32_t d2 = divisor/2;
    
    return ((value < 0) ? ((value-d2)/divisor) : ((value+d2)/divisor));
}

// Approximate Taylor series for atan2, home-grown implementation.
// Returns [0,2pi), where pi ~ 0x4000.
static int32_t atan2_approx(int32_t x, int32_t y)
{
    if(y==0)    return (x>=0 ? 0 : BRAD_PI);

    static const int fixShift= 15;
    int  phi, t, t2, dphi;

    OCTANTIFY(x, y, phi);
    phi *= BRAD_PI>>2;

    t= QDIV(y, x, fixShift);
    t2= -t*t>>fixShift;

    dphi= 0x0470;
    dphi= 0x1029 + (t2*dphi>>fixShift);
    dphi= 0x1F0B + (t2*dphi>>fixShift);
    dphi= 0x364C + (t2*dphi>>fixShift);
    dphi= 0xA2FC + (t2*dphi>>fixShift);
    dphi= dphi*t >> fixShift;

    return phi + ((dphi+4) >> 3);
}

int32_t atan2i( int32_t x, int32_t y, int32_t scale_factor )
{
    int tmp = atan2_approx( y, x );

    /* return a value similar to that returned by atan2f() */
    if ( tmp > BRAD_PI )
        tmp -= BRAD_2PI;
    
    return divideWithRounding(tmp * 360 * scale_factor, BRAD_2PI );
}

int32_t asini( int32_t sine, int32_t sine_range, int32_t scale_factor )
{
    int32_t res = atan2i( sine, i32sqrt( (sine_range + sine) * (sine_range-sine) ), scale_factor );

    return res;
}

int32_t acosi( int32_t cosine, int32_t cosine_range, int32_t scale_factor )
{
    int32_t res = atan2i( i32sqrt( (cosine_range + cosine) * (cosine_range-cosine) ), cosine, scale_factor );
    
    return res;
}

#ifdef _ATAN2_TEST_
#include <stdio.h>      /* printf */
#include <math.h>       /* atan2 */


#define PI 3.1415926535897932384626433832795

int main ( int argc, char **argv )
{
	if ( argc == 2 )
	{
		int y, x, range;
		int angle, tmp;
		double libres;
		
		range = atoi( argv[1] );

		for( x=-range; x < range; x++ )
		{
			for ( y=-range; y < range; y++ )
			{
				angle = atan2i( y, x, 10);

				libres = (atan2 ((double)y, (double)x) * 1800 / PI);

                if ( libres < 0 )
                    tmp = libres - 0.5f;
                else
                    tmp = libres + 0.5f;
				//printf("y %d x %d angle %d atan2 tmp %d %f\n", y, x, angle, tmp, libres );
				if ( (angle > tmp && (angle - tmp) > 1) || (angle < tmp && (tmp-angle) > 1) )
					printf("\r\n\tdiff x %d y %d %d %d %f\n", x, y, angle, tmp, libres );
				// NB Nothing prints out, which means that for these x,y values, the error is never more than 0.1 degree.
				// Looks like the errors occur when the angle is very close to the middle of two integer values (e.g. 35.50xxx).
			}
		}
	}
	else if ( argc == 3 )
	{
		int y, x, range;
		int angle, tmp;
		float libres;
		
		y = atoi( argv[1] );
		x = atoi( argv[2] );

		angle = atan2i( y, x, 10);

		libres = (atan2f ((float)y, (float)x) * 1800.0f / PI);
        if ( libres < 0 )
            tmp = libres - 0.5f;
        else
            tmp = libres + 0.5f;
		printf("\r\ny %d x %d angle %d atan2f tmp %d %f\n", y, x, angle, tmp, libres );
	}
	return 0;
}
#endif

