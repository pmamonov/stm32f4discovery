#include "FreeRTOS.h"

#ifdef TARGET_F407
#define CYCLES_PER_LOOP		7
#endif

#ifdef TARGET_F091
#define CYCLES_PER_LOOP		10
#endif

static inline void udelay(int us)
{
	volatile int i = us * configCPU_CLOCK_HZ / 1000000 / CYCLES_PER_LOOP;

	while (i--)
		;
}
