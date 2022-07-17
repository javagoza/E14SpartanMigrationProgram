
#include "oled_extensions.h"
#include "stdlib.h"

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

void OLED_putIntVariable(PmodOLED *InstancePtr, int32_t value) {
    if (value<0) {
        OLED_PutChar(InstancePtr,  '-');
    }
    OLED_PutChar(InstancePtr,  (abs(value) / 10000) % 10 + 48);
    OLED_PutChar(InstancePtr,  (abs(value) / 1000) % 10 + 48);
    OLED_PutChar(InstancePtr,  (abs(value) / 100) % 10 + 48);
    OLED_PutChar(InstancePtr,  (abs(value) / 10) % 10 + 48);
    OLED_PutChar(InstancePtr,  (abs(value) % 10) + 48);
}


/* ------------------------------------------------------------ */
/***    OLED_RecedeCursor
**
**  Parameters:
**      InstancePtr - pointer to SPI handler and OLED data
**
**  Return Value:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Recedes the character cursor by one character location,
**      wrapping at the beginning of line and forward to the bottom at the
**      beginning of the display.
*/

void  OLED_RecedeCursor(PmodOLED *InstancePtr)
{
    OLED *OledPtr = &(InstancePtr->OLEDState);

    OledPtr->xchOledCur -= 1;
    if (OledPtr->xchOledCur < 0) {
        OledPtr->xchOledCur =  OledPtr->xchOledMax;
        OledPtr->ychOledCur -= 1;
    }
    if (OledPtr->ychOledCur < 0) {
        OledPtr->ychOledCur = 0;
    }

    OLED_SetCursor(InstancePtr, OledPtr->xchOledCur, OledPtr->ychOledCur);
}
