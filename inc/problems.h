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

void P2_1(vector<TestResult> &testResults);
void P2_18(vector<TestResult>& testResults);

void P4_2(vector<TestResult>& testResults);