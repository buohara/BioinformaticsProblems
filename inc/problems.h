#pragma once

#include "commoninc.h"
#include "utils.h"

using namespace std;

enum TestResultCode
{
    PASS,
    FAIL,
    EXECUTION_ERROR
};

struct TestResult
{
    string testName;
    TestResultCode code;
    string testMsg;
};

void GetMinMax(vector<TestResult> &testResults);
void HonestProfessors(vector<TestResult>& testResults);

void RestrictionMapping(vector<TestResult>& testResults);
void MotifFinding(vector<TestResult>& testResults);