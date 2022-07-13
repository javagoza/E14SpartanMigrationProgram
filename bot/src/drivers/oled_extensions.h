
#ifndef __OLED_PRINT_VALUES_H_
#define __OLED_PRINT_VALUES_H_

#include "PmodOLED.h"

void OLED_putFloatVariable(PmodOLED *InstancePtr, int xch, int ych, char * head,
        float value, char * tail);

void OLED_putIntVariable(PmodOLED *InstancePtr, int16_t value);

void  OLED_RecedeCursor(PmodOLED *InstancePtr);

#endif // __OLED_PRINT_VALUES_H_
