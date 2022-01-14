#ifndef EXTERNAL_H
#define EXTERNAL_H

#include <unordered_map>
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

const std::unordered_map<stateIdT, std::string> supportStatesSymbol = {
    {CAC_STATE_PC_ID, "PC"},
    {CAC_STATE_RegX0_ID, "X0"},
    {CAC_STATE_RegX1_ID, "X1"},
    {CAC_STATE_RegX2_ID, "X2"},
    {CAC_STATE_RegX3_ID, "X3"},
    {CAC_STATE_RegX4_ID, "X4"},
    {CAC_STATE_RegX5_ID, "X5"}
};

const std::unordered_map<stateIdT, sizenBitT> supportStatesSize = {
    {CAC_STATE_PC_ID, UNIT_BIT_NUM},
    {CAC_STATE_RegX0_ID, UNIT_BIT_NUM},
    {CAC_STATE_RegX1_ID, UNIT_BIT_NUM},
    {CAC_STATE_RegX2_ID, UNIT_BIT_NUM},
    {CAC_STATE_RegX3_ID, UNIT_BIT_NUM},
    {CAC_STATE_RegX4_ID, UNIT_BIT_NUM},
    {CAC_STATE_RegX5_ID, UNIT_BIT_NUM}
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