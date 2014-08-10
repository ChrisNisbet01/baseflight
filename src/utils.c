#include "board.h"
#include "mw.h"
#if defined(BOARD_ALIGN_USES_INTEGER_MATH)
#include "sini.h"
#endif

static bool standardBoardAlignment = true;     // board orientation correction
#if !defined(BOARD_ALIGN_USES_INTEGER_MATH)
static float boardRotation[3][3];              // matrix
#endif

int constrain(int amt, int low, int high)
{
    if (amt < low)
        return low;
    else if (amt > high)
        return high;
    else
        return amt;
}

#if defined(BOARD_ALIGN_USES_INTEGER_MATH)
typedef struct rotation_context_st
{
    int32_t     rotationMatrix[3][3];    /* rotation matrix */
} rotation_context_st;

static rotation_context_st board_rotation;

static void initRotationMatrix( rotation_context_st * const pctx, int32_t const x, int32_t const y, int32_t const z, uint32_t const scaleFactor )
{
    int32_t cosx, sinx, cosy, siny, cosz, sinz;
    int32_t coszcosx, coszcosy, sinzcosx, coszsinx, sinzsinx;

    cosx = cosi(x, scaleFactor);
    sinx = sini(x, scaleFactor);
    cosy = cosi(y, scaleFactor);
    siny = sini(y, scaleFactor);
    cosz = cosi(z, scaleFactor);
    sinz = sini(z, scaleFactor);

    coszcosx = cosz * cosx;
    coszcosx = DIVIDE_WITH_ROUNDING(coszcosx, SINE_RANGE);

    coszcosy = cosz * cosy;
    coszcosy = DIVIDE_WITH_ROUNDING(coszcosy, SINE_RANGE);

    sinzcosx = sinz * cosx;
    sinzcosx = DIVIDE_WITH_ROUNDING(sinzcosx, SINE_RANGE);

    coszsinx = sinx * cosz;
    coszsinx = DIVIDE_WITH_ROUNDING(coszsinx, SINE_RANGE);

    sinzsinx = sinx * sinz;
    sinzsinx = DIVIDE_WITH_ROUNDING(sinzsinx, SINE_RANGE);

    // define rotation matrix
    pctx->rotationMatrix[0][0] = coszcosy;
    pctx->rotationMatrix[0][1] = -cosy * sinz;
    pctx->rotationMatrix[0][1] = DIVIDE_WITH_ROUNDING(pctx->rotationMatrix[0][1], SINE_RANGE);
    pctx->rotationMatrix[0][2] = siny;

    pctx->rotationMatrix[1][0] = sinzcosx + DIVIDE_WITH_ROUNDING((coszsinx * siny),SINE_RANGE);
    pctx->rotationMatrix[1][1] = coszcosx - DIVIDE_WITH_ROUNDING((sinzsinx * siny), SINE_RANGE);
    pctx->rotationMatrix[1][2] = -sinx * cosy;
    pctx->rotationMatrix[1][2] = DIVIDE_WITH_ROUNDING(pctx->rotationMatrix[1][2], SINE_RANGE);

    pctx->rotationMatrix[2][0] = (sinzsinx) - DIVIDE_WITH_ROUNDING((coszcosx * siny), SINE_RANGE);
    pctx->rotationMatrix[2][1] = (coszsinx) + DIVIDE_WITH_ROUNDING((sinzcosx * siny), SINE_RANGE);
    pctx->rotationMatrix[2][2] = cosy * cosx;
    pctx->rotationMatrix[2][2] = DIVIDE_WITH_ROUNDING(pctx->rotationMatrix[2][2], SINE_RANGE);

}

static void performRotation( rotation_context_st const * const pctx, int32_t * const vec[3] )
{
    int32_t x = vec[0];
    int32_t y = vec[1];
    int32_t z = vec[2]
    
    vec[0] = DIVIDE_WITH_ROUNDING(pctx->rotationMatrix[0][0] * x + pctx->rotationMatrix[1][0] * y + pctx->rotationMatrix[2][0] * z, SINE_RANGE);
    vec[1] = DIVIDE_WITH_ROUNDING(pctx->rotationMatrix[0][1] * x + pctx->rotationMatrix[1][1] * y + pctx->rotationMatrix[2][1] * z, SINE_RANGE);
    vec[2] = DIVIDE_WITH_ROUNDING(pctx->rotationMatrix[0][2] * x + pctx->rotationMatrix[1][2] * y + pctx->rotationMatrix[2][2] * z, SINE_RANGE);  
}

void initBoardAlignment(void)
{
    // standard alignment, nothing to calculate
    if (!mcfg.board_align_roll && !mcfg.board_align_pitch && !mcfg.board_align_yaw)
        return;

    standardBoardAlignment = false;

    initRotationMatrix( &board_rotation, mcfg.board_align_roll, mcfg.board_align_pitch, mcfg.board_align_yaw, 1 );

}

void alignBoard(int16_t *vec)
{
    // TODO: change so that int32 values are passed in to save shifting values into temporary variables.
    int32_t tmp[3];

    tmp[0] = vec[0];
    tmp[1] = vec[1];
    tmp[2] = vec[2];

    performRotation( &board_rotation, tmp );

    vec[0] = tmp[0];
    vec[1] = tmp[1];
    vec[2] = tmp[2];
}
#else
void initBoardAlignment(void)
{
    float roll, pitch, yaw;
    float cosx, sinx, cosy, siny, cosz, sinz;
    float coszcosx, coszcosy, sinzcosx, coszsinx, sinzsinx;

    // standard alignment, nothing to calculate
    if (!mcfg.board_align_roll && !mcfg.board_align_pitch && !mcfg.board_align_yaw)
        return;

    standardBoardAlignment = false;

    // deg2rad
    roll = mcfg.board_align_roll * M_PI / 180.0f;
    pitch = mcfg.board_align_pitch * M_PI / 180.0f;
    yaw = mcfg.board_align_yaw * M_PI / 180.0f;

    cosx = cosf(roll);
    sinx = sinf(roll);
    cosy = cosf(pitch);
    siny = sinf(pitch);
    cosz = cosf(yaw);
    sinz = sinf(yaw);

    coszcosx = cosz * cosx;
    coszcosy = cosz * cosy;
    sinzcosx = sinz * cosx;
    coszsinx = sinx * cosz;
    sinzsinx = sinx * sinz;

    // define rotation matrix
    boardRotation[0][0] = coszcosy;
    boardRotation[0][1] = -cosy * sinz;
    boardRotation[0][2] = siny;

    boardRotation[1][0] = sinzcosx + (coszsinx * siny);
    boardRotation[1][1] = coszcosx - (sinzsinx * siny);
    boardRotation[1][2] = -sinx * cosy;

    boardRotation[2][0] = (sinzsinx) - (coszcosx * siny);
    boardRotation[2][1] = (coszsinx) + (sinzcosx * siny);
    boardRotation[2][2] = cosy * cosx;
}

void alignBoard(int16_t *vec)
{
    int16_t x = vec[X];
    int16_t y = vec[Y];
    int16_t z = vec[Z];

    vec[X] = LRINTF(boardRotation[0][0] * x + boardRotation[1][0] * y + boardRotation[2][0] * z);
    vec[Y] = LRINTF(boardRotation[0][1] * x + boardRotation[1][1] * y + boardRotation[2][1] * z);
    vec[Z] = LRINTF(boardRotation[0][2] * x + boardRotation[1][2] * y + boardRotation[2][2] * z);
}
#endif

void alignSensors(int16_t *src, int16_t *dest, uint8_t rotation)
{
    switch (rotation) {
        case CW0_DEG:
            dest[X] = src[X];
            dest[Y] = src[Y];
            dest[Z] = src[Z];
            break;
        case CW90_DEG:
            dest[X] = src[Y];
            dest[Y] = -src[X];
            dest[Z] = src[Z];
            break;
        case CW180_DEG:
            dest[X] = -src[X];
            dest[Y] = -src[Y];
            dest[Z] = src[Z];
            break;
        case CW270_DEG:
            dest[X] = -src[Y];
            dest[Y] = src[X];
            dest[Z] = src[Z];
            break;
        case CW0_DEG_FLIP:
            dest[X] = -src[X];
            dest[Y] = src[Y];
            dest[Z] = -src[Z];
            break;
        case CW90_DEG_FLIP:
            dest[X] = src[Y];
            dest[Y] = src[X];
            dest[Z] = -src[Z];
            break;
        case CW180_DEG_FLIP:
            dest[X] = src[X];
            dest[Y] = -src[Y];
            dest[Z] = -src[Z];
            break;
        case CW270_DEG_FLIP:
            dest[X] = -src[Y];
            dest[Y] = -src[X];
            dest[Z] = -src[Z];
            break;
        default:
            break;
    }

    if (!standardBoardAlignment)
        alignBoard(dest);
}

int32_t divideWithRounding( int32_t value, uint32_t divisor )
{
    int32_t d2 = divisor/2;
    
    return ((value < 0) ? (value-d2)/divisor : (value+d2)/divisor);
}

long baseflightLrintf(float x)
{
    return (x > 0) ? x + 0.5f : x - 0.5f;
}

#ifdef PROD_DEBUG
void productionDebug(void)
{
    gpio_config_t gpio;

    // remap PB6 to USART1_TX
    gpio.pin = Pin_6;
    gpio.mode = Mode_AF_PP;
    gpio.speed = Speed_2MHz;
    gpioInit(GPIOB, &gpio);
    gpioPinRemapConfig(AFIO_MAPR_USART1_REMAP, true);
    serialInit(mcfg.serial_baudrate);
    delay(25);
    serialPrint(core.mainport, "DBG ");
    printf("%08x%08x%08x OK\n", U_ID_0, U_ID_1, U_ID_2);
    serialPrint(core.mainport, "EOF");
    delay(25);
    gpioPinRemapConfig(AFIO_MAPR_USART1_REMAP, false);
}
#endif
