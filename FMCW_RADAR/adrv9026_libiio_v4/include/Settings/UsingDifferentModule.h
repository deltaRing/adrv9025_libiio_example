#ifndef _USING_DIFFERENT_MODULE_H_
#define _USING_DIFFERENT_MODULE_H_

#include "../Utilize/common_define.h"
#include "../Utilize/iio_adrv9025.h"
#include "../Utilize/iio_adrv9009.h"

#if _USE_ADRV9025_ == 1

void Initialize_ADRV9025(int pn, int signal_length, int pulse_number, struct stream_cfg trxcfg);

#elif _USE_ADRV9009_ == 1

void Initialize_ADRV9009(int signal_length, int pulse_number, struct stream_cfg trxcfg);

#else

#error "Must Select a Platform"

#endif

#endif