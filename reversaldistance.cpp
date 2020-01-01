#include "problems.h"

struct Strip
{
    uint32_t beginIdx;
    uint32_t endIdx;
    bool decreasing;

    Strip(uint32_t begin, uint32_t end, bool decreasing) : beginIdx(begin), endIdx(end), decreasing(decreasing) {};
};

static ResultCode GetStrips(const vector<uint32_t>& perm, vector<Strip>& strips)
{
    assert(strips.size() == 0);

    uint32_t l          = 0;
    uint32_t r          = 0;
    bool curDecreasing  = false;

    while (r < perm.size())
    {
        if (r == (perm.size() - 1)) strips.push_back({ l, r - 1, curDecreasing });

        if (r == l)
        {
            if ((perm[r + 1] - perm[r]) == 1)
            {
                curDecreasing = false;
                r++;
                continue;
            }
            else if ((perm[r] - perm[r + 1]) == 1)
            {
                curDecreasing = true;
                r++;
                continue;
            }
            else
            {
                strips.push_back({ l, r, false });
                l++;
                r++;
                continue;
            }
        }

        if (curDecreasing && (perm[r] - perm[r + 1] == 1))
        {
            r++;
            continue;
        }
        else if (!curDecreasing && ((perm[r + 1] - perm[r] == 1)))
        {
            r++;
            continue;
        }
        else
        {
            strips.push_back({ l, r - 1, curDecreasing });
            l = r;
        }
    }

    return OK;
}

static bool FindDecreasingReversal(const vector<uint32_t>& perm, vector<uint32_t>& breakpoints)
{

}

static ResultCode BreakPointReversal(vector<uint32_t>& perm)
{
    vector<Strip> strips;
    GetStrips(perm, strips);

    return OK;
}

void ReversalDistance(vector<TestResult>& testResults)
{
    vector<uint32_t> perm = { 0, 8, 2, 7, 6, 5, 1, 4, 3, 9 };
    BreakPointReversal(perm);

    __debugbreak();

}