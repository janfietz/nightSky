/**
 * @file    board_drivers.c
 * @brief
 *
 * @{
 */

#include "board_drivers.h"

#include "ch.h"
#include "hal.h"

#include "ws281x.h"
#include "ledconf.h"
#include "usbcfg.h"

extern ws281xDriver ws281x;
extern SerialUSBDriver SDU1;

static ws281xConfig ws281x_cfg =
{
    LEDCOUNT,
    LED1,
    {
        12000000,
        WS2811_BIT_PWM_WIDTH,
        NULL,
        {
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL }
        },
        0,
        TIM_DIER_UDE | TIM_DIER_CC2DE | TIM_DIER_CC1DE,
    },
    &PWMD2,
    1,
    WS2811_ZERO_PWM_WIDTH,
    WS2811_ONE_PWM_WIDTH,
    STM32_DMA1_STREAM7,
    7,
};

void BoardDriverInit(void)
{
    sduObjectInit(&SDU1);
    ws281xObjectInit(&ws281x);
}

void BoardDriverStart(void)
{
    ws281xStart(&ws281x, &ws281x_cfg);
    palSetPadMode(GPIOA, 1, PAL_MODE_STM32_ALTERNATE_PUSHPULL);

    /*
     * Initializes a serial-over-USB CDC driver.
     */
    sduStart(&SDU1, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1000);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);
}

void BoardDriverShutdown(void)
{
    sduStop(&SDU1);
    ws281xStop(&ws281x);
}

/** @} */
