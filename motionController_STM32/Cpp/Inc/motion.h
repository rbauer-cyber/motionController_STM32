#ifndef MOTION_H_
#define MOTION_H_

#include <stdarg.h>

#ifdef __cplusplus
    extern "C" {
#endif

#define BSP_MAX_LED 5

void BSP_timerInterrupt();
void BSP_notifyTimerEvent();
void BSP_initTick();
void BSP_initMotor();
void BSP_initKnob();
void BSP_initSwitch();
void BSP_setPositionKnob(int8_t position);
void BSP_ToggleLed(uint8_t index);
uint8_t BSP_readKnob(void);
uint8_t BSP_readSwitch(void);
int16_t BSP_getMotorPosition(void);
int16_t BSP_MoveMotor(int16_t increment);
uint8_t BSP_getMotorMoving(void);
int32_t BSP_getMotorStepDelay(void);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H_ */
