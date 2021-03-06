
#ifndef __OLED_EXTENSION_H_
#define __OLED_EXTENSION_H_

#include "PmodOLED.h"


void OLED_putFloatVariable(PmodOLED *InstancePtr, int xch, int ych, char * head,
        float value, char * tail);

void OLED_putIntVariable(PmodOLED *InstancePtr, int32_t value);

void  OLED_RecedeCursor(PmodOLED *InstancePtr);

#endif // __OLED_EXTENSION_H_
