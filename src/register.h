#ifndef REGISTER_H
#define REGISTER_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "caclib.h"

class Register
{
    public:
        Register(threadT tid, stateIdT rid, sizenBitT bitSize, const std::vector<unitDataT>&& data);
        stateIdT getRegisterId() const;
        std::string getName() const;
        const std::vector<unitDataT>& getValue() const;
        void setValue(const std::vector<unitDataT>&& data);
        bool checkValue(const std::vector<unitDataT>& data) const;
        sizenBitT getSize() const;
        void updateSize(sizenBitT sz);
        std::string toString(const std::string& type) const;
    private:
        threadT threadId;
        stateIdT registerId;
        sizenBitT size;
        std::vector<unitDataT> valueV;
};

class RegisterSnapshot
{
    public:
        RegisterSnapshot(threadT tid);
        bool exists(stateIdT id) const;
        std::string getName(stateIdT id) const;
        const std::vector<unitDataT>& getValue(stateIdT id) const;
        void setValue(stateIdT id, const std::vector<unitDataT>&& data);
        bool checkValue(stateIdT id, const std::vector<unitDataT>& data) const;
        void updateSize(unsigned int vlen);
        std::string toString(stateIdT id, const std::string& type) const;
    private:
        threadT threadId;
        std::unordered_map<stateIdT, Register> snapshotCol;
};

#endif
