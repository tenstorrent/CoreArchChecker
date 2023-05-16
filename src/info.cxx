#include "info.h"
#include "cvm/plusargs.hpp"
#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <iomanip>

DECLARE_bool(cosim_tracer);

//Info
Info::Info(threadT tid, stateIdT stateId, const std::string &type, const std::vector<unitDataT>& item, unsigned int size):threadId(tid),stateId(stateId),infoType(type),size(size){
    itemName = supportStatesSymbol[stateId];
    const size_t sizeBytes = size / 8;
    assert((sizeBytes / sizeof(unitDataT)) == item.size());
    formatString = FLAGS_cosim_tracer ? fmt::format("{}:[Data:{:0{}x}]({})", type, fmt::join(item, "_"), sizeof(unitDataT)*2, size) : "";
};

std::string Info::getItemName(){
    return(itemName);
};

std::string Info::getType(){
    return(infoType);
}

std::string Info::getFormatInfo(){
    return(formatString);
};

unsigned int Info::getSize(){
    return(size);
}

//InfoCol
InfoCol::InfoCol(threadT tid, int stepNum, const std::string &type):threadId(tid),infoColType(type),stepNum(stepNum){};

std::unordered_map<std::string, Info> InfoCol::getInfoDict(){
    return(infoDict);
};

void InfoCol::outputStates(std::ostringstream &ss, InfoCol *infoColIns){
    if (!FLAGS_cosim_tracer) {
        return;
    }
    std::unordered_map<std::string, Info> infoDictIns = infoColIns->getInfoDict();
    // TODO(mboisvert): Can we make this better (i.e. no hardcoded widths)
    for(auto & infoIt : infoDict){
        int width = 48;
        if (infoIt.first.substr(0,1) == "V") {
          if (infoIt.second.getSize() == VEC_128) {
            width = 71;
          } else if (infoIt.second.getSize() == VEC_256) {
            width = 105;
          } else if (infoIt.second.getSize() == VEC_512) {
            width = 173;
          }
        }
        ss<<std::setw(20)<<infoIt.first<<std::setw(width)<<infoDict.at(infoIt.first).getFormatInfo()<<std::endl;
        if (infoDictIns.find(infoIt.first) != infoDictIns.end())
          ss<<std::setw(20)<<""<<std::setw(width)<<infoDictIns.at(infoIt.first).getFormatInfo()<<std::endl;
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
