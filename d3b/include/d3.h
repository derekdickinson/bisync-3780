#include <dos.h>
#include <stdio.h>
#include <setjmp.h>

#undef inportb
#undef outportb

#define SYNC 1

#include "include\maxes.h"
#include "include\enums.h"
#include "include\typedefs.h"
#include "include\protos.h"
#include "\common\include\protos.h"
#include "include\aliases.h"
#include "include\globals.h"
#include "\common\include\globals.h"
