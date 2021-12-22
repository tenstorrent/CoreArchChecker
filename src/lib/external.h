#ifndef EXTERNAL_H
#define EXTERNAL_H

#include <map>
#include "cboardTypeDef.h"
#include "stateIdDef.h"
#include "cboardConst.h"

const stateIdT supportStates [] = {
    CBOARD_STATE_PC_ID,
    CBOARD_STATE_RegX0_ID,
    CBOARD_STATE_RegX1_ID,
    CBOARD_STATE_RegX2_ID,
    CBOARD_STATE_RegX3_ID,
    CBOARD_STATE_RegX4_ID,
    CBOARD_STATE_RegX5_ID,
    CBOARD_STATE_RegX6_ID,
    CBOARD_STATE_RegX7_ID,
    CBOARD_STATE_RegX8_ID,
    CBOARD_STATE_RegX9_ID,
 }; 

const std::map<stateIdT, std::string> supportStatesSymbol = {
    {CBOARD_STATE_PC_ID, "PC"},
    {CBOARD_STATE_RegX0_ID, "X0"},
    {CBOARD_STATE_RegX1_ID, "X1"},
    {CBOARD_STATE_RegX2_ID, "X2"},
    {CBOARD_STATE_RegX3_ID, "X3"},
    {CBOARD_STATE_RegX4_ID, "X4"},
    {CBOARD_STATE_RegX5_ID, "X5"},
    {CBOARD_STATE_RegX6_ID, "X6"},
    {CBOARD_STATE_RegX7_ID, "X7"},
    {CBOARD_STATE_RegX8_ID, "X8"},
    {CBOARD_STATE_RegX9_ID, "X9"},
};


const std::map<stateIdT, sizenBitT> supportStatesSize = {
    {CBOARD_STATE_PC_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX0_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX1_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX2_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX3_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX4_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX5_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX6_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX7_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX8_ID, UNIT_BIT_NUM},
    {CBOARD_STATE_RegX9_ID, UNIT_BIT_NUM},
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