
#ifndef TIMER_H
#define	TIMER_H

#include <xc.h> // include processor files - each processor file is guarded. 
extern unsigned int systemTick;
unsigned int getTime(void);
unsigned char expTime(unsigned int waitTime);

#endif	/* TIMER_H */

