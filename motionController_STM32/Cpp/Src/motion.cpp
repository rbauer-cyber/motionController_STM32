//#include <AccelStepper.h>
#include "main.h"  // has declaration for encoder
#include "Stepper.hpp"
//#include <TimerOne.h>
#include "bsp.hpp"
#include "digitalOut.hpp"
#include "motion.h"
#include "motor.hpp"
#include "multiLed.hpp"
#include "console.h"

//#include "../include/fastRotaryEncoder.h"

//#define ACCEL_STEPPER

extern CMultiLed g_multiLed;

#define USE_BUILTIN_SWITCH
#define HIGH 1
#define LOW 0

// Created this file so that C++ stepper class can be used in project.
// Define the AccelStepper interface type: 4 wire motor in half step mode:
#define MotorInterfaceType 8

static int s_encoderPosition = 0;
int16_t s_stepsPerRevolution = 2048;
int16_t s_increment = 2048;
int16_t s_motorSpeed = 15;

#ifdef ACCEL_STEPPER
// Initialize with pin sequence IN1-IN3-IN2-IN4
//AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper = AccelStepper(MotorInterfaceType, A3, A1, A2, A0);
#else
// Initialize with pin sequence IN1-IN3-IN2-IN4 on the stepper motor controller board
// s_stepper(s_stepsPerRevolution, A3, A1, A2, A0);
Stepper s_stepper(s_stepsPerRevolution,
		kAnalogPin02, kAnalogPin04,
		kAnalogPin03, kAnalogPin05);
#endif

static int16_t s_motorPosition = 0;

#if 0
int testFastRotaryEncoder(CFastRotaryEncoder& encoder, int position)
{
    static bool buttonDown = false;
    bool buttonDownPressed = encoder.GetButtonDown();
    
    if ( buttonDownPressed )
    {
        if ( !buttonDown )
        {
            buttonDown = true;
            Serial.println("Button pressed.");
            delay(500);
            encoder.DumpStateTable();
        }
    }
    else
    {
        buttonDown = false;
    }
}
#endif

uint8_t BSP_readKnob(void)
{
    int newPosition = encoderUpdate();

    if ( (newPosition >= g_multiLed.MaxNumber()) )
    {
        newPosition = g_multiLed.MaxNumber() - 4;
    }
    else if ( newPosition < 0 )
    {
        newPosition = 4;
    }

    // Test for new rotary encoder position and return position.
    if (newPosition != s_encoderPosition)
    {
        char direction = (newPosition > s_encoderPosition) ? 'R' : 'L';
        consoleDisplayArgs("\r\nKnob: %c%d\r\n", direction, newPosition);
        s_encoderPosition = newPosition;
    }

    g_multiLed.SetNumber(s_encoderPosition);
    return s_encoderPosition;
}

void BSP_ToggleLed(uint8_t index)
{
	g_multiLed.ToggleLed(index);
}

void BSP_initRotaryEncoder()
{
    // Fast rotary encoder does not use delays for debouncing.
    // It uses state information to track phase of encoder A/B.
    //s_encoderPosition = FastRotaryEncoder.Setup();
    encoderInit();
}

void BSP_initKnob()
{
    BSP_initRotaryEncoder();
    //g_multiLed.ShowState();
}

void BSP_setPositionKnob(int8_t position)
{
	g_multiLed.SetNumber(position);
}

#ifdef ACCEL_STEPPER
// Accel Stepper control is very slow with 28BYJ-48 stepper motor.
// Performance is better with simple Stepper motor interface.
void BSP_initMotor()
{
    //s_stepper.setSpeed(s_motorSpeed);
    // Set the maximum steps per second:
    stepper.setMaxSpeed(1000);
    // Set the maximum acceleration in steps per second^2:
    stepper.setAcceleration(200);
    // Reset the position to 0:
    stepper.setCurrentPosition(0);
}

int16_t BSP_getMotorPosition(void)
{
    s_motorPosition = stepper.currentPosition();
    return s_motorPosition;
}

uint8_t BSP_getMotorMoving(void)
{
    uint8_t moving = 0;

    if (stepper.distanceToGo() != 0)
    {
        moving = 1;
        stepper.run();
    }
    else
    {
        moving = 0;
    }

    return moving;
}

int16_t BSP_MoveMotor(int16_t increment)
{    
    // Set target position:
    int16_t targetPosition = s_motorPosition + increment;
    // Run to position with set speed and acceleration:
    //stepper.runToNewPosition(targetPosition);
    //s_motorPosition = targetPosition;
    // Set target position:
    // Run to position with set speed and acceleration:
    stepper.moveTo(targetPosition);
    //stepper.runToPosition();
    //stepper.moveTo(position);
    //stepper.runToPosition(); 
#if 0
    // If at the end of travel go to the other end
    while (stepper.distanceToGo() != 0)
    {
        stepper.run();
    }
#else
    stepper.runToPosition(); 
    //stepper.run();
#endif
    //s_motorPosition = targetPosition;
    return s_motorPosition;
}
#else
static EDigitalPin s_buttonPin = kDigitalPin16;
static uint8_t s_buttonState = HIGH;

void BSP_initSwitch()
{
#ifdef USE_BUILTIN_SWITCH
    //pinMode(s_buttonPin, INPUT_PULLUP);
    s_buttonState = CDigitalOut::Read(s_buttonPin);
#endif
}

uint8_t BSP_readSwitch(void)
{
#ifdef USE_BUILTIN_SWITCH
	s_buttonState = CDigitalOut::Read(s_buttonPin);
#else
	s_buttonState = HIGH;
#endif
    return s_buttonState;
}

void BSP_initMotor()
{
    s_stepper.setSpeed(s_motorSpeed);
    s_motorPosition = 0;
}

int32_t BSP_getMotorStepDelay(void)
{
    return s_stepper.getStepDelay();
}

int16_t BSP_getMotorPosition(void)
{
    return s_motorPosition;
}

uint8_t BSP_getMotorMoving(void)
{
    uint8_t moving = 0;
    return moving;
}

int16_t BSP_MoveMotor(int16_t increment)
{
    s_stepper.step(increment);
    s_motorPosition += increment;
    return s_motorPosition;
}

void BSP_initTick(void) {
    // 100 microsec interrupt
#ifdef USE_TIMER1
    Timer1.initialize(100);
    Timer1.attachInterrupt(BSP_notifyTimerEvent);
    Timer1.start();
#endif
}
#endif
