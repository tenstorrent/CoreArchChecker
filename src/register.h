#ifndef REGISTER_H
#define REGISTER_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "cboardlib.h"

class Register
{
    public:
        Register(threadT tid, stateIdT rid, sizenBitT bitSize, unitDataT * data);
        void setValue(unitDataT * data);
        sizenBitT getSize();
        threadT getThreadId();
        stateIdT getRegisterId();
        std::vector<unitDataT> getValue();
        bool checkValue(unitDataT * data);
    private:
        stateIdT registerId;
        threadT threadId;
        sizenBitT size;
        std::vector<unitDataT> valueV;
};

class RegisterSnapshot
{
    public:
        RegisterSnapshot(threadT tid);
        std::vector<unitDataT> getValue(stateIdT id);
        void updateValue(stateIdT id, unitDataT * data);
        bool checkValue(stateIdT id, unitDataT * data);
    private:
        threadT threadId;
        std::unordered_map<stateIdT, Register> snapshotCol;
};

#endif
