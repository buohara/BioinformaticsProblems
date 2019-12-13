#include "problems.h"

static struct Professors
{
    bool honest[100];

    Professors()
    {
        memset(honest, false, 100 * sizeof(bool));
        uint32_t honestCnt = 50 + rand() % 51;

        while (honestCnt > 0)
        {
            uint32_t curIdx = rand() % 100;
            if (!honest[curIdx])
            {
                honest[curIdx] = true;
                honestCnt--;
            }
        }
    }

    bool QueryHonesty(uint32_t a, uint32_t b) const
    {
        if (honest[a]) return honest[b];
        else return rand() % 2 == 0;
    }
};

static void DetermineHonestProfessors(const Professors& profs, bool honest[100])
{
    uint32_t honestProf = ~0;

    for (uint32_t i = 0; i < 50; i++)
    {
        if (profs.QueryHonesty(2 * i, 2 * i + 1) && profs.QueryHonesty(2 * i + 1, 2 * i))
        {
            honestProf          = 2 * i;
            honest[honestProf]  = true;
            break;
        }
    }

    for (uint32_t i = 0; i < 100; i++)
    {
        if (i == honestProf) continue;
        honest[i] = profs.QueryHonesty(honestProf, i);
    }
}

void P2_18(vector<TestResult>& testResults)
{
    const uint32_t numIters = 100;

    for (uint32_t i = 0; i < numIters; i++)
    {
        Professors profs;
        bool honestResults[100];
        DetermineHonestProfessors(profs, honestResults);

        bool correctAnswer = true;

        for (uint32_t j = 0; j < 100; j++)
        {
            if (honestResults[j] != profs.honest[j])
            {
                correctAnswer = false;
                break;
            }
        }

        if (correctAnswer)
            testResults.push_back({ "HonestProfs[" + to_string(i) + "]", PASS, "" });
        else
            testResults.push_back({ "HonestProfs[" + to_string(i) + "]", FAIL, "" });
    }
}