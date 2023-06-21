#ifndef CAC_CORE_H
#define CAC_CORE_H

#include <map>
#include <queue>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "caclib.h"
#include "register.h"

struct ThreadData {
    bool status;
    int stepCount;
    int dutChangeCount;
    int simChangeCount;
    std::unordered_map<stateIdT, Register> dutRegisters;
    RegisterSnapshot simRegisters;
    std::queue<stateIdT> registersToCheck;
};

class CacCore
{
    public:
        // Constructor
        CacCore(threadT tNum);
        // Hello World function to make unit test work
        std::string getHello();
        // Reset function to clear state
        void init();
        void reset();
        // Configuration API
        void configureVlen(unsigned int vlen);
        // Dut API to update Register
        void updateRegister(threadT threadId, unsigned int typeEncoding, unsigned int typeOffset, const std::vector<unitDataT>&& data);
        void updateRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>&& data);
        // Simulator API to update Register
        void updateRefRegister(threadT threadId, unsigned int typeEncoding, unsigned int typeOffset, const std::vector<unitDataT>&& data);
        void updateRefRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>&& data);
        // Make a lock step
        void step(threadT threadId);
        // Api to get which step it is
        int getStep(threadT threadId);
        // Api to get the check result
        bool getStatus(threadT threadId);
        std::string getStatusStr(threadT threadId);
        std::string getResourceStr(threadT threadId);
        void resetStatus(threadT threadId);
    private:
        threadT threadNum;
        unsigned int cfg_vlen = VEC_128;
        std::unordered_map<threadT, ThreadData> threadData;
        stateIdT generateStateId(unsigned int typeEncoding, unsigned int typeOffset);
        bool checkRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>& data);
        unsigned int getRegisterSize(stateIdT id);
        std::ostringstream ss;
        std::string resourceStr = "";
};

#endif
