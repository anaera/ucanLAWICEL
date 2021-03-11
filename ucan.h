#ifndef UCAN_H
#define	UCAN_H
#include <xc.h> // include processor files - each processor file is guarded. 


#define TICK_TIME 19       //1мс = 100 * 10 
#define LAWICEL_TICK    1000    //1сек = 1мс * 1000
#define GAP_TIME    2
#define RCV_MASK 0x08
#define HI_SEND 0xFE        //0xFE
#define LO_SEND 0x80        //0x80
#define LEN_PACK    2   //номера посылок в пакете 0 и 1, номер CRC =LEN_PACK= 2 
#define MAX_REPIT    5
#define CRC_MASK    0x31
//#define DEV_ADDR    0x01

enum state {
    S_IDL = 0, S_RCV, S_SND, S_ERR
};

typedef union {
    unsigned char flag;

    struct {
        unsigned sndRPT : 3;
        unsigned echoTX : 1;
        unsigned sBit : 1;
        unsigned rBit : 1;
        unsigned sndRDY : 1;
        unsigned rcvRDY : 1;
    };
}
BYTESTATE;

typedef union {
    unsigned char flag;

    struct {
        unsigned drv : 1; /**ошибока UART                       :01*/
        unsigned bus : 1; /**ошибока приоритета шины            :02*/
        unsigned crc : 1; /**ошибока CRC                        :04*/
        unsigned frm : 1; /**ошибока фрейма                     :08*/
        unsigned ovr : 1; /**ошибока переполнения буфера приема :10*/
        unsigned rpt : 1; /**ошибока передачи                   :20*/
        unsigned : 1; /**резев                              :40*/
        unsigned act : 1; /**флаг завершения активности шины    :80*/
    };
}
BYTEERROR;

typedef struct {
    unsigned char gapTime; ///<величина защитного интервала в 100мкс интервалах
    unsigned char gapMult; ///<множитель защитного интервала в 100мкс интервалах
    unsigned char rcvMask; ///<маска выбоки бита 
    unsigned char crcMask; ///<CRC полином
} tConst;

void USBDeviceTasks(void);


unsigned char initCAN(unsigned char num);
inline void loadConstEngine(void);
inline void setMult(unsigned char num);

inline void setStateSend(void);
inline void setStateReceive(void);
inline void setStateIdle(void);

inline void initPackEngine(void);
inline void timeCtlEngine(void);
inline void actNextSend(void);
inline void packActEngine(void);

inline unsigned char nextBit(void);
void doPack(unsigned char pack[]);
unsigned char doCRC(unsigned char crc, unsigned char byte);

//API calls
void setEchoTX(void);
void clrEchoTX(void);

inline unsigned char *ptrRxD(void);
inline unsigned char *ptrTxD(void);
inline unsigned char isTxD(void);
inline unsigned char isRxD(void);
inline void doTxD(void);
inline void clrRxD(void);
unsigned char isERR(void);

#endif	/* UCAN_H */

