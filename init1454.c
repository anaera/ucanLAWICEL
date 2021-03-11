/*
 *Функции нициализации МК
 */
#include <xc.h> 
#include "init1454.h"


//                                 250, 125, 76.8, 38.4, 19.2, 9.6 | 4.8   2.4 
const unsigned int speedConst[8] = {23, 47, 95, 155, 311, 624, 1249, 2499}; //, 4999};
//------------------------------------------------------------------------------
/**
 * \file
 * \brief Инициализация регистров  конфигурация портов PIC
 */
//------------------------------------------------------------------------------

void initChip(void) {
    initCPU();
    initPORTA();
    //    initPORTB();
    initPORTC();
    initWDT();
}
//------------------------------------------------------------------------------

void initCPU(void) {
    //init_OSC
    //OSCCONbits.SCS = 0;

}
//------------------------------------------------------------------------------

void initPORTA(void) {
    //init_PORTA
    TRISA = 0xCF; //all pins input
    ANSELA = 0x00; // all analog to digital I/O      
    PORTA = 0x00;
    LATA = 0; //all pins set 0
}
//------------------------------------------------------------------------------

/*
void initPORTB(void) {
    //init_PORTB
    PORTB = 0x00;
    TRISB = 0; //all pins out
    LATB = 0; //all pins set 0
}
 */
void initPORTC(void) {
    //init_PORTC
    TRISC = 0xF2; //low pins out, high pins in
    ANSELC = 0x00; // all analog to digital I/O 
    PORTC = 0x00;
    LATC = 0; //all pins set 0

}

//----------------------------------------------------------------------

/**
 * Установить скорость uCAN и множитель GAP
 * 250, 125, 76.8, 38.4, 19.2, 9.6 | 4.8   2.4 
 * @param num - номер параметра скорости
 */
inline void setSpeed(unsigned char num) {
    SPBRG = speedConst[num];
}
//----------------------------------------------------------------------

/**
 * Инициализация UART
 */
void initUART(unsigned char num) {
    //   ANSELHbits.ANS11 = 0;

    TRISCbits.TRISC4 = 0; //TxD 
    TRISCbits.TRISC5 = 1; //RxD
    //SYNC=0 BRGH=1 BRG16=1   ->  Fosc/4*[N+1]    
    BAUDCON = 0;
    TXSTA = 0;
    RCSTA = 0;
    TXSTAbits.TXEN = 1; //TX Включить TX передачу
    RCSTAbits.CREN = 1; //CREN Включить RX прием
    TXSTAbits.SYNC = 0; //TX Асинхронный режим SYNC
    TXSTAbits.BRGH = 1; //высокоскоростной BRGH
    BAUDCONbits.BRG16 = 1; //BRG16 16-битный генератор скорости
    SPBRG = speedConst[num];
    RCSTAbits.SPEN = 1; //SPEN Включить УАРТ   
    //Устанавливаем приориет прерывания
    //IPR1bits.RCIP = 1; //HI priority RxD
}
//----------------------------------------------------------------------

void initTMR0(void) {
    //init_TMR0
    OPTION_REG = 2; //Прескалер 1:8, тактирование Fosc/4 = 12,  = ,08333...
    TMR0 = 0xB4;
    //Прескалер 1:4, тактирование Fosc/4,
    //один тик 16000000/16 = 1мкс (us)
    //Устанавливаем приориет прерывания   
    //   INTCON2bits.TMR0IP = 1; //HI priority
}
//------------------------------------------------------------------------------

void initWDT(void) {
    //init_WDT
    WDTCON = 0b00010010; //512ms
    //        WDTCONbits.SWDTEN=1;
}

void initINT(void) {
    //Разрешаем прерывание от Таймера 0
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    //Разрешаем прерывание от Таймера 1
    PIE1bits.TMR1IE = 0;
    
    OPTION_REGbits.INTEDG = 0;  //прерывание по падающему фронту
    INTCONbits.INTF = 0;
    INTCONbits.INTE = 0; //Разрешаем прерывание от INT0

    INTCONbits.PEIE = 1; //Разрешаем прерывания от переферии
    //RCONbits.IPEN = 1; //разрешаем приоритет прерываний

    PIE1bits.RCIE = 1; //Разрешение прерывание от приемника УАРТ

    INTCONbits.GIE = 1; //разрешаем прерывания
    //INTCONbits.GIEH = 1; //разрешаетвысокоприоритетные прерывания
    //INTCONbits.GIEL = 0; //разрешает низкоприоретееные прерывания    
}
