
//PAPA
#include "init1454.h"
#include "drive.h"
#include "ucan.h"
//------------------------------------------------------------------------------
/**
 * \file
 * \brief Обработчик прерываний
 * 
 * Таймер T0
 * формирование интервалапрерываний каждые 50мкс,
 * счетчик интервала 1мс,
 * формирование межпакетного интервала неактивности шины,
 * переход в режим передачи при неактивности шины.
 *
 * Приемник USART
 * прием байта в прерывании,
 * последующий вывод байта в режиме передачи.
 *
 * Контроль активности шины по прерыванию INT0
 * обнаружение падения уровня на шине (активности шины).
 *
 */

//----------------------------------------------------------------------

extern tConst CONST;
extern unsigned char divCnt;
extern unsigned char actStep, divCnt, countTick;
extern unsigned int systemTick;
        
void stepLED(unsigned char state);
void timeCtlEngine(void);
void timeCtlEngine(void);
void setStateReceive(void);
void packActEngine(void);

void __interrupt(high_priority) hiINT(void) {
    if (INTCONbits.TMR0IF && INTCONbits.TMR0IE) {
//        stepLED(actStep);
        INTCONbits.TMR0IF = 0;
        TMR0 = 0xBA;
        if (countTick) {
            countTick--; //считаем с шагом 100мкс (шагом GAP) до 10 == 1мс
        } else {
            countTick = TICK_TIME; //считаем до 1мс (TICK_TIME = 10)
//            uData.canFlags.sysTickFlag = 1; // взводим флаг 1мс
            systemTick++; //считаем системное время в миллисекундах
        }
        divCnt--;
        if (!divCnt) {
            divCnt = CONST.gapMult;
            timeCtlEngine(); //каждый интервал GAP = 100мс*gap.Mult вызываем обработчик
        }
    }
    if (INTCONbits.INTE && INTCONbits.INTF) {
        setStateReceive(); //переход в режим приема
    }
    if (PIE1bits.RCIE && PIR1bits.RCIF) {
        packActEngine();
    }
     if (USBIF && USBIE) {
        USBDeviceTasks();
    }
}
//----------------------------------------------------------------------

