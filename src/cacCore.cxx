#include "cacCore.h"
#include <fmt/format.h>
#include <iostream>
#include <iomanip>
#include "cvm/plusargs.hpp"

DECLARE_bool(cosim_tracer);
DEFINE_bool(cac_tracer, false, "Enable CAC trace prints");

// CacCore
CacCore::CacCore(threadT tNum):threadNum(tNum) {
  init();
};

std::string CacCore::getHello(){
    return("CacCore has been constructed!");
}

void CacCore::init() {
    for(threadT tid = 0; tid<threadNum; tid++){
        threadData.insert_or_assign(tid, ThreadData{
            .status = true,
            .stepCount = 0,
            .dutChangeCount = 0,
            .simChangeCount = 0,
            .dutRegisters = {},
            .simRegisters = RegisterSnapshot(tid),
            .registersToCheck = {}
        });
    }
}

void CacCore::reset() {
  init();
}

int CacCore::getStep(threadT threadId){
    return(threadData.at(threadId).stepCount);
};

// get if mismatch
bool CacCore::getStatus(threadT threadId){
    return(threadData.at(threadId).status);
};

std::string CacCore::getStatusStr(threadT threadId) {
    return ss.str();
}

void CacCore::resetStatus(threadT threadId){
    threadData.at(threadId).status = true;
};

// Configuration API
void CacCore::configureVlen(unsigned int vlen) {
    cfg_vlen = vlen;
    for(threadT tid = 0; tid<threadNum; tid++){
        threadData.at(tid).simRegisters.updateSize(cfg_vlen);
    }
}

unsigned int CacCore::getRegisterSize(stateIdT id) {
    if (id >= CAC_STATE_RegV0_ID && id <= CAC_STATE_RegV31_ID) {
      return cfg_vlen;
    } else {
      return supportStatesSize[id];
    }
}

// Simulator API to update Register
void CacCore::updateRefRegister(threadT threadId, unsigned int typeEncoding, unsigned int typeOffset, const std::vector<unitDataT>&& data) {
    stateIdT id = generateStateId(typeEncoding, typeOffset);
    updateRefRegister(threadId, id, std::move(data));
};

void CacCore::updateRefRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>&& data) {
    auto& threadLocal = threadData.at(threadId);
    threadLocal.simRegisters.setValue(id, std::move(data));
    ++threadLocal.simChangeCount;
};


// Dut API to update Register
void CacCore::updateRegister(threadT threadId, unsigned int typeEncoding, unsigned int typeOffset, const std::vector<unitDataT>&& data){
    stateIdT id = generateStateId(typeEncoding, typeOffset);
    return updateRegister(threadId, id, std::move(data));
};
void CacCore::updateRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>&& data){
    auto& threadLocal = threadData.at(threadId);
    auto it = threadLocal.dutRegisters.find(id);
    if (it == threadLocal.dutRegisters.end()) {
        threadLocal.dutRegisters.insert_or_assign(id, Register(threadId, id, getRegisterSize(id), std::move(data)));
    } else {
        it->second.setValue(std::move(data));
    }
    ++threadLocal.dutChangeCount;
    threadLocal.registersToCheck.push(id);
};

bool CacCore::checkRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>& data){
    return(threadData.at(threadId).simRegisters.checkValue(id, data));
};

// make a lock step
void CacCore::step(threadT threadId) {
    ss.str("");
    // use rtl changecount and check against iss snapshot
    auto& threadLocal = threadData.at(threadId);
    auto& registersToCheck = threadLocal.registersToCheck;
    const auto& dutRegisters = threadLocal.dutRegisters;
    const auto& simRegisters = threadLocal.simRegisters;
    ++threadLocal.stepCount;
    threadLocal.dutChangeCount = 0;
    threadLocal.simChangeCount = 0;
    bool firstPrint = true;
    while (!registersToCheck.empty()) {
        stateIdT id = registersToCheck.front();
        const auto& dutReg = dutRegisters.at(id);
        bool ckRst = checkRegister(threadId, id, dutReg.getValue());
        // First mismatch
        if (threadLocal.status && !ckRst) {
            ss << "\nRegister Mismatch\n";
            threadLocal.status = false;
        }
        if (FLAGS_cosim_tracer || !threadLocal.status) {
            if (firstPrint) {
                ss << fmt::format("Step: {}\n", threadLocal.stepCount);
                firstPrint = false;
            }
            // TODO(mboisvert): Can we make this better (i.e. no hardcoded widths)
            const std::string dutRegName = dutReg.getName();
            int width = 48;
            if (!dutRegName.empty() && dutRegName[0] == 'V') {
                switch (dutReg.getSize()) {
                    case VEC_128:
                        width = 71;
                        break;
                    case VEC_256:
                        width = 105;
                        break;
                    case VEC_512:
                        width = 173;
                        break;
                }
            }
            ss << fmt::format("{:>20}{:>{}}\n", dutRegName, dutReg.toString("DUT"), width);
            if (simRegisters.exists(id)) {
                ss << fmt::format("{:>20}{:>{}}\n", "", simRegisters.toString(id, "SIM"), width);
            }
        }
        registersToCheck.pop();
    }
};

// Generate State Id by type encoding and offset
// 0:RT_FIX, 1:RT_FLT, 2:RT_X, 3: RT_PAS
// GPR, FPR, CSR, Vec, PC
stateIdT CacCore::generateStateId(unsigned int typeEncoding, unsigned int typeOffset){
    if (typeEncoding == REGISTER_RT_FIX_ENCODING){
        return(CAC_STATE_RegX0_ID + typeOffset);
    } else if (typeEncoding == REGISTER_RT_FLT_ENCODING) {
        return(CAC_STATE_RegF0_ID + typeOffset);
    } else if (typeEncoding == REGISTER_RT_VEC_ENCODING) {
        return(CAC_STATE_RegV0_ID + typeOffset);
    }else{
        std::cout<<"\nError: Unknown register type encoding"<<std::endl;
        exit(1);
    }

}
