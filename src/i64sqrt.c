#include <stdint.h>
//#define TEST

#if 0
uint32_t i64sqrt(uint64_t x)
{
    uint64_t a = 0;
    uint64_t a2 = 0;
    int k;

    for (k = 31; k >= 0; k--) 
    {
    	uint64_t t = a2 + (a << (k+1)) + ((uint64_t)1 << (k + k));

    	if (x > t) 
    	{
    	    a += 1 << k;
    	    a2 = t;
    	}
    }

    return a;
}
#else
/* i64sqrt:
    ENTRY x: unsigned long
    EXIT  returns floor(sqrt(x) * pow(2, BITSPERLONG/2))

    Since the square root never uses more than half the bits
    of the input, we use the other half of the bits to contain
    extra bits of precision after the binary point.

    EXAMPLE
        suppose BITSPERLONG = 32
        then    usqrt(144) = 786432 = 12 * 65536
                usqrt(32) = 370727 = 5.66 * 65536

    NOTES
        (1) change BITSPERLONG to BITSPERLONG/2 if you do not want
            the answer scaled.  Indeed, if you want n bits of
            precision after the binary point, use BITSPERLONG/2+n.
            The code assumes that BITSPERLONG is even.
        (2) This is really better off being written in assembly.
            The line marked below is really a "arithmetic shift left"
            on the double-long value with r in the upper half
            and x in the lower half.  This operation is typically
            expressible in only one or two assembly instructions.
        (3) Unrolling this loop is probably not a bad idea.

    ALGORITHM
        The calculations are the base-two analogue of the square
        root algorithm we all learned in grammar school.  Since we're
        in base 2, there is only one nontrivial trial multiplier.

        Notice that absolutely no multiplications or divisions are performed.
        This means it'll be fast on a wide range of processors.

        CN - As it stands, the function will round the result up if the fractional part of the
        result is >= 0.5, so isqrt(31) will return the value 6.
*/
#define BITSPERLONG 64

#define TOP2BITS(x) ((x & (3LL << (BITSPERLONG-2))) >> (BITSPERLONG-2))
uint32_t i64sqrt(uint64_t x)
{
    uint32_t res;
	uint64_t a = 0L;            /* accumulator      */
	uint64_t r = 0L;            /* remainder        */
	uint32_t e;                 /* trial product    */
	int i;

	for (i = 0; i < BITSPERLONG/2+1; i++)   /* NOTE 1 */
	{
	    r = (r << 2) + TOP2BITS(x); x <<= 2; /* NOTE 2 */
	    a <<= 1;
	    e = (a << 1) + 1;
	    if (r >= e)
	    {
	          r -= e;
	          a++;
	    }
	}
    /* CN - Round upwards if the fractional part is >= 0.5. */
	res = (a >> 1) + (a & 1);

	return res;
}
#endif

#ifdef TEST
int main( int argc, char **argv )
{
    int32_t v = 1000;
    uint64_t tmp = (uint64_t)v * v + (uint64_t)v * v;
    int32_t res = i64sqrt(tmp);

    printf("\r\nres %d", res );
}
#endif

