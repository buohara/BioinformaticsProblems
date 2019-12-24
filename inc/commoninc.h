#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>

#include <Windows.h>

enum ResultCode
{
    OK                          = 0,
    INVALID_INPUT               = 1,
    UNABLE_TO_FIND_SOLUTION     = 2
};