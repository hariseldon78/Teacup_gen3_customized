#ifndef	_CLOCK_H
#define	_CLOCK_H

#include "config.h"

void clock_250ms(void);
void clock_10ms(void);

uint32_t seconds_from_boot(void);

#ifdef REST_TIME
extern uint32_t working_seconds;
void reset_idle();
#endif

#endif	/* _CLOCK_H */

