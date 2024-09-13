#ifndef __TOUCH_HANDLER_H__
#define __TOUCH_HANDLER_H__

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include <stdint.h>
#include "xpt2046.h"
#include "config.h"
#include "constants.h"

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

typedef struct touch_calibration_t
{
    uint16_t rawX;
    uint16_t rawY;
} touch_calibration_t;

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

extern XPT2046 touch;

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void initTouch();
void initTouchCalibration(void);
bool readTouchCalibration(touch_calibration_t *data);
bool writeTouchCalibration(touch_calibration_t *data);
bool isTouchInZone(int16_t x, int16_t y, int16_t w, int16_t h);

#endif /*__TOUCH_HANDLER_H__*/