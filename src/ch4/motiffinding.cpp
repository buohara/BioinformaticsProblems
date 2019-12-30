#include "problems.h"

static void GenerateMotifSequences(uint32_t nSeq, uint32_t seqLen, uint32_t motifLen, vector<string>& seqs, string& motif, vector<uint32_t> &offsets)
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
        seqs.push_back(curSeq);
    }
}

struct SearchNode
{
    uint32_t offsetVal;
    vector<bool> childOffsetSearched;
    SearchNode(uint32_t curOFfset, uint32_t numChildren) : offsetVal(curOFfset) { childOffsetSearched.resize(numChildren, false); }
};

uint32_t GetConsensus(vector<string>& seqs, vector<uint32_t>& offsets, uint32_t motifLen)
{
    vector<uint32_t> aScore(motifLen, 0);
    vector<uint32_t> tScore(motifLen, 0);
    vector<uint32_t> cScore(motifLen, 0);
    vector<uint32_t> gScore(motifLen, 0);

    uint32_t consensus = 0;

    for (uint32_t i = 0; i < seqs.size(); i++)
    {
        for (uint32_t j = 0; j < motifLen; j++)
        {
            char base = seqs[i][offsets[i] + j];

            switch (base)
            {
            case 'A':

                aScore[j]++;
                break;

            case 'T':

                tScore[j]++;
                break;

            case 'C':

                cScore[j]++;
                break;

            case 'G':

                gScore[j]++;
                break;

            default:
                break;
            }
        }
    }

    for (uint32_t i = 0; i < motifLen; i++)
    {
        uint32_t best = aScore[i];
        if (tScore[i] > best) best = tScore[i];
        if (cScore[i] > best) best = cScore[i];
        if (gScore[i] > best) best = gScore[i];

        consensus += best;
    }

    return consensus;
}

ResultCode FindMotif(vector<string>& seqs, uint32_t motifLen, vector<uint32_t>& offsets)
{
    vector<SearchNode> stack;

    const uint32_t seqLen       = seqs[0].length();
    const uint32_t offsetRange  = seqLen - motifLen;

    offsets.resize(seqs.size(), 0);

    uint32_t bestConsensus = 0;

    for (uint32_t i = 0; i < seqs.size(); i++)
    {
        stack.push_back(SearchNode(i, offsetRange));

        while (!stack.empty())
        {
            if (stack.size() == seqs.size())
            {
                vector<uint32_t> curOffsets(seqs.size(), 0);
                for (uint32_t j = 0; j < stack.size(); j++) curOffsets[j] = stack[j].offsetVal;
                uint32_t consensusScore = GetConsensus(seqs, curOffsets, motifLen);

                if (consensusScore > bestConsensus)
                {
                    bestConsensus = consensusScore;
                    memcpy(&offsets[0], &curOffsets[0], seqs.size() * sizeof(uint32_t));
                }

                stack.pop_back();
                continue;
            }

            auto& cur = stack.back();

            bool allChildrenSearched = true;

            for (uint32_t j = 0; j < cur.childOffsetSearched.size(); j++)
            {
                if (!cur.childOffsetSearched[j])
                {
                    cur.childOffsetSearched[j] = true;
                    stack.push_back(SearchNode(j, offsetRange));
                    allChildrenSearched = false;
                    break;
                }
            }

            if (allChildrenSearched) stack.pop_back();
        }
    }

    return OK;
}

void MotifFinding(vector<TestResult>& testResults)
{
    const uint32_t nSeq     = 6;
    const uint32_t seqLen   = 10;
    const uint32_t motifLen = 6;

    vector<string> seq;
    string motif;
    vector<uint32_t> offsets;

    GenerateMotifSequences(nSeq, seqLen, motifLen, seq, motif, offsets);

    vector<uint32_t> resultOffsets;

    FindMotif(seq, motifLen, resultOffsets);

    __debugbreak();
}