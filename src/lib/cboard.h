#ifndef CBOARD_CORE_H
#define CBOARD_CORE_H

#include <map>
#include <string>
#include <vector>
#include "cboardTypeDef.h"
#include "stateIdDef.h"
#include "external.h"

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
        std::map<stateIdT, Register> snapshotCol;
};

class Info
{
    public:
        Info(threadT tid, stateIdT stateId, std::string type, unitDataT * item);
        std::string getItemName();
        unitDataT * getData();
        std::string getFormatInfo();
        std::string getType();
    private:
        threadT threadId;
        stateIdT stateId;
        std::string infoType;
        std::string itemName;
        unitDataT * item;
        std::string formatString;
};

class InfoCol
{
    public:
        InfoCol(threadT tid, int stepNum, std::string type);
        std::map<std::string, Info> getInfoDict();
        void gatherInfo(Info infoItem);
        void outputStates(InfoCol *infoColIns);
    private:
        threadT threadId;
        std::string infoColType;
        std::map<std::string, Info> infoDict;
        int stepNum;
};

class Record
{
    public:
        Record(threadT tNum);
        void addInfoCol(threadT tid, bool ifdut, InfoCol col);
        void addInfo(threadT tid, bool ifdut, Info info);
        InfoCol getInfoColByStep(threadT tid, bool ifdut, int stepN);
    private:
        threadT threadNum;
        std::map<threadT, std::vector<InfoCol>> recorderDutCol;
        std::map<threadT, std::vector<InfoCol>> recorderSimCol;
};

class CBoard
{
    public:
        CBoard(threadT tNum);
        std::string getHello();
        void updateRegister(threadT threadId, stateIdT id, unitDataT * data);
        void updateRefRegister(threadT threadId, stateIdT id, unitDataT * data);
        bool checkRegister(threadT threadId, stateIdT id, unitDataT * data);
        // TODO: fuzz mask
        // void updateRegister(threadT threadId, stateIdT id, unitDataT * data, fuzzMaskT fuzzMask);
        //void updateMem(threadT threadId);
        void step(threadT threadId);
        int getStep(threadT threadId);
        bool getStatus(threadT threadId);
    private:
        threadT threadNum;
        Record *record;
        std::map<threadT, bool> status;
        std::map<threadT, int> stepCount;
        std::map<threadT, RegisterSnapshot> registerSnapshot;
        std::map<threadT, std::vector<Register>> checkingBuffer;
};

#endif