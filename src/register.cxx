#include "register.h"
#include <iostream>
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
    //for(const stateIdT &suportStateId : supportStates){ Not sure why DPI complain about it
    unsigned int numOfSates = sizeof(supportStates)/sizeof(supportStates[0]);
    for (unsigned int idIndex = 0; idIndex<numOfSates; idIndex = idIndex + 1){
        sizenBitT regSize = supportStatesSize.at(supportStates[idIndex]);
        size8BytesT rstValue[] = {0x0};
        Register reg(threadId, supportStates[idIndex], regSize, rstValue);
        snapshotCol.insert_or_assign(supportStates[idIndex], reg);
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
