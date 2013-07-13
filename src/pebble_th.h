/** \file
 * Functions in libpblth.a.
 */
#ifndef _pblth_h_
#define _pblth_h_

#include <stdint.h>
#include "pebble_os.h"

#define PEBBLE_SCREEN_WIDTH 144
#define PEBBLE_SCREEN_HEIGHT 168


extern int
__attribute__((__format__(printf, 2, 3)))
pebble_sprintf(
	char * buf,
	const char * fmt,
	...
);


extern void
text_layer_setup(
	Window * window,
	TextLayer * layer,
	GRect frame,
	GFont font
);


void
monospace_text(
	GContext * ctx,
	const char * buf,
	GFont font,
	GPoint xy,
	int x_size,
	int y_size
);


#define container_of(ptr, type, member) ({ \
	char * __mptr = (char*)(uintptr_t) (ptr); \
	(type *)(__mptr - offsetof(type,member) ); \
})


/** Translate a Pebble time struct into unix epoch.
 *
 * GMT offset is in seconds east of GMT.
 * For example, NYC EST is -5 * 3600 seconds.
 * \todo check dst offset!
 */
uint32_t
unix_epoch_time(
	const PblTm * const curr_time,
	const int gmt_offset
);


/** Compute the phase of the moon.
 * 0 == new moon,
 * 2953 == almost a full moon.
 */
uint32_t
moonphase(
	const PblTm * const now
);


uint32_t
julian_day_pbl(
	const PblTm * const now
);


uint32_t
julian_day(
	const int year, // since 0
	const int month, // 1 - 12
	const int day // 1 - 31
);


uint32_t
days_since_1900(
	uint32_t year
);

#endif