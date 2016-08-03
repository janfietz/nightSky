
/**
 * @file    effect_control.h
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#ifndef _EFFECT_CONTROL_H_
#define _EFFECT_CONTROL_H_

#include "ch.h"
#include "effect.h"

/*===========================================================================*/
/* Effect constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Effect pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
    void EffectControlResetWithColor(struct Color* color);
    void EffectControlDrawText(struct Color* color);

    void EffectControlInitThread(void);
    void EffectControlStartThread(void);

    void EffectControlNextEffect(void);
    void EffectControlNoEffect(void);
    void EffectControlHideControls(void);

    extern void SetLedColor(uint16_t led, const struct Color* color);
    extern void SetUpdateLed(void);
#ifdef __cplusplus
}
#endif

#endif /* _EFFECT_WANDERING_H_ */


/** @} */
