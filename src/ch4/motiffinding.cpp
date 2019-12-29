#include "problems.h"

static void GenerateMotifSequences(uint32_t nSeq, uint32_t seqLen, uint32_t motifLen, vector<string>& seq, string& motif, vector<uint32_t> &offsets)
{
    const char bases[4] = { 'A', 'C', 'T', 'G' };
    
    for (uint32_t i = 0; i < motifLen; i++) motif += bases[rand() % 4];

    const uint32_t offsetRange = seqLen - motifLen;

    for (uint32_t i = 0; i < nSeq; i++)
    {
        string curSeq = "";
        for (uint32_t j = 0; j < seqLen; j++) curSeq += bases[rand() % 4];

        offsets.push_back(rand() % offsetRange);
        memcpy(&curSeq[0] + offsets[i], &motif[0], motifLen);
        seq.push_back(curSeq);
    }
}

void MotifFinding(vector<TestResult>& testResults)
{
    const uint32_t nSeq     = 20;
    const uint32_t seqLen   = 100;
    const uint32_t motifLen = 6;

    vector<string> seq;
    string motif;
    vector<uint32_t> offsets;

    GenerateMotifSequences(nSeq, seqLen, motifLen, seq, motif, offsets);

    __debugbreak();
}