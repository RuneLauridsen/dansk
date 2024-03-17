#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <intrin.h>

#ifdef __cplusplus
#include "base_core.h"
#include "base_memory.h"
#include "base_memory.c"
#else
#include "base_ansiconsole.h"
#include "base_core.h"

#include "base_memory.h"
#include "base_string.h"
#include "base_buf.h"
#include "base_map.h"
#include "base_unicode.h"
#include "base_fmt.h"
#include "base_builder.h"
#include "base_os.h"

#include "base_memory.c"
#include "base_string.c"
#include "base_buf.c"
#include "base_map.c"
#include "base_unicode.c"
#include "base_fmt.c"
#include "base_builder.c"
#include "base_os.c"
#endif
