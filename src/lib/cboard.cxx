#include "cboard.h"
#include <iostream>
#include <sstream>
#include <iomanip>

std::string CBoard::getHello(){
    return("CBoard has been constructed!");
}

// Register
Register::Register(threadT tid, stateIdT rid, sizenBitT bitSize, unitDataT * data):threadId(tid), registerId(rid), size(bitSize),valueV(data, data+size/64){};

stateIdT Register::getRegisterId(){
    return(registerId);
};

void Register::setValue(unitDataT * data){
    valueV.clear();
    valueV.assign(data, data+size/64);
};

bool Register::checkValue(unitDataT * data){
    std::vector<unitDataT> valueRef;
    for (int i = 0; i<size/64; i = i + 1){
        valueRef.push_back(data[i]);
    }
    return(valueRef == valueV);
};

sizenBitT Register::getSize(){
    return(size);
};

std::vector<size8BytesT> Register::getValue(){
    return(valueV);
};

//RegisterSnapshot
RegisterSnapshot::RegisterSnapshot(threadT tid):threadId(tid){
    for(const stateIdT &suportStateId : supportStates){
        sizenBitT regSize = supportStatesSize.at(suportStateId);
        size8BytesT rstValue[] = {0x0};
        Register reg(threadId, suportStateId, regSize, rstValue);
        snapshotCol.insert_or_assign(suportStateId, reg);
    }
};

std::vector<size8BytesT> RegisterSnapshot::getValue(stateIdT id){
    Register reg = snapshotCol.at(id);
    return(reg.getValue());
};

void RegisterSnapshot::updateValue(stateIdT id, size8BytesT * data){
    Register reg = snapshotCol.at(id);
    reg.setValue(data);
    snapshotCol.at(id) = reg;
};

bool RegisterSnapshot::checkValue(stateIdT id, size8BytesT * data){
    Register reg = snapshotCol.at(id);
    return(reg.checkValue(data));
};

//Info
Info::Info(threadT tid, stateIdT stateId, const std::string &type, unitDataT * item):threadId(tid),stateId(stateId),infoType(type),item(item){
    itemName = supportStatesSymbol.at(stateId);
    std::stringstream tmpStream;
    //TODO: Need to fix for data is 128 bits
    tmpStream<<type<<":[Data:"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[0]<<"]";
    formatString = tmpStream.str();
};

std::string Info::getItemName(){
    return(itemName);
};

std::string Info::getType(){
    return(infoType);
}

unitDataT * Info::getData(){
    return(item);
};

std::string Info::getFormatInfo(){
    return(formatString);
};

//InfoCol
InfoCol::InfoCol(threadT tid, int stepNum, const std::string &type):threadId(tid),infoColType(type),stepNum(stepNum){};

std::unordered_map<std::string, Info> InfoCol::getInfoDict(){
    return(infoDict);
};

void InfoCol::outputStates(InfoCol *infoColIns){
    // std::map<std::string, Info> infoDict;
    // tmpStream<<type<<":[Data:"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[0]<<"]";
    std::unordered_map<std::string, Info> infoDictIns = infoColIns->getInfoDict();
    for(auto const & infoIt : infoDict){
        std::cout<<std::setw(20)<<infoIt.first<<std::setw(40)<<infoDict.at(infoIt.first).getFormatInfo()<<std::endl;
        std::cout<<std::setw(20)<<""<<std::setw(40)<<infoDictIns.at(infoIt.first).getFormatInfo()<<std::endl;
    }
};

void InfoCol::gatherInfo(Info & infoItem){
    std::string infoName = infoItem.getItemName();
    if (infoDict.find(infoName) != infoDict.end()){
        infoDict.at(infoName) = infoItem;
    }else{
        infoDict.insert_or_assign(infoName, infoItem);
    }
};

//Recorder
Record::Record(threadT tNum):threadNum(tNum){
    for(threadT tid = 0; tid<tNum; tid++){
        std::vector<InfoCol> infoDutList;
        recorderDutCol.insert_or_assign(tid, infoDutList);
        std::vector<InfoCol> infoSimList;
        recorderSimCol.insert_or_assign(tid, infoSimList);
    }
};

void Record::addInfo(threadT tid, bool ifdut, Info &info){
    if (ifdut == true){
        recorderDutCol.at(tid).back().gatherInfo(info);
    }else{
        recorderSimCol.at(tid).back().gatherInfo(info);
    }
};

void Record::addInfoCol(threadT tid, bool ifdut, InfoCol &col){
    if (ifdut == true){
        recorderDutCol.at(tid).push_back(col);
    }else{
        recorderSimCol.at(tid).push_back(col);
    }
};

InfoCol Record::getInfoColByStep(threadT tid, bool ifdut, int stepN){
    if (ifdut == true){
        return(recorderDutCol.at(tid).at(stepN));
    }else{
        return(recorderSimCol.at(tid).at(stepN));
    }
};

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

int CBoard::getStep(threadT threadId){
    return(stepCount.at(threadId));
};

bool CBoard::getStatus(threadT threadId){
    return(status.at(threadId));
};

void CBoard::updateRefRegister(threadT threadId, stateIdT id, unitDataT * data){
    registerSnapshot.at(threadId).updateValue(id, data);
    Info infoIns(threadId, id, "SIM", data); 
    record->addInfo(threadId, false, infoIns);
};

void CBoard::updateRegister(threadT threadId, stateIdT id, unitDataT * data){
    Info infoIns(threadId, id, "DUT", data);
    record->addInfo(threadId, true, infoIns);
    Register reg(threadId, id, supportStatesSize.at(id), data);
    checkingBuffer.at(threadId).push_back(reg);
};

bool CBoard::checkRegister(threadT threadId, stateIdT id, unitDataT * data){
    return(registerSnapshot.at(threadId).checkValue(id, data));
};

// double *d_arr = arr.data();
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