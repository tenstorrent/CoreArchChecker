#include "src/lib/cboard.h"
#include "gtest/gtest.h"

// Basic test template
TEST(TestCBoard, SingleHart) {
    threadT coreNum = 1;
    //instantiate CBoard by core num
    CBoard cbd(coreNum);
    threadT tid0 = 0;
    // Every step should match
    size8BytesT PCValue0 [] = {0xcafe0000};
    size8BytesT RegXXValue0 [] = {0xbeefbeef};
    size8BytesT PCValue1 [] = {0xcafe0008};
    size8BytesT RegXXValue1 [] = {0x0};
    size8BytesT PCValue2 [] = {0xcafe0010};
    size8BytesT RegXXValue2 [] = {0xdeadbeef};
    size8BytesT PCValue3 [] = {0xcafe0018};
    size8BytesT RegXXValue3 [] = {0xffffffff};
    // step 1
    //From Simulator:
    cbd.updateRefRegister(tid0, CBOARD_STATE_PC_ID, PCValue0);
    cbd.updateRefRegister(tid0, CBOARD_STATE_RegX0_ID, RegXXValue0);
    //From DUT
    cbd.updateRegister(tid0, CBOARD_STATE_PC_ID, PCValue0);
    cbd.updateRegister(tid0, CBOARD_STATE_RegX0_ID, RegXXValue0);
    //Single Step
    cbd.step(tid0);
    EXPECT_TRUE(cbd.getStatus(tid0));
    // step 2
    //From Simulator:
    cbd.updateRefRegister(tid0, CBOARD_STATE_PC_ID, PCValue1);
    cbd.updateRefRegister(tid0, CBOARD_STATE_RegX0_ID, RegXXValue1);
    cbd.updateRefRegister(tid0, CBOARD_STATE_RegX1_ID, RegXXValue0);
    //From DUT
    cbd.updateRegister(tid0, CBOARD_STATE_PC_ID, PCValue1);
    cbd.updateRegister(tid0, CBOARD_STATE_RegX1_ID, RegXXValue0);
    cbd.updateRegister(tid0, CBOARD_STATE_RegX0_ID, RegXXValue1);
    //Single Step
    cbd.step(tid0);
    EXPECT_TRUE(cbd.getStatus(tid0));
    // step 3 
    //From Simulator:
    cbd.updateRefRegister(tid0, CBOARD_STATE_PC_ID, PCValue2);
    cbd.updateRefRegister(tid0, CBOARD_STATE_RegX0_ID, RegXXValue0);
    cbd.updateRefRegister(tid0, CBOARD_STATE_RegX1_ID, RegXXValue2);
    //From DUT
    cbd.updateRegister(tid0, CBOARD_STATE_PC_ID, PCValue3);
    cbd.updateRegister(tid0, CBOARD_STATE_RegX1_ID, RegXXValue2);
    cbd.updateRegister(tid0, CBOARD_STATE_RegX0_ID, RegXXValue0);
    //Single Step
    cbd.step(tid0);
    EXPECT_FALSE(cbd.getStatus(tid0));
    //EXPECT_EQ(act, ans)<<"Type the debug Message here: "<<ans<<std::endl;
};