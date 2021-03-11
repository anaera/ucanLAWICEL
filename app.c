//PAPA
/*
 *Функции LAWICEL
 */

#include "app.h"
#include "usb/usb_device_cdc.h"
#include "system.h"
#include "ucan.h"
#include "timer.h"
#define MAX_ANSW 8
#define MAX_REQ 22
#define MAX_PACK 22
const unsigned char strOk[] = "\r\0";
const unsigned char strError[] = "\a\0";
const unsigned char strSend[] = "z\r\0";
const unsigned char strVersion[] = "V0101\r\0";
const unsigned char strSerial[] = "N1234\r\0";

byteFlag engineFlag;
static unsigned char periodSend; // интервал LAWICEL
unsigned int expSend; //время очередной отправки пакета

unsigned char lawicelState;
unsigned char cntGetPack;
unsigned char lenGetPack;

unsigned char hexVal; //рабочие переменные
unsigned char tmpVal; //рабочие переменные
unsigned char errVal; //рабочие переменные
unsigned char lawicelPack[10] = {0x00, 0x02, 0x02, 0x00}; //NMT set STOP

unsigned char *ptrTx, *ptrRx;
char stringPack [MAX_PACK]; //пакет из буфера приема в HEX-строку для вывода
unsigned char cntAnsw;
char stringAnswer [MAX_ANSW]; //строка ответов на комманды извне

void (*proc) (void);
//----------------------------------------------------------------------
/**
 * Преобразование HEX символа в целое INT
 * @param val
 * код HEX символа
 * @return
 * целое значение INT 
 */
//----------------------------------------------------------------------

unsigned char hexToInt(char val) {
    if ((val > 0x2F)&&(val < 0x3A)) {
        return (val - 0x30);
    } else {
        if ((val > 0x40)&&(val < 0x47)) {
            return (val - 0x37);
        } else {
            if ((val > 0x60)&&(val < 0x67)) {
                return (val - 0x57);
            } else {
                return 0xFF;
            }
        }
    }
}
//----------------------------------------------------------------------
/**
 * Получение кода HEX символа от значения INT
 * @param byte
 * значение INT
 * @return 
 * код символа HEX
 */
//----------------------------------------------------------------------

char intToHex(unsigned char byte) {
    return ((byte < 10) ? (byte + 48) : (byte + 55));
}
//----------------------------------------------------------------------
/**
 * Вывод строки символов с термнальным нулем в USB port 
 * @param string
 * Терминированная нулем строка символов во FLASH
 */
//----------------------------------------------------------------------

void putChar(char ch) {
    if (cntAnsw < MAX_ANSW) {
        stringAnswer [cntAnsw] = ch;
        cntAnsw++;
    }
}


//    putrsUSBUSART(string);
//}
//----------------------------------------------------------------------
/**
 * Вывод строки символов с термнальным нулем в USB port 
 * @param string
 * Терминированная нулем строка символов в RAM
 */
//----------------------------------------------------------------------

//void putString(char *string) {
//    putsUSBUSART(string);
//}
//----------------------------------------------------------------------
/**
 * 
 */
//----------------------------------------------------------------------
//----------------------------------------------------------------------
/**
 * Вывод строки Ok
 */
//----------------------------------------------------------------------

void putOk(void) {
    LATAbits.LATA4 = LATAbits.LATA4^1;
    putChar('\r');
    //mUSBUSARTTxRom(strOk, 1);
}
//----------------------------------------------------------------------
/**
 * Вывод строки Error
 */
//----------------------------------------------------------------------

void putError(void) {
    LATAbits.LATA5 = LATAbits.LATA5^1;
    putChar('\a');
    //    mUSBUSARTTxRom(strError, 1);
}
//----------------------------------------------------------------------
/**
 * Информация о версии
 */
//----------------------------------------------------------------------
#define VERSION_HARDWARE_MAJOR '1'
#define VERSION_HARDWARE_MINOR '0'
#define VERSION_FIRMWARE_MAJOR '1'
#define VERSION_FIRMWARE_MINOR '8'

void putVersion(void) {
    putChar('V');
    putChar(VERSION_HARDWARE_MAJOR);
    putChar(VERSION_HARDWARE_MINOR);
    putChar(VERSION_FIRMWARE_MAJOR);
    putChar(VERSION_FIRMWARE_MINOR);
    putChar('\r');
    //   mUSBUSARTTxRom(strVersion, 6);
}
//----------------------------------------------------------------------
/**
 * Информация о серийнике
 */
//----------------------------------------------------------------------

void putSerial(void) {
    putChar('N');
    putChar('1');
    putChar('2');
    putChar('3');
    putChar('4');
    putChar('\r');
    //  mUSBUSARTTxRom(strSerial, 6);
}
//----------------------------------------------------------------------
/**
 * Открыть канал CAN
 */
//----------------------------------------------------------------------

void activEngine(void) {
    engineFlag.open = 1;
    engineFlag.listen = 0;
    putOk();
}
//----------------------------------------------------------------------
/**
 * Режим прослушиванияшины
 */
//----------------------------------------------------------------------

void listenEngine(void) {
    engineFlag.open = 1;
    engineFlag.listen = 1;
    putOk();
}
//----------------------------------------------------------------------
/**
 * Закрыть канал CAN
 */
//----------------------------------------------------------------------

void closeEngine(void) {
    engineFlag.open = 0;
    engineFlag.listen = 0;
    putOk();
}
//----------------------------------------------------------------------
/**
 * Статус драйвера LAWICEL
 */
//----------------------------------------------------------------------

void statusEngine(void) {
    //    if (engineFlag.open == 1) {
    unsigned char err = isERR();
    valueToString(0);
    return;
}
//----------------------------------------------------------------------
/**
 * Установить скорость CAN
 * 0-115; 1-57.6; 2-38.4; 3-19.2; 4-9.6; 5-4.8; 6-2.4; 7-1.2
 */
//----------------------------------------------------------------------

void speedEngine(void) {
    if (tmpVal < 8) {
        engineFlag.speed = tmpVal;
        setSpeed(tmpVal);
        setMult(tmpVal);
        putOk();
        return;
    }
    putError();
    return;
}
//----------------------------------------------------------------------
/**
 * Установить отображение меток времени
 * 0-не показывать метки времени
 * 1-показать метки времени
 */
//----------------------------------------------------------------------

void switchEcho(void) {
    if (tmpVal == 0) {
        clrEchoTX();
    } else {
        if (tmpVal == 1) {
            setEchoTX();
        } else {
            putError();
            return;
        }
    }
    putOk();
    return;
}

void timeEngine(void) {
    if (tmpVal == 0) {
        engineFlag.time = 0;
    } else {
        if (tmpVal == 1) {
            engineFlag.time = 1;
        } else {
            putError();
            return;
        }
    }
    putOk();
    return;
}
//----------------------------------------------------------------------
/**
 * Установить период повторения отправки пакетов
 */
//----------------------------------------------------------------------

void repitSend(void) {
    setRepitVal(tmpVal);
    putOk();
}

//----------------------------------------------------------------------
/**
 * Отправка пакета CAN
 */
//----------------------------------------------------------------------

void sendPack(unsigned char rtr) {
    unsigned int val;
    LATCbits.LATC0 = LATCbits.LATC0^1;
    if ((engineFlag.open == 1) && (engineFlag.listen == 0)) {
        if ((lawicelPack[0]&0x80) == 0) {
            lawicelPack[0] = lawicelPack[0] << 1;
            if (lawicelPack[1]&0x80) {
                lawicelPack[0] = lawicelPack[0] | 0x01;
            }
        }

        lawicelPack[1] = ((lawicelPack[1] << 1)&0xE0) | rtr | (lawicelPack[1] & 0x0F);
        doPack(lawicelPack);
        val = (unsigned int) periodSend;
        expSend = getTime() + (val << 4);
        putChar('z');
        putChar('\r');
        return;
    }
    putError();
    return;
}

void sendPackS(void) {
    if ((lawicelPack[1]&0x0F) == 0) {
        sendPack(0x10);
        return;
    }
    putError();
    return;
}

void sendPackT(void) {
    LATCbits.LATC2 = LATCbits.LATC2^1;
    sendPack(0x00);
}

//----------------------------------------------------------------------
/**
 * Инициализация LAWICEL
 */
//----------------------------------------------------------------------

void nullCommand(void) {
};

void lawicelInit(void) {
    LATCbits.LATC0 = 0;
    LATCbits.LATC2 = 0;
    ptrRx = ptrRxD(); //получить адрес пакета приема
    ptrTx = ptrTxD(); //получить адрес пакета передачи
    proc = nullCommand; //процедура пустой комманды
    periodSend = 0;
    lawicelState = 1; //начальное сосотояние мащины LAWICEL
}

//----------------------------------------------------------------------

void setRepitVal(unsigned char val) {
    periodSend = val; //устанавливаем время циклической отсылки пакета LAWECIL
    expSend = getTime()+(unsigned char) (periodSend << 4);
}

//----------------------------------------------------------------------

unsigned char isRepitSND(void) {
    if (periodSend) {
        if (expTime(expSend)) { //истек период до следующей посылки
            expSend = getTime()+(unsigned char) (periodSend << 4); //время
            return 1; //пора повторить отсылку пакета
        }
    }
    return 0; //ждем наступленияя момента
}
//----------------------------------------------------------------------
/**
 * 
 */
//----------------------------------------------------------------------

void doLawicelOrProc(void) {
    //установлена ли периодическая отправка пакетов, да-отправляем
    unsigned char *ptrBufferRx;
    if (isRepitSND()) {
        if ((engineFlag.open == 1) && (engineFlag.listen == 0)) {
            if (isTxD()) {
                doPack(lawicelPack);
                //               nextSend();
            }
        }
    }
    //процедуры ввода/вывода 
    if (USBUSARTIsTxTrfReady()) { //можно что либо вывести
        //контроль ошибки приема /передачи
        /*
        errVal = isERR();
        errVal = 0;
        if (errVal) {
            errVal = errVal & 0x7F;
            if (errVal) {
                valueToString(errVal);
                putString(stringPack); //вывод строки
                return;
            }

        } else {
                   }*/

        cntAnsw = 0;
        lawicel(); //ввод символов
        if (cntAnsw) {
            mUSBUSARTTxRam((unsigned char*) stringAnswer, cntAnsw)//вывод строки
            cntAnsw = 0;
            return;
        }

        //вывод принятого пакета
        if ((engineFlag.open == 1) || (engineFlag.listen == 1)) {
            if (isRxD()) { //принят ли пакет
                //преобразуем пакет в строку
                unsigned char len = packToString(ptrRx);
                clrRxD(); //прием завершен
                mUSBUSARTTxRam((unsigned char*) stringPack, len)//вывод строки                        
                return;
            }
        }
        //диалог вывода/ввода  LAWICEL


    }
}
//----------------------------------------------------------------------
/**
 * Преобразование пакета в HEX строку
 * @param pPack
 * указатель на пакет CAN
 */
//----------------------------------------------------------------------

unsigned char packToString(unsigned char *pPack) {
    unsigned char len, cnt;
    byteOfWord ZTM;
    ZTM.value = getTime();
    len = ((*(pPack + 1))&0x0F) + 2;
    cnt = 0;
    stringPack[cnt] = 116; //"t";
    if (((*(pPack + 1))&0x10)) stringPack[cnt] = 115; //"s"

    if ((*pPack & 0x01) == 1) {
        (*(pPack + 1)) = (((*(pPack + 1) >> 1) | 0x80)&0xF0) | (*(pPack + 1)&0x0F);
    }
    *pPack = *pPack >> 1;
    cnt++;
    while (len) {
        stringPack[cnt] = intToHex((*pPack & 0xF0) >> 4);
        cnt++;
        stringPack[cnt] = intToHex(*pPack & 0x0F);
        cnt++;
        len--;
        pPack++;
    }
    if (engineFlag.time) {
        stringPack[cnt] = intToHex((ZTM.hi & 0xF0) >> 4);
        cnt++;
        stringPack[cnt] = intToHex(ZTM.hi & 0x0F);
        cnt++;
        stringPack[cnt] = intToHex((ZTM.lo & 0xF0) >> 4);
        cnt++;
        stringPack[cnt] = intToHex(ZTM.lo & 0x0F);
        cnt++;
    }
    stringPack[cnt] = '\r';
    cnt++;
    return cnt;
    //stringPack[cnt] = 0;
}
//----------------------------------------------------------------------

void valueToString(unsigned char val) {
    putChar('F');
    putChar(intToHex((val & 0xF0) >> 4));
    putChar(intToHex(val & 0x0F));
    putChar('\r');
    /*
    stringPack[0] = 70; // "F"
        stringPack[1] = intToHex((val & 0xF0) >> 4);
    stringPack[2] = intToHex(val & 0x0F);
    stringPack[3] = '\r';
    stringPack[4] = 0;
     */
}
//----------------------------------------------------------------------
/**
 * Прием сивола из USB porta
 */
//----------------------------------------------------------------------

void lawicel(void) {
    unsigned char cnt, num, val[MAX_REQ];
    num = getsUSBUSART(val, MAX_REQ);
    if (num) {
        for (cnt = 0; cnt < num; cnt++) {
            lawicelCheck(val[cnt]);
        }
    }
}
//----------------------------------------------------------------------
/**
 * Анализатор принятых символов LAWICEL
 * @param val
 * принятый символ
 */
//----------------------------------------------------------------------

void lawicelCheck(unsigned char val) {
    switch (lawicelState) {
        case 0:
        {
            lawicelState = 1;
            if (val == '\r') {
                proc();
                return;
            }
            putError();
            return;
        }
        case 1:
        {
            lawicelState = 0;
            switch (val) {
                case 86: //V
                    proc = putVersion;
                    return;
                case 78: //N
                    proc = putSerial;
                    return;
                case 79: //O
                    proc = activEngine;
                    return;
                case 76: //L
                    proc = listenEngine;
                    return;
                case 67: //C
                    proc = closeEngine;
                    return;
                case 70: //F
                    proc = statusEngine;
                    return;
                    //двухсимвольные комманды
                case 69: //E
                    proc = switchEcho;
                    lawicelState = 2;
                    return;
                case 83: //S
                    proc = speedEngine;
                    lawicelState = 2;
                    return;
                case 90: //Z
                    proc = timeEngine;
                    lawicelState = 2;
                    return;
                case 82: //R
                    proc = repitSend;
                    lawicelState = 2;
                    return;
                case 116: //t
                    proc = sendPackT;
                    cntGetPack = 0;
                    lenGetPack = 1;
                    lawicelState = 3;
                    return;
                case 115: //s
                    proc = sendPackS;
                    cntGetPack = 0;
                    lenGetPack = 1;
                    lawicelState = 3;
                    return;
                case 13:
                    lawicelState = 1;
                    //                    proc = putOk;
                    return;
                default:
                    lawicelState = 5;
                    return;
            }
            return;
        }
        case 2:
        {
            lawicelState = 1;
            if (val == '\r') {
                tmpVal = 0;
                proc();
                return;
            }
            lawicelState = 0;
            if ((val > 0x2F)&&(val < 0x3A)) {
                tmpVal = val - 0x30;
                return;
            } else {
                lawicelState = 5;
                return;
            }
        }
        case 3:
        {

            if (val == '\r') {
                lawicelState = 1;
                putError();
                return;
            }

            tmpVal = hexToInt(val);

            if (tmpVal > 15) {
                lawicelState = 5;
                return;
            }
            lawicelState = 4;
            return;
        }
        case 4:
        {
            if (val == '\r') {
                lawicelState = 1;
                putError();
                return;
            }
            hexVal = hexToInt(val);
            if (hexVal > 15) {
                lawicelState = 5;
                return;
            }

            if (cntGetPack == 1) {
                if (hexVal < 9) {
                    lenGetPack = lenGetPack + hexVal;
                } else {
                    lawicelState = 5;
                    return;
                };
            }
            lawicelPack [cntGetPack] = hexVal | tmpVal << 4;
            cntGetPack++;
            lawicelState = 3;
            if (cntGetPack > lenGetPack) {
                lawicelState = 0;
                return;
            }
            return;
        }
        case 5:
        {
            if (val == '\r') {
                lawicelState = 1;
                putError();
                return;
            }
        }

        default:
        {
            proc = nullCommand;
            lawicelState = 1;
            return;
        }
    }
}
//----------------------------------------------------------------------