#include "timer.h"
//----------------------------------------------------------------------
/**
 * Возвращает текущее значение счетчика миллисикунд (макс 32768мс)
 * @return значение счетчика в миллисекундах
 */
unsigned int getTime(void) {
    unsigned int time;
    INTCONbits.TMR0IE = 0; //запрещаем прерывание от Таймера 0
    time = systemTick;
    INTCONbits.TMR0IE = 1; //Разрешаем прерывание от Таймера 0
    return time;
}
//----------------------------------------------------------------------

/**
 * Проверяет истекло ли время до ожидаемого события
 * @param waitTime - время наступления события в миллисекундах (<32768ms)
 * @return 1 - событие наступило, 0 - событие не наступило
 */
unsigned char expTime(unsigned int waitTime) {
    unsigned int currentTime;
    INTCONbits.TMR0IE = 0; //запрещаем прерывание от Таймера 0
    currentTime = systemTick;
    INTCONbits.TMR0IE = 1; //Разрешаем прерывание от Таймера 0  
    waitTime++;
    if (currentTime > waitTime) {
        if (currentTime - waitTime < 0x8000) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (waitTime - currentTime > 0x8000) {
            return 1;
        } else {
            return 0;
        }
    }
}
