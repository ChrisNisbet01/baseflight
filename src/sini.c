
// A sine approximation via a fourth-order cosine approx.
static int isin_S4(int x)
{
    int c, y;
    static const int qN= 13, qA= 12, B=19900, C=3516;

    c= x<<(30-qN);              // Semi-circle info into carry.
    x -= 1<<qN;                 // sine -> cosine calc

    x= x<<(31-qN);              // Mask with PI
    x= x>>(31-qN);              // Note: SIGNED shift! (to qN)
    x= x*x>>(2*qN-14);          // x=x^2 To Q14

    y= B - (x*C>>14);           // B - x^2*C
    y= (1<<qA)-(x*y>>16);       // A - x^2*(B-x^2*C)

    return c>=0 ? y : -y;
}

/*
	sini():
	Returns the sine of an angle.
	Result is in the range -result_range -> result_range representing the sine * result_range.
	angle_degrees: in degrees multiplied by the divisor
	result: sine of the angle * result_range
*/
int sini( int angle_degrees, int divisor )
{
	int result;
	
	if ( divisor < 1 )
		divisor = 1;
	/* convert angle into internal units */
	angle_degrees = (angle_degrees * 91)/divisor;

    /* calculate sine */
	result = isin_S4( angle_degrees );

	return result;
}

int cosi( int angle_degrees, int divisor )
{
	int result;
	
	if ( divisor < 1 )
		divisor = 1;
	/* convert angle into internal units */
	angle_degrees = (angle_degrees * 91)/divisor;
    /* cosine(angle) == sin(angle+90) */
	angle_degrees += 8192;

    /* calculate cosine */
	result = isin_S4( angle_degrees );

	return result;
}

