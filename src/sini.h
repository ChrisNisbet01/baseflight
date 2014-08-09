#ifndef __SINI_H__
#define __SINI_H__

#define SINE_RANGE  4096

/*
    sini():
    Find the sine of an angle where angle is in degrees.

    angle_degrees: The angle multiplied by the divisor.
        So if you want the sine of 50.1 degrees, set angle_degrees to
        501 and set the divisor to 10
    divisor: The value in angle_degrees is divided by this value
        before calculating the sine.
    returns: The sine of the angle. The range of the value is
        a fraction of SINE_RANGE represeting the sine of the angle.
        so the return value for the sine of -90 degrees would be -SINE_RANGE.
*/
int sini( int angle_degrees, int divisor );

/*
    cosi():
    Find the cosine of an angle where angle is in degrees.

    angle_degrees: The angle multiplied by the divisor.
        So if you want the cosine of 50.1 degrees, set angle_degrees to
        501 and set the divisor to 10
    divisor: The value in angle_degrees is divided by this value
        before calculating the sine.
    returns: The cosine of the angle. The range of the value is
        a fraction of SINE_RANGE represeting the sine of the angle.
        so the return value for the sine of 90 degrees would be SINE_RANGE.
*/
int cosi( int angle_degrees, int divisor );

#endif
