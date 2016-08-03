/**
 * @file    effect_control.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_control.h"
#include "ch.h"
#include "hal.h"

#include "display.h"
#include "displayconf.h"
#include "ledconf.h"
#include "effect_randompixels.h"
#include "effect_fadingpixels.h"
#include "effect_nightsky.h"
#include "effect_simplecolor.h"
#include <string.h>
#include <stdlib.h>

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

static THD_WORKING_AREA(waEffectControlThread, 1024);

static struct effect_list effects;

static systime_t lastPatternSelect;

struct Color colorEffects1[LEDCOUNT];
struct EffectFadeState fadeEffects1[LEDCOUNT];

struct Color colorBuffer1[LEDCOUNT];
struct DisplayBuffer display =
{
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,
        .pixels = colorBuffer1,
};

struct Color resetColor;
static int16_t activeEffect = 0;
static bool nextEffect = false;
static bool noEffect = false;

/*===========================================================================*/
/* SimpleColor                                                               */
/*===========================================================================*/
static struct EffectSimpleColorCfg effSimpleColor_cfg =
{
    .color = {85, 185, 255},
    .fillbuffer = true,
};

static struct EffectSimpleColorData effSimpleColor_data =
{
    .reset = false,
};

static struct Effect effSimpleColor =
{
    .effectcfg = &effSimpleColor_cfg,
    .effectdata = &effSimpleColor_data,
    .update = &EffectSimpleUpdate,
    .reset = &EffectSimpleReset,
    .p_next = NULL,
};

/*===========================================================================*/
/* RandomPixels                                                              */
/*===========================================================================*/
static struct EffectRandomPixelsCfg effRandomPixels_cfg =
{
    .spawninterval = MS2ST(300),
    .color = {0, 0, 0},
    .randomRed = true,
    .randomGreen = true,
    .randomBlue = true,
};

static struct EffectRandomPixelsData effRandomPixels_data =
{
    .lastspawn = 0,
    .pixelColors = colorEffects1,
};

static struct Effect effRandomPixels =
{
    .effectcfg = &effRandomPixels_cfg,
    .effectdata = &effRandomPixels_data,
    .update = &EffectRandomPixelsUpdate,
    .reset = &EffectRandomPixelsReset,
    .p_next = NULL,
};

/*===========================================================================*/
/* NightSky                                                                  */
/*===========================================================================*/
static struct EffectNightSkyCfg effNightSky_cfg =
{
    .color = {255, 255, 255},
    .randomColor = true,
    .randomizePropability = 5,
    .fadeperiod = MS2ST(100) * 50,
};

static struct EffectNightSkyData effNightSky_data =
{
    .randomizeFades = true,
    .lastupdate = 0,
    .pixelColors = colorEffects1,
    .fadeStates = fadeEffects1,
};

static struct Effect effNightSky =
{
    .effectcfg = &effNightSky_cfg,
    .effectdata = &effNightSky_data,
    .update = &EffectNightSkyUpdate,
    .reset = &EffectNightSkyReset,
    .p_next = NULL,
};

/*===========================================================================*/
/* FadingPixels                                                             */
/*===========================================================================*/
static struct EffectFadingPixelsCfg effFadingPixels_cfg =
{
    .spawninterval = MS2ST(300),
    .fadeperiod = MS2ST(2000),
    .color = {0, 0, 0},
    .randomColor = true,
    .number = 1,
};

static struct EffectFadingPixelsData effFadingPixels_data =
{
    .lastspawn = 0,
    .pixelColors = colorEffects1,
    .fadeStates = fadeEffects1,
};

static struct Effect effFadingPixels =
{
    .effectcfg = &effFadingPixels_cfg,
    .effectdata = &effFadingPixels_data,
    .update = &EffectFadingPixelsUpdate,
    .reset = &EffectFadingPixelsReset,
    .p_next = NULL,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static void Draw(void)
{
    int i;
    systime_t current = chVTGetSystemTime();
    /* clear buffer */
    memset(display.pixels, 0, sizeof(struct Color) * LEDCOUNT);

    if (effects.p_next != NULL)
    {
        EffectUpdate(effects.p_next, 0, 0, current, &display);
    }

    for (i = 0; i < LEDCOUNT; i++)
    {
        SetLedColor(i, &display.pixels[i]);
    }

    //HACK to have LED2 with steady color
    struct Color col = {0, 0, 255};
    SetLedColor(3, &col);

    SetUpdateLed();
}

static void ResetEffects(void)
{
    systime_t current = chVTGetSystemTime();
    struct Effect* effect = effects.p_next;
    EffectReset(effect, 0, 0, current);
}

static THD_FUNCTION(EffectControlThread, arg)
{
    (void) arg;
    chRegSetThreadName("effectcontrol");

    effects.p_next = NULL;

    int16_t effectNumber = -1;
    systime_t time = chVTGetSystemTime();
    while (TRUE)
    {
        if (noEffect == true)
        {
            effects.p_next = NULL;
        }
        if (nextEffect == true)
        {
            noEffect = false;
            nextEffect = false;

            activeEffect++;
            if (activeEffect > 3)
            {
                activeEffect = 1;
            }

            time = chVTGetSystemTime();
            (void)time;
        }

        if (activeEffect != effectNumber)
        {
            //cleanup of last active effects
            if(effectNumber == 1)
            {

            }

            effectNumber = activeEffect;

            //setup of new active effects
            if (effectNumber == 0)
            {
                effects.p_next = &effSimpleColor;
            }
            else if(effectNumber == 1)
            {
                effects.p_next = &effNightSky;
            }
            else if (effectNumber == 2)
            {
                effects.p_next = &effRandomPixels;
            }
            else if (effectNumber == 3)
            {
                effFadingPixels_cfg.number = 1 + (rand() % 3);
                ColorRandom(&effFadingPixels_cfg.color);
                effFadingPixels_cfg.randomColor = (rand() % 2) > 0;

                effects.p_next = &effFadingPixels;
            }
            else
            {
                effects.p_next = &effRandomPixels;
            }

            ResetEffects();
        }

        Draw();

        chThdSleepMilliseconds(10);
    }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief
 *
 */

void ResetWithColor(struct Color* color)
{
    if (activeEffect == 0xFF)
    {
        activeEffect = 0xFF - 1;
    }
    else
    {
        activeEffect = 0xFF;
    }

    ColorCopy(color, &resetColor);

    lastPatternSelect = chVTGetSystemTime();
}



void EffectControlInitThread(void)
{
    activeEffect = 1;
}

void EffectControlStartThread(void)
{
    chThdCreateStatic(waEffectControlThread, sizeof(waEffectControlThread), LOWPRIO, EffectControlThread, NULL);
}

void EffectControlNextEffect(void)
{
    nextEffect = true;
}

void EffectControlNoEffect(void)
{
    noEffect = true;
}

/** @} */
