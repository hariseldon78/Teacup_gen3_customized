#ifndef	_CLOCK_H
#define	_CLOCK_H

#include "config.h"

void clock_250ms(void);
void clock_10ms(void);

uint32_t seconds_from_boot(void);

extern uint32_t working_seconds;

#endif	/* _CLOCK_H */

