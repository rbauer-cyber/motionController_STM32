#include "console.h"
#include "multiLed.hpp"
#include "digitalOut.hpp"
#include "gpio.h"

// kDigitalPin13 selects builtin LED on the nucleo board
static const EDigitalPin kBuiltinLED = kDigitalPin13;

// Initialize and create global CMultiLed variable g_multiLed.
static uint8_t s_multiLedPins[] =
{
		kDigitalPin08, // PA9
		kDigitalPin09, // PC7
		kDigitalPin10, // PB6
		kDigitalPin11, // PA7
		kDigitalPin12, // PA6
};

static uint8_t s_numPins = sizeof(s_multiLedPins) / sizeof(s_multiLedPins[0]);
CMultiLed g_multiLed( s_multiLedPins, s_numPins );

CMultiLed::CMultiLed( uint8_t ledPins[], uint8_t numPins )
{
    m_numPins = ( numPins < MAX_LEDS) ? numPins : MAX_LEDS;

    for ( uint8_t i = 0; i < m_numPins; i++ )
    {
        EDigitalPin ePin = static_cast<EDigitalPin>(ledPins[i]);
    	m_ledPins[i] = ePin;
        CDigitalOut::Write(ePin, 0);
    }

    m_maxNumber = (1 << (m_numPins));
    m_number = 0;
    //consoleDisplay("CMultiLed:: constructor");
}

CMultiLed::CMultiLed()
{
    //consoleDisplay("CMultiLed:: constructor");
}

uint8_t CMultiLed::MaxPins() { return MAX_LEDS; }
uint8_t CMultiLed::MaxNumber() { return m_maxNumber; }

void CMultiLed::SetLed(uint8_t index, uint8_t state)
{
    if ( index < m_numPins )
    {
        EDigitalPin ePin = static_cast<EDigitalPin>(m_ledPins[index]);
        bool pinState = (state == 0) ? false : true;
        CDigitalOut::Write(ePin, pinState);
        CONSOLE_DISPLAY_ARGS("CMultiLed::SetLed: setting led %d to %d\r\n", index, pinState);
    }
    else
    {
        CDigitalOut::Toggle(kBuiltinLED);
    }
}

void CMultiLed::ToggleLed(uint8_t index)
{
    if ( index < m_numPins )
    {
        EDigitalPin ePin = static_cast<EDigitalPin>(m_ledPins[index]);
        CDigitalOut::Toggle(ePin);
        CONSOLE_DISPLAY_ARGS("CMultiLed::ToggleLed: toggling led %d\r\n", index);
    }
    else
    {
        CDigitalOut::Toggle(kBuiltinLED);
    }
}

void CMultiLed::ShowState()
{
	CONSOLE_DISPLAY_ARGS("CMultiLed: numpins = %d, maxNumber = %d\r\n",
    		m_numPins, m_maxNumber);
}

void CMultiLed::SetNumber(uint8_t number)
{
    if ( number < m_maxNumber )
    {
        m_number = number;

        for ( uint8_t bit = 0, count = 0; count < m_numPins; count++, bit++)
        {
            uint8_t bitMask = (1 << bit);
            bool pinState = (number & bitMask) ? true : false;
            EDigitalPin pin = static_cast<EDigitalPin>(m_ledPins[count]);
            CDigitalOut::Write(pin, pinState);
        }
    }
}
