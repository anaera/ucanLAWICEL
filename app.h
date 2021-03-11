//PAPA
#ifndef APP_H
#define	APP_H

#include <xc.h>
typedef union {
    unsigned char byte;

    struct {
        unsigned speed : 3;
        unsigned time : 1;
        unsigned f4 : 1;
        unsigned repit : 1;
        unsigned listen : 1;
        unsigned open : 1;
    };
} byteFlag;

typedef union {
    unsigned int value;

    struct {
        unsigned char lo;
        unsigned char hi;
    };
} byteOfWord;

unsigned char hexToInt(char val);
char intToHex(unsigned char byte);
void putChar(char ch);
//void putString(char *string);
void nullCommand(void);
void putOk(void);
void putError(void);
void putVersion(void);
void putSerial(void);
void actionEngine(void);
void listenEngine(void);
void closeEngine(void);
void statusEngine(void);
void speedEngine(void);
inline void setSpeed(unsigned char num);
void timeEngine(void);
void repitSend(void);
void sendPack(unsigned char);
void lawicelInit(void);
void setRepitVal(unsigned char val);
unsigned char isRepitSND(void);
void doLawicelOrProc(void);
unsigned char packToString(unsigned char *pPack);
void valueToString(unsigned char val);
void lawicel(void);
void lawicelCheck(unsigned char);

#endif