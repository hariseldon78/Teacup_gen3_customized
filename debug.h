#ifndef	_DEBUG_H
#define	_DEBUG_H

#include	<stdint.h>

#ifdef	DEBUG
	#define		DEBUG_PID				1
	#define		DEBUG_DDA				2
	#define		DEBUG_POSITION	4
	#define 	DEBUG_CLOCK	8
#else
	// by setting these to zero, the compiler should optimise the relevant code out
	#define		DEBUG_PID				0
	#define		DEBUG_DDA				0
	#define		DEBUG_POSITION	0
	#define 	DEBUG_CLOCK	0
#endif

#define		DEBUG_ECHO			128

extern volatile uint8_t	debug_flags;

#ifdef DEBUG_MODE
void debug_modes_start();
#endif

#endif	/* _DEBUG_H */


