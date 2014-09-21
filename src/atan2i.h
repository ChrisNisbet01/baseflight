#ifndef __ATAN2I_H__
#define __ATAN2I_H__

/* 
    atan2i():
    Integer approximation for the atan2 function.
    Return a value in degrees (NOT RADIANS) multiplied by the scale factor 
    So, with an angle of 90 degrees and a scale factor of 10, the value
    returned would be 900.
*/
int32_t atan2i( int32_t x, int32_t y, int32_t scale_factor );
int32_t asini( int32_t sine, int32_t sine_range, int32_t scale_factor );
int32_t acosi( int32_t cosine, int32_t cosine_range, int32_t scale_factor );

#endif /* __ATAN2I_H__ */

