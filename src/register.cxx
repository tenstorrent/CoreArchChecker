#include "register.h"
#include <algorithm>
#include <fmt/format.h>
#include <iostream>
#include <vector>
// Register
Register::Register(threadT tid, stateIdT rid, sizenBitT bitSize, const std::vector<unitDataT>&& data):threadId(tid), registerId(rid), size(bitSize),valueV(data){};

stateIdT Register::getRegisterId() const {
    return(registerId);
};

std::string Register::getName() const {
    return supportStatesSymbol[registerId];
}

const std::vector<unitDataT>& Register::getValue() const {
    return(valueV);
}

void Register::setValue(const std::vector<unitDataT>&& data) {
    valueV = data;
};

bool Register::checkValue(const std::vector<unitDataT>& data) const {
    auto elems_to_cmp = size/64;
    return std::equal(data.begin(), data.begin() + elems_to_cmp, valueV.begin(), valueV.begin() + elems_to_cmp);
};

sizenBitT Register::getSize() const {
    return(size);
};

void Register::updateSize(sizenBitT sz){
  size = sz;
  valueV.resize(size/64, 0);
};

std::string Register::toString(const std::string& type) const {
    return fmt::format("{}:[Data:{:0{}x}]({})", type, fmt::join(valueV, "_"), sizeof(unitDataT)*2, size);
}

//RegisterSnapshot
RegisterSnapshot::RegisterSnapshot(threadT tid):threadId(tid) {
    for(const stateIdT &supportStateId : supportStates){
        sizenBitT regSize = supportStatesSize[supportStateId];
        std::vector<size8BytesT> rstValue(regSize / 64, 0x0);
        Register reg(threadId, supportStateId, regSize, std::move(rstValue));
        snapshotCol.insert_or_assign(supportStateId , reg);
    }
};

bool RegisterSnapshot::exists(stateIdT id) const {
    return (snapshotCol.find(id) != snapshotCol.end());
}

std::string RegisterSnapshot::getName(stateIdT id) const {
    return snapshotCol.at(id).getName();
}

const std::vector<unitDataT>& RegisterSnapshot::getValue(stateIdT id) const {
    return snapshotCol.at(id).getValue();
};

void RegisterSnapshot::setValue(stateIdT id, const std::vector<unitDataT>&& data) {
    snapshotCol.at(id).setValue(std::move(data));
};

bool RegisterSnapshot::checkValue(stateIdT id, const std::vector<unitDataT>& data) const {
    return snapshotCol.at(id).checkValue(data);
};

void RegisterSnapshot::updateSize(unsigned int vlen) {
    for(const stateIdT &id : supportStates){
      if (id >= CAC_STATE_RegV0_ID && id <= CAC_STATE_RegV31_ID) {
        Register& reg = snapshotCol.at(id);
        reg.updateSize(vlen);
      }
    }
}

std::string RegisterSnapshot::toString(stateIdT id, const std::string& type) const {
    return snapshotCol.at(id).toString(type);
}
