#ifndef _LOICDECODER_TESTING_TEST_SHARED_H_
#define _LOICDECODER_TESTING_TEST_SHARED_H_

#include "loic_decoder/loic_decoder.h"

#define ensure(EXP) if(!(EXP)) return 1;
#define ensure_const(EXP) if constexpr (!(EXP)) return 1;
#define TEST_CASE

#endif
