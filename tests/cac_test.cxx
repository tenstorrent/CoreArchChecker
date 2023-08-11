#include "src/cac_core.h"

#include <limits>
#include <unordered_map>
#include <vector>

#include "gtest/gtest.h"

#include "cvm/plusargs.hpp"

DECLARE_bool(bridge_log);

namespace cac {

// Helper testbench for setting/getting resource values.
// This is useful because the Resource/ResourceSnapshots use
// move constructors so we don't need to manually create copies
// when passing resource values into functions.
class CacTest : public ::testing::Test {
 protected:
    data_t GetResource(resource_id_t id, int step) {
        return resources_[id][step];
    }

    void SetResource(resource_id_t id, int step, const data_t& val) {
        if (resources_.find(id) == resources_.end()) {
            resources_.insert_or_assign(id, std::unordered_map<int, data_t>());
        }
        resources_[id].insert_or_assign(step, val);
    }

    // Reg ID -> Step # -> Resource val.
    std::unordered_map<resource_id_t, std::unordered_map<int, data_t>> resources_;
};

TEST_F(CacTest, BitsetTest) {
    unit_data_t num = 0xabcd;
    data_t bool_vec = CreateBitVec<decltype(num)>(num);
    auto bitset1 = ToBitset<UNIT_BIT_NUM>(bool_vec);
    EXPECT_EQ(num, bitset1);
    auto bitset2 = ToBitset<UNIT_BIT_NUM>(bool_vec, 0, bitset1.size());
    EXPECT_EQ(num, bitset2);
    auto bitset3 = ToBitset<UNIT_BIT_NUM>(bool_vec, 8, 4);
    EXPECT_EQ(0xb, bitset3);
}

TEST_F(CacTest, PcResourceTest){
    data_t foo = CreateBitVec<unit_data_t>(0xabcd);
    // Creating a copy due to std::move of foo.
    data_t expected_val = foo;
    std::string expected_name = "PC";
    std::string expected_str = "ISS:[Data:000000000000abcd](64)";
    resource_id_t expected_id = resource_id_t{
        .resource = resource_t::pc_reg,
        .offset = 0
    };
    std::optional<std::unique_ptr<VariableSizeResource>> ret = VariableSizeResource::Create(expected_id, std::move(foo));
    EXPECT_NE(ret, std::nullopt);
    std::unique_ptr<VariableSizeResource>& resource = ret.value();
    EXPECT_EQ(DefaultSize(expected_id.resource), resource->GetSize())<<"ERROR: (Resource Test) Size mismatch!";
    EXPECT_EQ(expected_val, resource->GetValue())<<"ERROR: (Resource Test) Value mismatch!";
    EXPECT_EQ(expected_name, resource->GetName())<<"ERROR: (Resource Test) Name mismatch!";
    EXPECT_EQ(expected_str, resource->ToString("ISS"))<<"ERROR: (Resource Test) Formatted string mismatch";

    data_t foo2 = CreateBitVec<unit_data_t>(0xbeef);
    data_t expected_val_2 = foo2;
    EXPECT_TRUE(resource->SetValue(std::move(foo2)));
    EXPECT_EQ(expected_val_2, resource->GetValue())<<"ERROR: (Resource Test) SetValue function is wrong!";

    data_t foo3 = CreateBitVec<uint32_t>(0xabcd);
    data_t expected_val_3 = foo3;
    EXPECT_FALSE(resource->SetValue(std::move(foo3)));
    EXPECT_NE(expected_val_3, resource->GetValue())<<"ERROR: (Resource Test) SetValue should not allow invalid sizes for resources";

    EXPECT_FALSE(resource->SetSize(128))<<"ERROR: (Resource Test) SetSize should fail for PC";

    data_t foo4 = CreateBitVec<unit_data_t>(0x1234);
    mask_t mask = CreateBitVec<unit_data_t>(0xF0F0F0F0F0F0F0F0);
    EXPECT_TRUE(resource->SetValue(std::move(foo4), mask));
    data_t expected_val_4 = CreateBitVec<unit_data_t>(0x0000000000001030);
    EXPECT_EQ(expected_val_4, resource->GetValue())<<"ERROR: (ResourceTest) Data not masked correctly";
}

TEST_F(CacTest, VectorResourceTest){
    data_t foo = CreateBitVec<unit_data_t>({0x10, 0x20});
    data_t expected_val = foo;
    int expected_size = 128;
    std::string expected_name = "V5";
    std::string expected_str = "ISS:[Data:0000000000000020_0000000000000010](128)";
    resource_id_t expected_id = resource_id_t{
        .resource = resource_t::vec_reg,
        .offset = 5
    };
    std::optional<std::unique_ptr<VariableSizeResource>> ret = VariableSizeResource::Create(expected_id, std::move(foo));
    EXPECT_NE(ret, std::nullopt);
    std::unique_ptr<VariableSizeResource>& resource = ret.value();
    EXPECT_EQ(expected_size, resource->GetSize())<<"ERROR: (Resource Test) Size mismatch!";
    EXPECT_EQ(expected_val, resource->GetValue())<<"ERROR: (Resource Test) Value mismatch!";
    EXPECT_EQ(expected_name, resource->GetName())<<"ERROR: (Resource Test) Name mismatch!";
    EXPECT_EQ(expected_str, resource->ToString("ISS"))<<"ERROR: (Resource Test) Formatted string mismatch";

    data_t foo2 = CreateBitVec<unit_data_t>({0xdead, 0xbeef});
    data_t expected_val_2(foo2);
    EXPECT_TRUE(resource->SetValue(std::move(foo2)));
    EXPECT_EQ(expected_val_2, resource->GetValue())<<"ERROR: (Resource Test) SetValue function is wrong!";

    data_t foo3 = CreateBitVec<unit_data_t>({0x1234, 0x5678});
    mask_t mask = CreateBitVec<std::bitset<128>>(std::bitset<128>(0xF0F0F0F0F0F0F0F0) | std::bitset<128>(0xF0F0F0F0F0F0F0F0) << 64);
    EXPECT_TRUE(resource->SetValue(std::move(foo3), mask));
    data_t expected_val_3 = CreateBitVec<std::bitset<128>>(std::bitset<128>(0x0000000000001030) | (std::bitset<128>(0x0000000000005070) << 64));
    EXPECT_EQ(expected_val_3, resource->GetValue())<<"ERROR: (ResourceTest) Data not masked correctly";

    EXPECT_TRUE(resource->SetSize(expected_size * 2));
    EXPECT_EQ(expected_size * 2, resource->GetSize())<<"ERROR: (Resource Test) SetSize function is wrong!";
    EXPECT_FALSE(resource->SetSize(5))<<"ERROR: (Resource Test) Cannot resize to an invalid size for this resource type";
}

TEST_F(CacTest, CsrResourceTest){
    data_t foo = CreateBitVec<unit_data_t>(0x1234);
    // Creating a copy due to std::move of foo.
    data_t expected_val = foo;
    std::string expected_name = "C_0x3ff";
    std::string expected_str = "ISS:[Data:0000000000001234](64)";
    resource_id_t expected_id = resource_id_t{
        .resource = resource_t::csr_reg,
        .offset = 1023
    };
    std::optional<std::unique_ptr<VariableSizeResource>> ret = VariableSizeResource::Create(expected_id, std::move(foo));
    EXPECT_NE(ret, std::nullopt);
    std::unique_ptr<VariableSizeResource>& resource = ret.value();
    EXPECT_EQ(DEFAULT_SIZE_BITS.at(expected_id.resource), resource->GetSize())<<"ERROR: (Resource Test) Size mismatch!";
    EXPECT_EQ(expected_val, resource->GetValue())<<"ERROR: (Resource Test) Value mismatch!";
    EXPECT_EQ(expected_name, resource->GetName())<<"ERROR: (Resource Test) Name mismatch!";
    EXPECT_EQ(expected_str, resource->ToString("ISS"))<<"ERROR: (Resource Test) Formatted string mismatch";

    data_t foo2 = CreateBitVec<unit_data_t>(0xbeef);
    data_t expected_val_2 = foo2;
    EXPECT_TRUE(resource->SetValue(std::move(foo2)));
    EXPECT_EQ(expected_val_2, resource->GetValue())<<"ERROR: (Resource Test) SetValue function is wrong!";

    data_t foo3 = CreateBitVec<unit_data_t>(0x1234);
    mask_t mask = CreateBitVec<unit_data_t>(0xF0F0F0F0F0F0F0F0);
    EXPECT_TRUE(resource->SetValue(std::move(foo3), mask));
    data_t expected_val_3 = CreateBitVec<unit_data_t>(0x0000000000001030);
    EXPECT_EQ(expected_val_3, resource->GetValue())<<"ERROR: (ResourceTest) Data not masked correctly";

    EXPECT_FALSE(resource->SetSize(128))<<"ERROR: (Resource Test) SetSize should fail for PC";
}

TEST_F(CacTest, ResourceSnapshotTest){
    ResourceSnapshot rs(true, "ISS");
    data_t pc_val = CreateBitVec<unit_data_t>(0xdeadbeef);
    data_t pc_val_cpy = pc_val;
    data_t reg_xx_val = CreateBitVec<unit_data_t>(0xcafe0001);
    data_t reg_xx_val_cpy = reg_xx_val;
    data_t csr_val = CreateBitVec<unit_data_t>(0xabcd1234);
    data_t csr_val_cpy = csr_val;
    data_t vec_reg_val = CreateBitVec<std::bitset<128>>(0x0201);
    data_t vec_reg_val_cpy = vec_reg_val;
    resource_id_t pc_id = resource_id_t{
        .resource = resource_t::pc_reg,
        .offset = 0
    };
    resource_id_t reg_xx_id = resource_id_t{
        .resource = resource_t::int_reg,
        .offset = 5
    };
    resource_id_t csr_id = resource_id_t{
        .resource = resource_t::csr_reg,
        .offset = 1023
    };
    resource_id_t vec_reg_id = resource_id_t{
        .resource = resource_t::vec_reg,
        .offset = 12
    };
    
    EXPECT_TRUE(rs.SetValue(pc_id, std::move(pc_val)));
    EXPECT_TRUE(rs.SetValue(reg_xx_id, std::move(reg_xx_val)));
    EXPECT_TRUE(rs.SetValue(csr_id, std::move(csr_val)));
    EXPECT_TRUE(rs.SetValue(vec_reg_id, std::move(vec_reg_val)));

    EXPECT_TRUE(rs.Exists(pc_id));
    EXPECT_TRUE(rs.Exists(reg_xx_id));
    EXPECT_TRUE(rs.Exists(csr_id));
    EXPECT_TRUE(rs.Exists(vec_reg_id));
    // Space pre-reserved.
    resource_id_t fp_id = resource_id_t{
        .resource = resource_t::fp_reg,
        .offset = 31
    };
    EXPECT_TRUE(rs.Exists(fp_id));
    // Space not pre-reserved.
    EXPECT_FALSE(ResourceSnapshot(false, ToString(src_t::dut)).Exists(fp_id));

    EXPECT_TRUE(rs.CheckValue(pc_id, pc_val_cpy))<< "ERROR: (ResourceSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.CheckValue(reg_xx_id, reg_xx_val_cpy))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.CheckValue(csr_id, csr_val_cpy))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.CheckValue(vec_reg_id, vec_reg_val_cpy))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    // Default reg val
    EXPECT_TRUE(rs.CheckValue(fp_id, data_t(DEFAULT_SIZE_BITS.at(fp_id.resource), 0)));

    EXPECT_EQ(rs.GetSize(pc_id), UNIT_BIT_NUM);
    EXPECT_EQ(rs.GetSize(vec_reg_id), VEC_128);

    EXPECT_FALSE(rs.SetValue(pc_id, CreateBitVec<unit_data_t>({0x1, 0x2})));
    EXPECT_TRUE(rs.CheckValue(pc_id, pc_val_cpy))<< "ERROR: (ResourceSnapshot Test) Unexpected checking value!";

    EXPECT_TRUE(rs.SetValue(pc_id, CreateBitVec<unit_data_t>({0x2})));
    EXPECT_TRUE(rs.CheckValue(pc_id, CreateBitVec<unit_data_t>({0x2})))<< "ERROR: (ResourceSnapshot Test) Unexpected checking value!";

    EXPECT_TRUE(rs.SetValue(vec_reg_id, CreateBitVec<unit_data_t>({0x1, 0x2, 0x3, 0x4})));
    EXPECT_EQ(rs.GetSize(vec_reg_id), VEC_256);
    EXPECT_TRUE(rs.SetVlen(VEC_128));
    EXPECT_EQ(rs.GetSize(vec_reg_id), VEC_128);
    EXPECT_TRUE(rs.CheckValue(vec_reg_id, CreateBitVec<unit_data_t>({0x1, 0x2})))<< "ERROR: (ResourceSnapshot Test) Unexpected checking value!";

    EXPECT_EQ(rs.GetChangeCount(), 4);
    rs.ResetChangedResources();
    EXPECT_EQ(rs.GetChangeCount(), 0);
}

// Test for Core
TEST_F(CacTest, BasicCoreTest){
    hart_t num_harts = 2;
    CacCore cbd(num_harts);
    hart_t tid0 = 0;
    hart_t tid1 = 1;
    resource_id_t pc_id = resource_id_t{
        .resource = resource_t::pc_reg,
        .offset = 0
    };
    resource_id_t reg_xx_id = resource_id_t{
        .resource = resource_t::int_reg,
        .offset = 5
    };
    SetResource(pc_id, 1, CreateBitVec<unit_data_t>({0xcafe0000}));
    SetResource(pc_id, 2, CreateBitVec<unit_data_t>({0xcafe0008}));
    SetResource(reg_xx_id, 1, CreateBitVec<unit_data_t>({0xbeefbeef}));
    SetResource(reg_xx_id, 2, CreateBitVec<unit_data_t>({0xdeadbeef}));
    // Single hart
    // Hart 0 Step 1
    // From ISS:
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, reg_xx_id, GetResource(reg_xx_id, 1), std::nullopt));
    // From Dut:
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, reg_xx_id, GetResource(reg_xx_id, 1), std::nullopt));
    cbd.Step(tid0);
    EXPECT_EQ(cbd.GetStep(tid0), 1)<<"ERROR: (CAC Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.GetStatus(tid0));
    // Hart 0 Step 2
    // From ISS:
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 2), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, reg_xx_id, GetResource(reg_xx_id, 2), std::nullopt));
    // From Dut:
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 2), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, reg_xx_id, GetResource(reg_xx_id, 2), std::nullopt));
    cbd.Step(tid0);
    EXPECT_EQ(cbd.GetStep(tid0), 2)<<"ERROR: (CAC Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.GetStatus(tid0));
    // Multi hart
    // From ISS:
    // Hart0 Step 3
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, reg_xx_id, GetResource(reg_xx_id, 1), std::nullopt));
    // Hart1 Step 1
    EXPECT_TRUE(cbd.UpdateResource(tid1, src_t::iss, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid1, src_t::iss, reg_xx_id, GetResource(reg_xx_id, 1), std::nullopt));
    // From Dut:
    // Hart0 Step 3
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, reg_xx_id, GetResource(reg_xx_id, 1), std::nullopt));
    // Hart1 Step 1
    EXPECT_TRUE(cbd.UpdateResource(tid1, src_t::dut, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid1, src_t::dut, reg_xx_id, GetResource(reg_xx_id, 1), std::nullopt));
    cbd.Step(tid0);
    EXPECT_EQ(cbd.GetStep(tid0), 3)<<"ERROR: (Multi-Hart Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.GetStatus(tid0));
    cbd.Step(tid1);
    EXPECT_EQ(cbd.GetStep(tid1), 1)<<"ERROR: (Multi-Hart Test) Wrong Step Counts!";
    EXPECT_TRUE(cbd.GetStatus(tid1));
    // Intentional mismatch
    // Hart 0 Step 4
    // From ISS:
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 2), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::iss, pc_id, GetResource(reg_xx_id, 2), std::nullopt));
    // From Dut:
    // Hart0 Step 4
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, pc_id, CreateBitVec<unit_data_t>({0xabc}), std::nullopt));
    EXPECT_TRUE(cbd.UpdateResource(tid0, src_t::dut, pc_id, CreateBitVec<unit_data_t>({0xdef}), std::nullopt));
    cbd.Step(tid0);
    EXPECT_EQ(cbd.GetStep(tid0), 4)<<"ERROR: (Intentional Reg Mismatch Test) Wrong Step Counts!";
    EXPECT_FALSE(cbd.GetStatus(tid0));
}

TEST_F(CacTest, VectorRegistersTest) {
    CacCore cac(1);
    hart_t tid0 = 0;
    resource_id_t pc_id = resource_id_t{
        .resource = resource_t::pc_reg,
        .offset = 0
    };
    resource_id_t vec_reg_id = resource_id_t{
        .resource = resource_t::vec_reg,
        .offset = 5
    };
    SetResource(pc_id, 1, CreateBitVec<unit_data_t>({0xcafe0000}));
    SetResource(pc_id, 2, CreateBitVec<unit_data_t>({0xcafe0008}));
    SetResource(pc_id, 3, CreateBitVec<unit_data_t>({0xcafe000C}));
    SetResource(vec_reg_id, 1, CreateBitVec<unit_data_t>({0xbeefbeef,0xdeaddead}));
    SetResource(vec_reg_id, 2, CreateBitVec<unit_data_t>({0xbeefbeef,0xdeaddead}));
    SetResource(vec_reg_id, 3, CreateBitVec<unit_data_t>({0xdeadbeef,0xbeefdead}));
    // step 1
    //From ISS:
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, vec_reg_id, GetResource(vec_reg_id, 1), std::nullopt));
    //From DUT
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, vec_reg_id, GetResource(vec_reg_id, 1), std::nullopt));
    //Single Step
    cac.Step(tid0);
    EXPECT_TRUE(cac.GetStatus(tid0));
    //step 2
    //From ISS:
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 2), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, vec_reg_id, GetResource(vec_reg_id, 2), std::nullopt));
    //From DUT
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 2), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, vec_reg_id, GetResource(vec_reg_id, 2), std::nullopt));
    //Single Step
    cac.Step(tid0);
    EXPECT_TRUE(cac.GetStatus(tid0));
    //step 3
    //From ISS:
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 3), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, vec_reg_id, GetResource(vec_reg_id, 3), std::nullopt));
    //From DUT
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 3), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, vec_reg_id, GetResource(vec_reg_id, 2), std::nullopt));
    //Single Step
    cac.Step(tid0);
    EXPECT_FALSE(cac.GetStatus(tid0));
    cac.ResetStatus(tid0);
    EXPECT_TRUE(cac.GetStatus(tid0));
}

TEST_F(CacTest, StringFormatTest) {
    CacCore cac(1);
    hart_t tid0 = 0;
    unit_data_t pc_val = 0xcafe0000;
    resource_id_t pc_id = resource_id_t{
        .resource = resource_t::pc_reg,
        .offset = 0
    };
    resource_id_t vec_reg_0_id = resource_id_t{
        .resource = resource_t::vec_reg,
        .offset = 0
    };
    resource_id_t vec_reg_1_id = resource_id_t{
        .resource = resource_t::vec_reg,
        .offset = 1
    };
    SetResource(pc_id, 1, CreateBitVec<unit_data_t>(pc_val));
    SetResource(pc_id, 2, CreateBitVec<unit_data_t>(pc_val + 0x1));
    SetResource(vec_reg_0_id, 1, CreateBitVec<unit_data_t>({0xbeefbeef,0xdeaddead}));
    SetResource(vec_reg_1_id, 2, CreateBitVec<unit_data_t>({0xcafecafe,0xcafeabcd}));
    // Hart 0 Step 1
    EXPECT_EQ("", cac.GetStatusStr(tid0));
    // From ISS:
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, vec_reg_0_id, GetResource(vec_reg_0_id, 1), std::nullopt));
    // From Dut:
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, vec_reg_0_id, GetResource(vec_reg_0_id, 1), std::nullopt));
    cac.Step(tid0);
    EXPECT_TRUE(cac.GetStatus(tid0));
    std::string first_expected_format_str = FLAGS_bridge_log ? "Step: 1\n \
                 PC                 DUT:[Data:00000000cafe0000](64)\n \
                                    ISS:[Data:00000000cafe0000](64)\n \
                 V0                 DUT:[Data:00000000deaddead_00000000beefbeef](128)\n \
                                    ISS:[Data:00000000deaddead_00000000beefbeef](128)\n" : "";
    EXPECT_EQ(first_expected_format_str, cac.GetStatusStr(tid0));
    cac.ResetStatus(tid0);
    // Hart 0 Step 2
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 2), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, vec_reg_1_id, GetResource(vec_reg_1_id, 2), std::nullopt));
    // From Dut:
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 1), std::nullopt));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, vec_reg_1_id, GetResource(vec_reg_1_id, 2), std::nullopt));
    std::string second_expected_format_str = "Step: 2\n \
                 PC                 DUT:[Data:00000000cafe0000](64)\n \
                                    ISS:[Data:00000000cafe0001](64)\n \
                 V1                 DUT:[Data:00000000cafeabcd_00000000cafecafe](128)\n \
                                    ISS:[Data:00000000cafeabcd_00000000cafecafe](128)\n";
    cac.Step(tid0);
    EXPECT_FALSE(cac.GetStatus(tid0));
    EXPECT_EQ(second_expected_format_str, cac.GetStatusStr(tid0));
}

TEST_F(CacTest, MaskTest) {
    CacCore cac(1);
    hart_t tid0 = 0;
    unit_data_t pc_val = 0xcafe0000;
    resource_id_t pc_id = resource_id_t{
        .resource = resource_t::pc_reg,
        .offset = 0
    };
    SetResource(pc_id, 1, CreateBitVec<unit_data_t>(pc_val));
    SetResource(pc_id, 2, CreateBitVec<unit_data_t>(0xabcd1234beef5678));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 1), std::nullopt));
    mask_t mask = CreateBitVec<unit_data_t>(std::numeric_limits<unit_data_t>::max());
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 1), mask));
    cac.Step(tid0);
    EXPECT_TRUE(cac.GetStatus(tid0));
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::iss, pc_id, GetResource(pc_id, 2), std::nullopt));
    mask = CreateBitVec<unit_data_t>(0xFF00FF00FF00FF00);
    EXPECT_TRUE(cac.UpdateResource(tid0, src_t::dut, pc_id, GetResource(pc_id, 2), mask));
    cac.Step(tid0);
    EXPECT_FALSE(cac.GetStatus(tid0));
    std::string expected_format_str = "Step: 2\n \
                 PC                 DUT:[Data:ab001200be005600](64)\n \
                                    ISS:[Data:abcd1234beef5678](64)\n";
    EXPECT_EQ(expected_format_str, cac.GetStatusStr(tid0));
}


}
