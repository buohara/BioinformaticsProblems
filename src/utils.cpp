#include "commoninc.h"

/**
 * GetMilliseconds Get timestamp in milliseconds since beginning of clock epoch.
 *
 * @return Timestamp in milliseconds.
 */

long long GetMilliseconds()
{
    static LARGE_INTEGER frequency;
    static BOOL useQpc = QueryPerformanceFrequency(&frequency);

    if (useQpc)
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / frequency.QuadPart;
    }
    else
    {
        return GetTickCount64();
    }
}