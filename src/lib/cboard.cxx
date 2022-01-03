#include "cboard.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// CBoard
CBoard::CBoard(threadT tNum):threadNum(tNum){
    record = new Record(threadNum);
    for(threadT tid = 0; tid<tNum; tid++){
        RegisterSnapshot regSnpSt(tid);
        registerSnapshot.insert_or_assign(tid, regSnpSt);
        std::vector<Register> ckBuff;
        checkingBuffer.insert_or_assign(tid, ckBuff);
        stepCount.insert_or_assign(tid, 0);
        status.insert_or_assign(tid, true);

        InfoCol dutInfoColIns(tid, stepCount.at(tid), "DUT");
        record->addInfoCol(tid, true, dutInfoColIns);
        InfoCol simInfoColIns(tid, stepCount.at(tid), "SIM");
        record->addInfoCol(tid, false, simInfoColIns);
    }
};

std::string CBoard::getHello(){
    return("CBoard has been constructed!");
}

int CBoard::getStep(threadT threadId){
    return(stepCount.at(threadId));
};

// get if mismatch
bool CBoard::getStatus(threadT threadId){
    return(status.at(threadId));
};

// Simulator API to update Register
void CBoard::updateRefRegister(threadT threadId, stateIdT id, unitDataT * data){
    registerSnapshot.at(threadId).updateValue(id, data);
    Info infoIns(threadId, id, "SIM", data); 
    record->addInfo(threadId, false, infoIns);
};

// Dut API to update Register
void CBoard::updateRegister(threadT threadId, stateIdT id, unitDataT * data){
    Info infoIns(threadId, id, "DUT", data);
    record->addInfo(threadId, true, infoIns);
    Register reg(threadId, id, supportStatesSize.at(id), data);
    checkingBuffer.at(threadId).push_back(reg);
};

bool CBoard::checkRegister(threadT threadId, stateIdT id, unitDataT * data){
    return(registerSnapshot.at(threadId).checkValue(id, data));
};

// make a lock step
void CBoard::step(threadT threadId){
    //RegisterSnapshot regSnpSt = registerSnapshot.at(threadId);
    std::vector<Register> buffer = checkingBuffer.at(threadId);
    bool tmpResult = true;
    for (std::vector<Register>::iterator it = buffer.begin(); it != buffer.end(); ++it) {
        std::vector<size8BytesT> reg = it->getValue();
        unitDataT *dat = reg.data();
        bool ckRst;
        ckRst = checkRegister(threadId, it->getRegisterId(), dat);
        status.at(threadId) = status.at(threadId) && ckRst;
    }
    //print out
    if (status.at(threadId) == false){
        std::cout<<"Register Mismatch"<<std::endl;
    }
    std::cout<<"Step: "<<stepCount.at(threadId)<<std::endl;
    InfoCol dutInfoColDebug = record->getInfoColByStep(threadId, true, stepCount.at(threadId));
    InfoCol simInfoColDebug = record->getInfoColByStep(threadId, false, stepCount.at(threadId));
    dutInfoColDebug.outputStates(&simInfoColDebug);

    stepCount.at(threadId) = stepCount.at(threadId) + 1;
    checkingBuffer.at(threadId).clear();

    InfoCol dutInfoColIns(threadId, stepCount.at(threadId), "DUT");
    record->addInfoCol(threadId, true, dutInfoColIns);
    InfoCol simInfoColIns(threadId, stepCount.at(threadId), "SIM");
    record->addInfoCol(threadId, false, simInfoColIns);
};