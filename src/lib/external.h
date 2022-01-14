#ifndef EXTERNAL_H
#define EXTERNAL_H

#include "cacTypeDef.h"
#include "stateIdDef.h"
#include "cacConst.h"

const stateIdT supportStates [] = {
    CAC_STATE_PC_ID,
    CAC_STATE_RegX0_ID,
    CAC_STATE_RegX1_ID,
    CAC_STATE_RegX2_ID,
    CAC_STATE_RegX3_ID,
    CAC_STATE_RegX4_ID,
    CAC_STATE_RegX5_ID
 }; 

const std::string supportStatesSymbol [] = {
    "PC",
    "X0",
    "X1",
    "X2",
    "X3",
    "X4",
    "X5",
};

const sizenBitT supportStatesSize [] = {
    UNIT_BIT_NUM, //CAC_STATE_PC_ID
    UNIT_BIT_NUM, //CAC_STATE_RegX0_ID
    UNIT_BIT_NUM, //CAC_STATE_RegX0_ID
    UNIT_BIT_NUM, //CAC_STATE_RegX0_ID
    UNIT_BIT_NUM, //CAC_STATE_RegX0_ID
    UNIT_BIT_NUM, //CAC_STATE_RegX0_ID
    UNIT_BIT_NUM, //CAC_STATE_RegX0_ID
};

//const std::map<stateIdT, dataVectorT*> supportStatesResetValue = {
//    {CBOARD_STATE_PC_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX0_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX1_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX2_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX3_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX4_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX5_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX6_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX7_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX8_ID, RST_8_BYTE_VALUE},
//    {CBOARD_STATE_RegX9_ID, RST_8_BYTE_VALUE},
//}

#endif