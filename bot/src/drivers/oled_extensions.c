
#include "oled_extensions.h"

/*
 * Displays on the OLED a float variable with a header and a tail
 * ie T: 12.67 C
 * OLED_putFloatVariable(&myOled, 1, 1, "T: ", temp_degc, " C");
 */
void OLED_putFloatVariable(PmodOLED *InstancePtr, int xch, int ych, char * head,
        float value, char * tail) {
    OLED_SetCursor(InstancePtr, xch, ych);
    OLED_PutString(InstancePtr, head);
    OLED_PutChar(InstancePtr, ((int) value / 10) % 10 + 48);
    OLED_PutChar(InstancePtr, ((int) value) % 10 + 48);
    OLED_PutString(InstancePtr, ".");
    OLED_PutChar(InstancePtr, ((int) (value * 10)) % 10 + 48);
    OLED_PutChar(InstancePtr, ((int) (value * 100)) % 10 + 48);
    OLED_PutString(InstancePtr, tail);
}

void OLED_putIntVariable(PmodOLED *InstancePtr, int16_t value) {

    OLED_PutChar(InstancePtr,  (value / 10000) % 10 + 48);
    OLED_PutChar(InstancePtr,  (value / 1000) % 10 + 48);
    OLED_PutChar(InstancePtr,  (value / 100) % 10 + 48);
    OLED_PutChar(InstancePtr,  (value / 10) % 10 + 48);
    OLED_PutChar(InstancePtr,  (value % 10) + 48);
}
