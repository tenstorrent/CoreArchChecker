#include "register.h"
#include <algorithm>
#include <iostream>
#include <vector>
// Register
Register::Register(threadT tid, stateIdT rid, sizenBitT bitSize, const std::vector<unitDataT>&& data):threadId(tid), registerId(rid), size(bitSize),valueV(data){};

stateIdT Register::getRegisterId(){
    return(registerId);
};

void Register::setValue(const std::vector<unitDataT>&& data){
    valueV = data;
};

bool Register::checkValue(const std::vector<unitDataT>& data){
    auto elems_to_cmp = size/64;
    return std::equal(data.begin(), data.begin() + elems_to_cmp, valueV.begin(), valueV.begin() + elems_to_cmp);
};

sizenBitT Register::getSize(){
    return(size);
};

void Register::updateSize(sizenBitT sz){
  size = sz;
  valueV.resize(size/64, 0);
};

std::vector<unitDataT>& Register::getValue(){
    return(valueV);
};

//RegisterSnapshot
RegisterSnapshot::RegisterSnapshot(threadT tid):threadId(tid){
    for(const stateIdT &supportStateId : supportStates){
        sizenBitT regSize = supportStatesSize[supportStateId];
        std::vector<size8BytesT> rstValue(regSize / 64, 0x0);
        Register reg(threadId, supportStateId, regSize, std::move(rstValue));
        snapshotCol.insert_or_assign(supportStateId , reg);
    }
};

std::vector<size8BytesT>& RegisterSnapshot::getValue(stateIdT id){
    Register& reg = snapshotCol.at(id);
    return(reg.getValue());
};

void RegisterSnapshot::updateSize(unsigned int vlen){
    for(const stateIdT &id : supportStates){
      if (id >= CAC_STATE_RegV0_ID && id <= CAC_STATE_RegV31_ID) {
        Register& reg = snapshotCol.at(id);
        reg.updateSize(vlen);
      }
    }
}

void RegisterSnapshot::updateValue(stateIdT id, const std::vector<unitDataT>&& data){
    Register& reg = snapshotCol.at(id);
    reg.setValue(std::move(data));
};

bool RegisterSnapshot::checkValue(stateIdT id, const std::vector<unitDataT>& data){
    Register& reg = snapshotCol.at(id);
    return(reg.checkValue(data));
};
