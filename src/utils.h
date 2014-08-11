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


int constrain(int amt, int low, int high);
// sensor orientation
void alignSensors(int16_t *src, int16_t *dest, uint8_t rotation);
void initBoardAlignment(void);
void productionDebug(void);
