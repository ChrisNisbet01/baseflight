#pragma once

//#define DIVIDE_WITH_ROUNDING( n, d ) ((n) < 0 ? ((n)-((d)/2))/(d) : ((n)+((d)/2))/(d))
#define DIVIDE_WITH_ROUNDING( n, d ) divideWithRounding( (n), (d) )
int32_t divideWithRounding( int32_t value, int32_t divisor );

#if !defined(USE_STD_LRINTF)
#define LRINTF(x) baseflightLrintf(x)  
long baseflightLrintf(float x);
#else
#define LRINTF(x) lrintf(x)  
#endif

#if defined(ROTATIONS_USE_INTEGER_MATH)
typedef struct rotation_context_st
{
    int32_t     rotationMatrix[3][3];    /* rotation matrix */
} rotation_context_st;
void initRotationMatrix( rotation_context_st * const pctx, int32_t const x, int32_t const y, int32_t const z, uint32_t const scaleFactor );
void performRotation( rotation_context_st const * const pctx, int32_t *vec );
#endif

int constrain(int amt, int low, int high);
// sensor orientation
void alignSensors(int16_t *src, int16_t *dest, uint8_t rotation);
void initBoardAlignment(void);
void productionDebug(void);
