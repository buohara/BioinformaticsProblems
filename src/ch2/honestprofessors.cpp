#include "problems.h"

const uint32_t PROFCNT = 100;
const uint32_t INVALID = ~0;

struct Professors
{
    bool honest[PROFCNT];
    uint32_t queryCnt;

    Professors() : queryCnt(0)
    {
        memset(honest, false, PROFCNT * sizeof(bool));
        uint32_t honestCnt = (PROFCNT / 2) + rand() % (PROFCNT / 2);

        while (honestCnt > 0)
        {
            uint32_t curIdx = rand() % PROFCNT;
            if (!honest[curIdx])
            {
                honest[curIdx] = true;
                honestCnt--;
            }
        }
    }

    
    /**
     * GetQueryCnt - Return how many times this professor set has been queried.
     * @return Number of times the set has been queried.
     */
    
    uint32_t GetQueryCnt() { return queryCnt; }

    /**
     * QueryHonesty - Check if professor a things professor b is honest. If a is honest,
     * returns if b is actually honest. If b is dishonest, returns random true/false.
     *
     * @param  a Professor to query.
     * @param  b Professor to query about.
     * @return   Whether professor a considers professor b honest.
     */
    
    bool QueryHonesty(uint32_t a, uint32_t b)
    {
        queryCnt++;
        if (honest[a]) return honest[b];
        else return rand() % 2 == 0;
    }
};

/**
 * DetermineHonestProfessors - Given a group of honest/dishonest professors that can
 * query each other's honesty, determine which ones are and are not honest.
 * 
 * Algorithm uses observation that if professors p1 and p2 query each other, the result
 * can only be (H, H) if both are honest or both are dishonest. So, pair off
 * professors, have them query each other, and save the (H, H) pairs. Pick one professor
 * from each of these pairs and create new pairs, and iterate again. Since there are
 * most honest professors to begin, each step of this process will remove dishonest professors
 * until only one or two honest professors remain.
 *
 * This algorithm doesn't satisfy the < 198 query max condition. I'll have to keep thinking
 * to come up with a better algorithm.
 *
 * @param profs  [in] List of professors that can query each other's honesty.
 * @param honest [in/out] List of whether each professor is honest as determined by this algorihtm.
 */

static void DetermineHonestProfessors(Professors& profs, bool honest[PROFCNT])
{
    memset(honest, false, PROFCNT * sizeof(bool));

    vector<pair<uint32_t, uint32_t>> pairs;
    for (uint32_t i = 0; i < PROFCNT / 2; i++) pairs.push_back({ 2 * i, 2 * i + 1 });

    uint32_t honestProf = INVALID;

    while (1)
    {
        vector<uint32_t> honestCandidates;

        for (auto& pair : pairs)
        {
            bool b1 = profs.QueryHonesty(pair.first, pair.second);
            bool b2 = profs.QueryHonesty(pair.second, pair.first);

            if (b1 && b2)
                honestCandidates.push_back(rand() % 2 == 0 ? pair.first : pair.second);
        }

        if (honestCandidates.size() <= 2)
        {
            honestProf          = honestCandidates[0];
            honest[honestProf]  = true;
            break;
        }

        pairs.resize(0);

        for (uint32_t i = 0; i < honestCandidates.size() / 2; i++)
            pairs.push_back({ honestCandidates[2 * i], honestCandidates[2 * i + 1] });
    }

    for (uint32_t i = 0; i < PROFCNT; i++)
        if (i != PROFCNT)
            honest[i] = profs.QueryHonesty(honestProf, i);
}

/**
 * HonestProfessors - Honest/deceitful professor problem test. Randomly generate 100 professors with
 * different honesties/dishonesties, then run the solution algorithm. Test passes if algorithm
 * correctly determines the honest professors in 198 queries or less. Fails if result is incorrect
 * or uses too many queries.
 *
 * @param testResults [in/out] Test result list to append to.
 */

void HonestProfessors(vector<TestResult>& testResults)
{
    const uint32_t numIters = 1000;

    srand((uint32_t)time(NULL));

    for (uint32_t i = 0; i < numIters; i++)
    {
        Professors profs;
        bool honestResults[100];
        DetermineHonestProfessors(profs, honestResults);

        bool correctAnswer      = true;
        bool belowMaxQueries    = true;

        for (uint32_t j = 0; j < 100; j++)
        {
            if (honestResults[j] != profs.honest[j])
            {
                correctAnswer = false;
                break;
            }
        }

        if (profs.GetQueryCnt() > 198) belowMaxQueries = false;

        if (correctAnswer && belowMaxQueries)
            testResults.push_back({ "HonestProfs[" + to_string(i) + "]", PASS, "" });
        else if (!correctAnswer)
            testResults.push_back({ "HonestProfs[" + to_string(i) + "]", FAIL, "Algorithm produced incorrect result." });
        else
            testResults.push_back({ "HonestProfs[" + to_string(i) + "]", FAIL, "Algorithm used too many queries." });
    }
}