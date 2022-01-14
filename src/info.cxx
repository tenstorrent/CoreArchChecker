#include "info.h"
#include <iostream>
#include <sstream>
#include <iomanip>

//Info
Info::Info(threadT tid, stateIdT stateId, const std::string &type, unitDataT * item):threadId(tid),stateId(stateId),infoType(type),item(item){
    itemName = supportStatesSymbol[stateId];
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