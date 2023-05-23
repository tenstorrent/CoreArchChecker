#include "src/cacCore.h"
#include "cvm/plusargs.hpp"
#include "gtest/gtest.h"
#include <vector>
#include <unordered_map>

DECLARE_bool(cosim_tracer);

// Helper testbench for setting/getting register values.
// This is useful because the Register/RegisterSnapshots use
// move constructors so we don't need to manually create copies
// when passing register values into functions.
class CacTest : public ::testing::Test {
 protected:
    std::vector<size8BytesT> GetRegister(int id, int step) {
        return registers_[id][step];
    }

    void SetRegister(int id, int step, const std::vector<size8BytesT>& val) {
        if (registers_.find(id) == registers_.end()) {
            registers_.insert_or_assign(id, std::unordered_map<int, std::vector<size8BytesT>>());
        }
        registers_[id].insert_or_assign(step, val);
    }

    // Reg ID -> Step # -> Register val.
    std::unordered_map<int, std::unordered_map<int, std::vector<size8BytesT>>> registers_;
};


// Basic test template
TEST_F(CacTest, HelloWorld) {
    CacCore cbd(4);
    std::string act = cbd.getHello();
    std::string ans = "CacCore has been constructed!";
    EXPECT_EQ(act, ans)<<"Type the debug Message here: "<<ans<<std::endl;
};

// Test for class Register
TEST_F(CacTest, BasicRegisterTest){
    std::vector<size8BytesT> foo = {16, 32};
    std::vector<size8BytesT> expectedVector(foo);
    int regSize = 128;
    std::string regName = "PC";
    std::string regFormatString = "SIM:[Data:0000000000000010_0000000000000020](128)";
    Register r(0, 0, regSize, std::move(foo));
    EXPECT_EQ(regSize, r.getSize())<<"ERROR: (Register Test) Size is mismatch!";
    EXPECT_EQ(expectedVector, r.getValue())<<"ERROR: (Register Test) Value is mismatch!";
    EXPECT_EQ(regName, r.getName())<<"ERROR: (Register Test) Name is mismatch!";
    EXPECT_EQ(regFormatString, r.toString("SIM"))<<"ERROR: (Register Test) Formatted string is mismatch";

    std::vector<size8BytesT> foo2 = {0xdead, 0xbeef};
    std::vector<size8BytesT> expectedVector2(foo2);
    r.setValue(std::move(foo2));
    EXPECT_EQ(expectedVector2, r.getValue())<<"ERROR: (Register Test) setValue function is wrong!";

    r.updateSize(regSize * 2);
    EXPECT_EQ(regSize * 2, r.getSize())<<"ERROR: (Register Test) updateSize function is wrong!";
};

// Test for class RegisterSnapshot
TEST_F(CacTest, BasicSnapshotTest){
    threadT threadId = 0;
    RegisterSnapshot rs(threadId);
    size8BytesT pcVal = 0xdeadbeef;
    size8BytesT regXXVal = 0xcafe0001;
    rs.setValue(CAC_STATE_PC_ID, {pcVal});
    rs.setValue(CAC_STATE_RegX0_ID, {regXXVal});
    rs.setValue(CAC_STATE_RegX1_ID, {regXXVal});
    rs.setValue(CAC_STATE_RegX2_ID, {regXXVal});
    rs.setValue(CAC_STATE_RegX3_ID, {regXXVal});
    rs.setValue(CAC_STATE_RegX4_ID, {regXXVal});
    rs.setValue(CAC_STATE_RegX5_ID, {regXXVal});

    EXPECT_TRUE(rs.checkValue(CAC_STATE_PC_ID, {pcVal}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX0_ID, {regXXVal}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX1_ID, {regXXVal}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX2_ID, {regXXVal}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX3_ID, {regXXVal}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX4_ID, {regXXVal}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX5_ID, {regXXVal}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";

    EXPECT_FALSE(rs.checkValue(CAC_STATE_PC_ID, {pcVal + 0x1}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX0_ID, {regXXVal + 0x1}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX1_ID, {regXXVal + 0x1}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX2_ID, {regXXVal + 0x1}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX3_ID, {regXXVal + 0x1}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX4_ID, {regXXVal + 0x1}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX5_ID, {regXXVal + 0x1}))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
};

// Test for Core
TEST_F(CacTest, BasicCoreTest){
    threadT coreNum = 2;
    //instantiate CBoard by core num
    CacCore cbd(coreNum);
    threadT tid0 = 0;
    threadT tid1 = 1;
    SetRegister(CAC_STATE_PC_ID, 1, {0xcafe0000});
    SetRegister(CAC_STATE_PC_ID, 2, {0xcafe0008});
    SetRegister(CAC_STATE_RegX0_ID, 1, {0xbeefbeef});
    SetRegister(CAC_STATE_RegX0_ID, 2, {0xdeadbeef});
    // Single hart
    // Hart 0 Step 1
    // From Simulator:
    cbd.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    // Direct using state ID
    cbd.updateRefRegister(tid0, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    // Using encoding and offset
    cbd.updateRefRegister(tid0, REGISTER_RT_FIX_ENCODING, 1, GetRegister(CAC_STATE_RegX0_ID, 1));
    // From Dut:
    cbd.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cbd.updateRegister(tid0, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    cbd.updateRegister(tid0,  REGISTER_RT_FIX_ENCODING, 1, GetRegister(CAC_STATE_RegX0_ID, 1));
    cbd.step(tid0);
    EXPECT_EQ(cbd.getStep(tid0), 1)<<"ERROR: (CAC Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.getStatus(tid0));
    // Hart 0 Step 2
    // From Simulator:
    cbd.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cbd.updateRefRegister(tid0, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 2));
    cbd.updateRefRegister(tid0, REGISTER_RT_FLT_ENCODING, 6, GetRegister(CAC_STATE_RegX0_ID, 2));
    // From Dut:
    cbd.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cbd.updateRegister(tid0, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 2));
    cbd.updateRegister(tid0, REGISTER_RT_FLT_ENCODING, 6, GetRegister(CAC_STATE_RegX0_ID, 2));
    cbd.step(tid0);
    EXPECT_EQ(cbd.getStep(tid0), 2)<<"ERROR: (CBoard Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.getStatus(tid0));
    // Multi hart
    // From Simulator:
    // Hart0 Step 3
    cbd.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cbd.updateRefRegister(tid0, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    // Hart1 Step 1
    cbd.updateRefRegister(tid1, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cbd.updateRefRegister(tid1, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    // From Dut:
    // Hart0 Step 3
    cbd.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cbd.updateRegister(tid0, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    // Hart1 Step 1
    cbd.updateRegister(tid1, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cbd.updateRegister(tid1, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    cbd.step(tid0);
    EXPECT_EQ(cbd.getStep(tid0), 3)<<"ERROR: (Multi-Hart Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.getStatus(tid0));
    cbd.step(tid1);
    EXPECT_EQ(cbd.getStep(tid1), 1)<<"ERROR: (Multi-Hart Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.getStatus(tid1));
    // Intentional mismatch
    // Hart 0 Step 4
    // From Simulator:
    cbd.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cbd.updateRefRegister(tid0, CAC_STATE_RegX0_ID, GetRegister(CAC_STATE_RegX0_ID, 2));
    // From Dut:
    // Hart0 Step 4
    cbd.updateRegister(tid0, CAC_STATE_PC_ID, {0xabc});
    cbd.updateRegister(tid0, CAC_STATE_RegX0_ID, {0xdef});
    cbd.step(tid0);
    EXPECT_EQ(cbd.getStep(tid0), 4)<<"ERROR: (Intentional Reg Mismatch Test) Wrong Step Counts!";
    EXPECT_FALSE(cbd.getStatus(tid0));
};

TEST_F(CacTest, VectorRegistersTest) {
    threadT coreNum = 1;
    //instantiate CAC by core num
    CacCore cac(coreNum);
    threadT tid0 = 0;
    // Every step should match
    SetRegister(CAC_STATE_PC_ID, 1, {0xcafe0000});
    SetRegister(CAC_STATE_PC_ID, 2, {0xcafe0008});
    SetRegister(CAC_STATE_RegX0_ID, 1, {0xbeefbeef,0xdeaddead});
    SetRegister(CAC_STATE_RegX0_ID, 2, {0xbeefbeef,0xdeaddead});
    SetRegister(CAC_STATE_RegX0_ID, 3, {0xdeadbeef,0xbeefdead});
    SetRegister(CAC_STATE_RegX0_ID, 4, {0xabcdefffffffff,0x1234567800000000});
    SetRegister(CAC_STATE_RegX0_ID, 5, {0xcafe0020cafe0028, 0xcafe0030cafe0038, 0xcafe0040cafe0048, 0xcafe0050cafe0058});
    SetRegister(CAC_STATE_RegX0_ID, 6, {0x0, 0x0, 0xcafe0020cafe0028, 0xcafe0030cafe0038});
    SetRegister(CAC_STATE_RegX0_ID, 7, {0xcafe0020cafe0028, 0xcafe0030cafe0038, 0xcafe0040cafe0048, 0xcafe0050cafe0058, 0xcafe0020cafe0068, 0xcafe0030cafe0078, 0xcafe0040cafe0088, 0xcafe0050cafe0098});
    SetRegister(CAC_STATE_RegX0_ID, 8, {0xcafe0020cafe0028, 0xcafe0030cafe0038, 0xcafe0040cafe0048, 0xcafe0050cafe0058, 0x0, 0x0, 0xcafe0040cafe0088, 0xcafe0050cafe0098});
    // step 1
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRefRegister(tid0, CAC_STATE_RegV0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRegister(tid0, CAC_STATE_RegV0_ID, GetRegister(CAC_STATE_RegX0_ID, 1));
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    //step 2
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 31, GetRegister(CAC_STATE_RegX0_ID, 2));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 31, GetRegister(CAC_STATE_RegX0_ID, 2));
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    //step 3
    //From Simulator:
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 1, GetRegister(CAC_STATE_RegX0_ID, 3));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 2, GetRegister(CAC_STATE_RegX0_ID, 3));
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 4
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 1, GetRegister(CAC_STATE_RegX0_ID, 4));
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 5
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 20, GetRegister(CAC_STATE_RegX0_ID, 5));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 20, GetRegister(CAC_STATE_RegX0_ID, 1));
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 6
    //From Simulator:
    cac.configureVlen(256);
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 21, GetRegister(CAC_STATE_RegX0_ID, 5));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 21, GetRegister(CAC_STATE_RegX0_ID, 5));
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 7
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 22, GetRegister(CAC_STATE_RegX0_ID, 6));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 22, GetRegister(CAC_STATE_RegX0_ID, 5));
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 8
    //From Simulator:
    cac.configureVlen(512);
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 23, GetRegister(CAC_STATE_RegX0_ID, 7));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 2));
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 23, GetRegister(CAC_STATE_RegX0_ID, 7));
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
    //step 9
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRegister(tid0, REGISTER_RT_VEC_ENCODING, 24, GetRegister(CAC_STATE_RegX0_ID, 7));
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, GetRegister(CAC_STATE_PC_ID, 1));
    cac.updateRefRegister(tid0, REGISTER_RT_VEC_ENCODING, 24, GetRegister(CAC_STATE_RegX0_ID, 8));
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    cac.resetStatus(tid0);
}

TEST_F(CacTest, StringFormatTest) {
    threadT coreNum = 1;
    CacCore cac(coreNum);
    threadT tid0 = 0;
    size8BytesT pcVal = 0xcafe0000;
    SetRegister(CAC_STATE_RegV0_ID, 1, {0xbeefbeef,0xdeaddead});
    SetRegister(CAC_STATE_RegV1_ID, 2, {0xcafecafe,0xcafeabcd});
    // Hart 0 Step 1
    EXPECT_EQ("", cac.getStatusStr(tid0));
    // From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, {pcVal});
    cac.updateRefRegister(tid0, CAC_STATE_RegV0_ID, GetRegister(CAC_STATE_RegV0_ID, 1));
    // From Dut:
    cac.updateRegister(tid0, CAC_STATE_PC_ID, {pcVal});
    cac.updateRegister(tid0, CAC_STATE_RegV0_ID, GetRegister(CAC_STATE_RegV0_ID, 1));
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    std::string first_expected_format_str = FLAGS_cosim_tracer ? "Step: 1\n \
                 PC                 DUT:[Data:00000000cafe0000](64)\n \
                                    SIM:[Data:00000000cafe0000](64)\n \
                 V0                      DUT:[Data:00000000beefbeef_00000000deaddead](128)\n \
                                         SIM:[Data:00000000beefbeef_00000000deaddead](128)\n" : "";
    EXPECT_EQ(first_expected_format_str, cac.getStatusStr(tid0));
    cac.resetStatus(tid0);
    // Hart 0 Step 2
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, {pcVal + 0x1});
    cac.updateRefRegister(tid0, CAC_STATE_RegV1_ID, GetRegister(CAC_STATE_RegV1_ID, 2));
    // From Dut:
    cac.updateRegister(tid0, CAC_STATE_PC_ID, {pcVal + 0x2});
    cac.updateRegister(tid0, CAC_STATE_RegV1_ID, GetRegister(CAC_STATE_RegV1_ID, 2));
    std::string second_expected_format_str = "\nRegister Mismatch\nStep: 2\n \
                 PC                 DUT:[Data:00000000cafe0002](64)\n \
                                    SIM:[Data:00000000cafe0001](64)\n \
                 V1                      DUT:[Data:00000000cafecafe_00000000cafeabcd](128)\n \
                                         SIM:[Data:00000000cafecafe_00000000cafeabcd](128)\n";
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
    EXPECT_EQ(second_expected_format_str, cac.getStatusStr(tid0));
}
