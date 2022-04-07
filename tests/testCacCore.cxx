#include "src/cacCore.h"
#include "gtest/gtest.h"

// Basic test template
TEST(TestCAC, SingleHart) {
    threadT coreNum = 1;
    //instantiate CAC by core num
    CacCore cac(coreNum);
    threadT tid0 = 0;
    // Every step should match
    size8BytesT PCValue0 [] = {0xcafe0000};
    size8BytesT RegXXValue0 [] = {0xbeefbeef};
    size8BytesT PCValue1 [] = {0xcafe0008};
    size8BytesT RegXXValue1 [] = {0x0};
    size8BytesT PCValue2 [] = {0xcafe0010};
    size8BytesT RegXXValue2 [] = {0xdeadbeef};
    size8BytesT PCValue3 [] = {0xcafe0018};
    size8BytesT RegXXValue3 [] = {0xabcdefffffffff};
    // step 1
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue0);
    cac.updateRefRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue0);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue0);
    cac.updateRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue0);
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    // step 2
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue1);
    cac.updateRefRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue1);
    cac.updateRefRegister(tid0, REGISTER_RT_FIX_ENCODING, 31, RegXXValue0);
    cac.updateRefRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue0);
    cac.updateRefRegister(tid0, CAC_STATE_RegX12_ID, RegXXValue3);
    cac.updateRefRegister(tid0, CAC_STATE_RegF31_ID, RegXXValue3);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue1);
    cac.updateRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue0);
    cac.updateRegister(tid0, REGISTER_RT_FIX_ENCODING, 31, RegXXValue0);
    cac.updateRegister(tid0, CAC_STATE_RegX12_ID, RegXXValue3);
    cac.updateRegister(tid0, CAC_STATE_RegF31_ID, RegXXValue3);
    cac.updateRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue1);
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    // step 3 
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue2);
    cac.updateRefRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue0);
    cac.updateRefRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue2);
    cac.updateRefRegister(tid0, CAC_STATE_Instruction_ID, RegXXValue2);
    cac.updateRefRegister(tid0, REGISTER_RT_FLT_ENCODING, 1, RegXXValue2);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue3);
    cac.updateRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue2);
    cac.updateRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue0);
    cac.updateRegister(tid0, CAC_STATE_Instruction_ID, RegXXValue0);
    cac.updateRegister(tid0, REGISTER_RT_FLT_ENCODING, 1, RegXXValue1);
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    //EXPECT_EQ(act, ans)<<"Type the debug Message here: "<<ans<<std::endl;
};

TEST(TestVectorCAC, SingleHart) {
    threadT coreNum = 1;
    //instantiate CAC by core num
    CacCore cac(coreNum);
    threadT tid0 = 0;
    // Every step should match
    size8BytesT PCValue0 [] = {0xcafe0000};
    size8BytesT RegXXValue0 [] = {0xbeefbeef,0xdeaddead};
    size8BytesT PCValue1 [] = {0xcafe0008};
    size8BytesT RegXXValue1 [] = {0x0,0x0};
    size8BytesT PCValue2 [] = {0xcafe0010};
    size8BytesT RegXXValue2 [] = {0xdeadbeef,0xbeefdead};
    size8BytesT PCValue3 [] = {0xcafe0018};
    size8BytesT RegXXValue3 [] = {0xabcdefffffffff,0x1234567800000000};
    // step 1
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue0);
    cac.updateRefRegister(tid0, CAC_STATE_RegV0_ID, RegXXValue0);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue0);
    cac.updateRegister(tid0, CAC_STATE_RegV0_ID, RegXXValue0);
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    //step 2
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue1);
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 31, RegXXValue3);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue1);
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 31, RegXXValue3);
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    //step 3
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue3);
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 1, RegXXValue1);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue3);
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 4
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue3);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue3);
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 1, RegXXValue1);
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 5
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue2);
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 20, RegXXValue2);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue2);
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 20, RegXXValue0);
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
}
