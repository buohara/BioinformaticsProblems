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
 * IsSubset - Determine if s1 is a subset of s2. Assumes s1 and s2 are sorted in ascending
 * order.
 *
 * @param  s1 First set. Elements assumed to be in ascending order.
 * @param  s2 Second set. Elements assumed to be in ascending order.
 *
 * @return    True if s1 is a subset of s2.
 */

static bool IsSubset(const vector<uint32_t>& s1, const vector<uint32_t>& s2)
{
    if (s1.size() == 0) return true;
    if (s1.size() > s2.size()) return false;

    uint32_t found = 0;

    for (uint32_t i = 0; i < s2.size(); i++)
        if (s1[found] == s2[i])
            found++;

    if (found == s1.size()) return true;
    return false;
}

/**
 * ComputePDBackTracking - Compute the partial digest PD of an input list L of pairwise
 * distances. PD is a set of points such that the distance between each pair of its points is L. 
 *
 * This algorithm iteratively grabs the largest unused distance from the input list and computes
 * points that are this distance from the left/or right ends of PD (the endpoints of PD must be {0, max(L)). 
 * It then generates a list of pairwise distances for all points currently computed. If these computed
 * distances are part of the input distance list, accept the new point and move on to the next
 * largest distance. Otherwise, remove the current point. If a previous distance hasn't been checked
 * from both ends of PD, check the other end. Otherwise, this distance doesn't work in the current solution,
 * so back up and check previous distances. Algorithm terminates if a computed distance set matches the input
 * list, or all combinations of points have been tested and no solution found.
 *
 * @param  list [in] Input set of pairwise distances between points. Assumed to be sorted in ascending order.
 * @param  pd   [in/out] The computed set of points PD from L. Assumed empty on input.
 *
 * @return      INVALID_INPUT if no distances input. OK if solution found, UNABLE_TO_FIND_SOLUTION otherwise.
 */

static ResultCode ComputePDBacktracking(const vector<uint32_t>& distList, vector<uint32_t>& pd)
{
    assert(pd.size() == 0);
    if (distList.size() == 0) return INVALID_INPUT;

    const uint32_t distMax = distList[distList.size() - 1];

    struct SearchNode
    {
        uint32_t curPoint;
        int32_t nextIdx;
        bool searchedLeft;
        bool searchedRight;
    };

    vector<SearchNode> searchStack;
    searchStack.push_back({ distMax, (int32_t)distList.size() - 2, false, false });

    while (!searchStack.empty())
    {
        auto& cur = searchStack.back();

        if (cur.searchedLeft == false && cur.searchedRight == false)
        {
            vector<uint32_t> pdCand = { 0, distMax };
            for (uint32_t i = 1; i < searchStack.size(); i++) pdCand.push_back(searchStack[i].curPoint);
            sort(pdCand.begin(), pdCand.end());

            vector<uint32_t> candDist;
            GetPairwiseDistances(pdCand, candDist);

            if (!IsSubset(candDist, distList))
            {
                searchStack.pop_back();
                continue;
            }
            else
            {
                if (candDist.size() == distList.size())
                {
                    pd.resize(pdCand.size());
                    memcpy(&pd[0], &pdCand[0], pdCand.size() * sizeof(uint32_t));
                    return OK;
                }
            }
        }

        while (cur.nextIdx >= 0)
        {
            if (cur.searchedLeft == false)
            {
                cur.searchedLeft = true;
                searchStack.push_back({ distMax - distList[cur.nextIdx], cur.nextIdx - 1, false, false });
                break;
            }

            else if (cur.searchedRight == false)
            {
                cur.searchedRight = true;
                searchStack.push_back( {distList[cur.nextIdx], cur.nextIdx - 1, false, false });
                break;
            }

            else
            {
                cur.nextIdx--;
                cur.searchedLeft    = false;
                cur.searchedRight   = false;
            }
        }

        if (cur.nextIdx == -1)
            searchStack.pop_back();
    }

    return UNABLE_TO_FIND_SOLUTION;
}

/**
 * P4_2 - Restriction mapping problems. The restriction mapping/turnpike problem asks, given
 * a list of pairwise distances between points, can we reconstruct the original set of points.
 * This routine tests two algorithms that solve this problem, one a brute-force technique and the other
 * a backtracking tecnique (see ComputePDBF and ComputePDBackTracking above, respectively).
 * 
 * This routine randomly generates 100 lists of points for power-of-two list sizes between 2 and 2048.
 * It generates the distance list for these points, then times each algorithm's solution. Tests
 * pass if the algorithms reproduce the original point set. Tests pass, but a warning is given if the
 * brute-force algorithm outperforms the backtracking algorithm.
 *
 * @param testResults Result list to append results to.
 */

void P4_2(vector<TestResult>& testResults)
{
    const uint32_t itersPerSize = 100;
    const uint32_t maxListSize  = 2048;
    const uint32_t maxVal       = 10000;

    uint32_t testCase = 0;

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

            /*
            Brute force search complexity grows exponentially and slows down quickly. For example, for 8 points, 
            we get 28 pair-wise distances. The algorithm has to potentially search through 26 choose 6 = 230230 
            potential combinations of point candidates to find a solution. For now, omit these test cases.

            vector<uint32_t> solutionBF;
            long long t1BF      = GetMilliseconds();
            ResultCode resBF    = OK;ComputePDBF(dist, solutionBF);
            long long t2BF      = GetMilliseconds();
            float bfSec         = ((float)t2BF - (float)t1BF) / 1000.0f;
            */

            vector<uint32_t> solutionBT;
            long long t1BT      = GetMilliseconds();
            ResultCode resBT    = ComputePDBacktracking(dist, solutionBT);
            long long t2BT      = GetMilliseconds();
            float btSec         = ((float)t2BT - (float)t1BT) / 1000.0f;

            string testStr      = to_string(testCase);
            string sizeStr      = to_string(i);
            //string bfTStr       = to_string(bfSec);
            string btTStr       = to_string(btSec);

            /*if (resBF == UNABLE_TO_FIND_SOLUTION)
            {
                testResults.push_back({ "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr, FAIL, "Brute force algorithm couldn't find solution." });
                continue;
            }*/

            if (resBT == UNABLE_TO_FIND_SOLUTION)
            {
                testResults.push_back({ "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr, FAIL, "Backtracking algorithm couldn't find solution." });
                continue;
            }

            /*if (memcmp(&solutionBF[0], &points[0], i * sizeof(uint32_t) != 0))
            {
                testResults.push_back({ "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr, FAIL, "Brute force algorithm found wrong solution." });
                continue;
            }*/

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

            /*if (bfSec < btSec)
            {
                testResults.push_back(
                    { 
                        "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr, 
                        PASS, 
                        "Warning: Brute force outperformed backtracking: BF = " + bfTStr + "sec, BT = " + btTStr + "sec."
                    }
                );
            }
            else
            {
                testResults.push_back(
                    {
                        "RestMap::RandomList[" + testStr + "] Point Set Size =" + sizeStr,
                        PASS,
                        "BF = " + bfTStr + "sec, BT = " + btTStr + "sec."
                    }
                );
            }*/

            testCase++;
        }
    }
}