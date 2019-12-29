#include "problems.h"

using namespace std;

/**
 * GetMinMax - Get the minimum and maximum values of a list. Algorithm works
 * inward from both ends, comparing pairs of values at each step. If upper
 * value "hi" is greater than lower "lo", check hi against max and lo against
 * min. Do the opposite if lo is greater than hi.
 *
 * @param list [in] An unsorted list of values to get min and max values from.
 * Will return INVALID_INPUT if list length is zero.
 *
 * @param min [in/out] Min value found in list.
 * @param max [in/out] Max value found in list.
 *
 * @return Result code. OK if min/max found successfully. INVALID_INPUT from zero-length
 * lists.
 */

static ResultCode GetMinMax(const vector<uint32_t> &list, uint32_t &min, uint32_t &max)
{
    if (list.size() == 0) return INVALID_INPUT;

    min             = ~0;
    max             = 0;
    uint32_t len    = (uint32_t)list.size();

    for (uint32_t i = 0; i <= len / 2; i++)
    {
        uint32_t hi = list[len - i - 1];
        uint32_t lo = list[i];

        if (hi > lo)
        {
            if (hi > max) max = hi;
            if (lo < min) min = lo;
        }
        else
        {
            if (lo > max) max = lo;
            if (hi < min) min = hi;
        }
    }

    return OK;
}

/**
 * GetMinMaxBruteForce - Get the minimum and maximum values of a list. Simply loop through
 * every list value and compare with current min/max. Used for comparing with
 * GetMinMax above.
 *
 * @param list [in] An unsorted list of values to get min and max values from.
 * Will return INVALID_INPUT if list length is zero.
 *
 * @param min [in/out] Min value found in list.
 * @param max [in/out] Max value found in list.
 *
 * @return Result code. OK if min/max found successfully. INVALID_INPUT from zero-length
 * lists.
 */

static ResultCode GetMinMaxBruteForce(const vector<uint32_t>& list, uint32_t& min, uint32_t& max)
{
    if (list.size() == 0) return INVALID_INPUT;

    min = ~0;
    max = 0;

    for (auto& val : list)
    {
        if (val > max) max = val;
        if (val < min) min = val;
    }

    return OK;
}

/**
 * TestLengthZeroList - Test get min/max of a zero-length list. Test passes if get
 * min/max returns INVALID_INPUT.
 *
 * @param [in/out] List of test results to append results to.
 */

static void TestLengthZeroList(vector<TestResult>& testResults)
{
    uint32_t min    = 0;
    uint32_t max    = 0;
    ResultCode res  = OK;

    res = GetMinMax({}, min, max);

    if (res == INVALID_INPUT)
        testResults.push_back({ "MinMax::LengthZeroList", PASS, "" });
    else
        testResults.push_back(
            { "MinMax::LengthZeroList", FAIL, "Zero length list should return INVALID_INPUT." }
    );
}

/**
 * TestLengthoneList - Test get min/max of a length one list. Create a list of a single random
 * value x. Test passes if getminmax returns {x, x}, 
 *
 * @param [in/out] List of test results to append results to.
 */

static void TestLengthOneList(vector<TestResult>& testResults)
{
    uint32_t min    = 0;
    uint32_t max    = 0;
    ResultCode res  = OK;

    const uint32_t randVal = rand();

    res = GetMinMax({ randVal }, min, max);

    if (min == randVal && max == randVal)
        testResults.push_back({ "MinMax::LengthOneList", PASS, "" });
    else
    {
        const string randValStr = to_string(randVal);
        const string minStr     = to_string(min);
        const string maxStr     = to_string(max);

        testResults.push_back(
            {
                "MinMax::LengthOneList",
                FAIL,
                "Expected {min, max} = {" + randValStr + ", " + randValStr + "}, found {" + minStr + ", " + maxStr + "}"
            }
        );
    }
}

/**
 * TestRandomLists - Generate lists of random length and get min/max. Compare with brute-force
 * linear search. Test cases pass if results match with brute-force results.
 *
 * @param [in/out] List of test results to append results to.
 */

static void TestRandomLists(vector<TestResult> & testResults)
{
    uint32_t min        = 0;
    uint32_t max        = 0;

    uint32_t minBF      = 0;
    uint32_t maxBF      = 0;

    ResultCode res      = OK;
    ResultCode resBF    = OK;

    const uint32_t maxLen   = 1000000;
    const uint32_t numIters = 100;

    for (uint32_t i = 0; i < numIters; i++)
    {
        string iterStr = to_string(i);

        uint32_t curLen = rand() % maxLen;
        vector<uint32_t> list(curLen);
        for (uint32_t i = 0; i < curLen; i++) list[i] = rand();

        res     = GetMinMax(list, min, max);
        resBF   = GetMinMaxBruteForce(list, minBF, maxBF);

        if (min == minBF && max == maxBF)
            testResults.push_back({ "MinMax::RandomList[" + iterStr + "]", PASS, "" });
        else
        {
            const string minStr     = to_string(min);
            const string maxStr     = to_string(max);
            const string minBFStr   = to_string(minBF);
            const string maxBFStr   = to_string(maxBF);

            testResults.push_back(
                {
                    "MinMax::RandomList[" + iterStr + "]",
                    FAIL,
                    "Expected {min, max} = {" + minBFStr + ", " + maxBFStr + "}, found {" + minStr + ", " + maxStr + "}"
                }
            );
        }
    }
}

/**
 * GetMinMax - Run tests of algorithm that gets min and max of an unsorted input list. First, run
 * a few specific corner cases, then run random lists of random lengths.
 *
 * @param testResults [in/out] List to append test results to.
 */

void GetMinMax(vector<TestResult>& testResults)
{
    TestLengthZeroList(testResults);
    TestLengthOneList(testResults);
    TestRandomLists(testResults);
}