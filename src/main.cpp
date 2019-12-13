#include "problems.h"
#include "commoninc.h"

using namespace std;

typedef void (*pfnProblem)(vector<TestResult>& testResults);

map<string, pfnProblem> problems =
{
    { "P2_1", P2_1 },
    { "P2_18", P2_18 },
};

/**
 * DisplayTestsAndExit - Print out list of available tests and exit.
 */

void DisplayTestsAndExit()
{
    printf("Available Problems:\n\n");
    for (auto& p : problems) printf("%s\n", p.first.c_str());
    exit(0);
}

/**
 * ReportTestResults - Report pass/fail statistics from list of test results and
 * print error logs.
 *
 * @param results [in] List of test results from test execution.
 */

void ReportTestResults(const vector<TestResult>& results)
{
    uint32_t passCnt    = 0;
    uint32_t failCnt    = 0;
    uint32_t execErrCnt = 0;
    uint32_t testCnt    = (uint32_t)results.size();

    for (auto& res : results)
    {
        if (res.code == PASS) passCnt++;
        if (res.code == FAIL) failCnt++;
        if (res.code == EXECUTION_ERROR) execErrCnt++;
    }

    printf(
        "Test Statistics: Pass(%u/%u), Fail(%u/%u), ExecutionError(%u/%u)\n",
        passCnt,
        testCnt,
        failCnt,
        testCnt,
        execErrCnt,
        testCnt
    );

    if (failCnt > 0)
    {
        printf("Failure Log:\n\n");

        for (auto& res : results)
            if (res.code == FAIL)
                printf("FAILED - %s, Message: %s\n", res.testName.c_str(), res.testMsg.c_str());
    }


    if (execErrCnt > 0)
    {
        printf("Execution Error Log:\n\n");

        for (auto& res : results)
            if (res.code == EXECUTION_ERROR)
                printf("EXECUTION ERROR - %s, Message: %s\n", res.testName.c_str(), res.testMsg.c_str());
    }
}

/**
 * main - Main driver routine. Run list of problems/tests specified on command line and
 * report results.
 *
 * @param  argc [in] CL argument count.
 * @param  argv [in] CL arguments.
 * @return      Zero on success.
 */

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Please specify one or more test names.\n\n");
        DisplayTestsAndExit();
    }

    vector<string> args(argv + 1, argv + argc);
    vector<TestResult> results;

    for (auto& prob : args)
    {
        if (problems.count(prob) == 0) printf("Unknown problem specified: %s\n\n", prob.c_str());
        problems[prob](results);
    }

    ReportTestResults(results);

    return 0;
}