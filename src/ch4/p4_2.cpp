#include "problems.h"

/**
 * GetPairwiseDistances - Given a list of points, return the sorted list of distances between each pair
 * of points in the input list.
 * 
 * @param points    [in] List of input points.
 * @param distances [in/out] List of sorted pairwise distances computed by this function. Assumed empty on input.
 */

static ResultCode GetPairwiseDistances(const vector<uint32_t>& points, vector<uint32_t>& distances)
{
    assert(distances.size() == 0);
    distances.resize(points.size() * (points.size() - 1) / 2);
    uint32_t cnt = 0;

    for (uint32_t i = 0; i < points.size(); i++)
        for (uint32_t j = i + 1; j < points.size(); j++)
            distances[cnt++] = points[i] > points[j] ? points[i] - points[j] : points[j] - points[i];

    sort(distances.begin(), distances.end());

    return OK;
}

/**
 * ReflectSet - Restriction mapping algorithms might not return unique point sets. For example,
 * A set 10 - {0, 2, 4, 7, 10} = {10, 8, 6, 3, 0} (i.e., reflected around the max element 10) 
 * will generate the same pairwise distances as {0, 2, 4, 7, 10}. If two restriction mapping
 * algorithms generate different results, it's useful to know if they are reflections of one
 * another. This routine reflects a solution around its max element.
 * 
 * @param inSet  [in] Set to reflect around its max element.
 * @param outSet [in/out] Reflected set computed by this function. Assumed empty on input.
 */

static ResultCode ReflectSet(const vector<uint32_t>& inSet, vector<uint32_t>& outSet)
{
    if (outSet.size() != 0) return INVALID_INPUT;
    if (inSet.size() == 0) return INVALID_INPUT;

    const uint32_t maxElem = inSet[inSet.size() - 1];
    for (auto& elem : inSet) outSet.push_back(maxElem - elem);
}

/**
 * ComputePDBF - Compute the partial digest PD of an input list L of pairwise
 * distances. PD is a set of points such that the distance between each pair of its points is L. 
 *
 * This is a brute-force algorithm. Since all PD points x
 * must be in the input distance list (dist(x, 0) = x), take combinations of points from
 * the input list and compute the list of pairwise distances. If a combination generates
 * the input distance list, return that combination.
 *
 * @param  list [in] Input set of pairwise distances between points. Assumed to be sorted in ascending order.
 * @param  pd   [in/out] The computed set of points PD from L. Assumed empty on input.
 *
 * @return      INVALID_INPUT if no distances input. OK if solution found, UNABLE_TO_FIND_SOLUTION otherwise.
 */

static ResultCode ComputePDBF(const vector<uint32_t>& distList, vector<uint32_t> &pd)
{
    assert(pd.size() == 0);
    if (distList.size() == 0) return INVALID_INPUT;

    uint32_t n = 2;
    uint32_t i = 2;

    while (i * (i - 1) / 2 != (uint32_t)distList.size()) n = i++;

    vector<bool> comboIdcs(distList.size() - 1, false);

    for (uint32_t i = 0; i < n - 2; i++) comboIdcs[distList.size() - 2 - i] = true;

    do
    {
        vector<uint32_t> candidate(n);
        candidate[0]        = 0;
        candidate[n - 1]    = distList[distList.size() - 1];

        uint32_t candIdx = 1;

        for (uint32_t i = 0; i < comboIdcs.size(); i++)
            if (comboIdcs[i]) candidate[candIdx++] = distList[i];

        vector<uint32_t> distances;
        GetPairwiseDistances(candidate, distances);

        if (memcmp(&distances[0], &distList[0], distList.size() * sizeof(uint32_t)) == 0)
        {
            pd.resize(n);

            pd[0]       = 0;
            pd[n - 1]   = distList[distList.size() - 1];

            for (uint32_t i = 1; i < n - 1; i++) 
                pd[i] = candidate[i];

                return OK;
        }
    }
    while (next_permutation(comboIdcs.begin(), comboIdcs.end()));

    return UNABLE_TO_FIND_SOLUTION;
}

/**
 * SearchDistListResursive - This algorithm iteratively grabs the largest unused distance D from 
 * the input list L and computes points that are D from the left/or right ends of PD
 * (the endpoints of PD must be {0, max(L)). It then generates a list of pairwise distances for
 * all current solution points. If these computed distances are part of the input distance list,
 * remove them from the input list, add the newly computed point to the solution, 
 * and recurse into this function.
 *
 * If a recursion fails (unable to find a viable point from remaining distances), remove the last added
 * point from the solution and add the distances it generated back to the input list, then try
 * a new distance. Algorithm terminates successfully if remaining distance list is empty.
 *
 * @param pd    [in/out] Current intermediate solution point set.
 * @param dist  [in/out] Remaining distances
 */

static bool SearchDistListRecursive(set<uint32_t>& pd, unordered_map<uint32_t, uint32_t> &dist)
{
    bool empty = true;

    for (auto& pair : dist)
    {
        if (pair.second != 0)
        {
            empty = false;
            break;
        }
    }

    if (empty) return true;

    const uint32_t maxDist = *pd.rbegin();

    for (auto &pair : dist)
    {
        uint32_t curMax     = pair.first;
        uint32_t left       = curMax;
        uint32_t right      = maxDist - curMax;

        bool leftValid      = true;
        bool rightValid     = true;

        vector<uint32_t> newDistList;

        for (auto& pt : pd)
        {
            uint32_t newDist = pt > left ? pt - left : left - pt;
            if (dist.count(newDist) == 0)
            {
                leftValid = false;
                break;
            }
            newDistList.push_back(newDist);
        }

        if (leftValid)
        {
            pd.insert(left);
            for (auto& newDist : newDistList) dist[newDist]--;
            if (SearchDistListRecursive(pd, dist)) return true;
            else
            {
                pd.erase(left);
                for (auto& newDist : newDistList) dist[newDist]++;
                leftValid = false;
            }
        }

        if (!leftValid)
        {
            newDistList.resize(0);

            for (auto& pt : pd)
            {
                uint32_t newDist = pt > right ? pt - right : right - pt;
                if (dist.count(newDist) == 0)
                {
                    rightValid = false;
                    break;
                }
                newDistList.push_back(newDist);
            }

            if (rightValid)
            {
                pd.insert(right);
                for (auto& newDist : newDistList) dist[newDist]--;
                if (SearchDistListRecursive(pd, dist)) return true;
                else
                {
                    pd.erase(right);
                    for (auto& newDist : newDistList) dist[newDist]++;
                }
            }
        }
    }

    return false;
}

/**
 * ComputePDBackTracking - Driver routine for recursive method above. Simply copies the input distances into a map
 * and initializes the solution point set with  {0, max}, then passes them to the recursive search.
 *
 * @param  distSetIn    [in] Input set of pairwise distances between points. Assumed to be sorted in ascending order.
 * @param  pd           [in/out] The computed set of points PD from L. Assumed empty on input.
 *
 * @return              INVALID_INPUT if no distances input. OK if solution found, UNABLE_TO_FIND_SOLUTION otherwise.
 */

static ResultCode ComputePDBacktracking(const vector<uint32_t>& distList, vector<uint32_t>& pd)
{
    assert(pd.size() == 0);
    if (distList.size() == 0) return INVALID_INPUT;

    unordered_map<uint32_t, uint32_t> distSet;
    set<uint32_t> pdSet;

    const uint32_t maxDist = distList[distList.size() - 1];
    pdSet.insert(0);
    pdSet.insert(maxDist);

    for (uint32_t i = 0; i < distList.size() - 1; i++)
    {
        if (distSet.count(distList[i]) == 0) distSet[distList[i]] = 1;
        else distSet[distList[i]]++;
    }

    if (SearchDistListRecursive(pdSet, distSet))
    {
        for (auto& pt : pdSet) pd.push_back(pt);
        sort(pd.begin(), pd.end());

        return OK;
    }

    return UNABLE_TO_FIND_SOLUTION;
}

/**
 * P4_2 - Restriction mapping problems. The restriction mapping/turnpike problem asks, given
 * a list of pairwise distances between points, can we reconstruct the original set of points.
 * This routine tests a backtracking technique (see ComputePDBackTracking above).
 * 
 * This routine randomly generates 100 lists of points for power-of-two list sizes between 2 and 256.
 * It generates the distance list for these points, then times the algorithm's solution. Tests
 * pass if the algorithms reproduce the original point set.
 *
 * The brute force algorithm above is not tested. For even small values of N, the
 * number of trial point combinations grows exponentially.
 *
 * @param testResults Result list to append results to.
 */

void P4_2(vector<TestResult>& testResults)
{
    const uint32_t itersPerSize = 100;
    const uint32_t maxListSize  = 256;
    const uint32_t maxVal       = 10000;

    uint32_t testCase = 0;

    vector<uint32_t> testList = { 2, 2, 3, 3, 4, 5, 6, 7, 8, 10 };
    vector<uint32_t> pd;

    ComputePDBacktracking(testList, pd);

    for (uint32_t i = 2; i <= maxListSize; i *= 2)
    {
        for (uint32_t j = 0; j < itersPerSize; j++)
        {
            set<uint32_t> pointSet;
            pointSet.insert(0);

            while (pointSet.size() < i) pointSet.insert(rand() % maxVal);
            vector<uint32_t> points;

            for (auto& val : pointSet) points.push_back(val);
            vector<uint32_t> dist;
            GetPairwiseDistances(points, dist);

            vector<uint32_t> solutionBT;
            long long t1BT      = GetMilliseconds();
            ResultCode resBT    = ComputePDBacktracking(dist, solutionBT);
            long long t2BT      = GetMilliseconds();
            float btSec         = ((float)t2BT - (float)t1BT) / 1000.0f;

            string testStr      = to_string(testCase);
            string sizeStr      = to_string(i);
            string btTStr       = to_string(btSec);

            if (resBT == UNABLE_TO_FIND_SOLUTION)
            {
                testResults.push_back({ "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr, FAIL, "Backtracking algorithm couldn't find solution." });
                continue;
            }

            if (memcmp(&solutionBT[0], &points[0], i * sizeof(uint32_t) != 0))
            {
                testResults.push_back({ "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr, FAIL, "Backtracking algorithm found wrong solution." });
                continue;
            }

            testResults.push_back(
                {
                    "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr,
                    PASS,
                    "BT = " + btTStr + "sec."
                }
            );

            testCase++;
        }
    }
}