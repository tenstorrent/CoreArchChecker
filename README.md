# CBoard
## Architectural Checker 

[![Build Status](https://travis-ci.org/joemccann/dillinger.svg?branch=master)](https://yyz-gitlab.local.tenstorrent.com/chuang/cboard)

CBoard is a tool to check RTL DUT against an architectural simulator (Whisper) in lock step. If there is a mismatch in the state after the step, an error will be return bridge and the test will end. CBoard is also designed to provide memory sycchronization features to enable mutli-threaded run with memory sharing.

## Build CBoard
Build the CBoard based on Bazel

```sh
cd cboard
bazel build --cxxopt='-std=c++17' //src/lib:cboardcore
# bazel-out/darwin-fastbuild/bin/src/lib/libcboardcore.so
```

## Supported API
```sh
// Constructor
CBoard(threadT tNum);
// Dut API to update Register
void updateRegister(threadT threadId, stateIdT id, unitDataT * data);
// Simulator API to update Register
void updateRefRegister(threadT threadId, stateIdT id, unitDataT * data);
// make a lock step
void step(threadT threadId);
// get if mismatch
bool getStatus(threadT threadId);
```

## Example in Unit test
```sh
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
```

## Result of Unit test
```sh
Step: 0
                  PC             DUT:[Data:00000000cafe0000]
                                 SIM:[Data:00000000cafe0000]
                  X0             DUT:[Data:00000000beefbeef]
                                 SIM:[Data:00000000beefbeef]
Step: 1
                  PC             DUT:[Data:00000000cafe0008]
                                 SIM:[Data:00000000cafe0008]
                  X0             DUT:[Data:0000000000000000]
                                 SIM:[Data:0000000000000000]
                  X1             DUT:[Data:00000000beefbeef]
                                 SIM:[Data:00000000beefbeef]
Register Mismatch
Step: 2
                  PC             DUT:[Data:00000000cafe0018]
                                 SIM:[Data:00000000cafe0010]
                  X0             DUT:[Data:00000000beefbeef]
                                 SIM:[Data:00000000beefbeef]
                  X1             DUT:[Data:00000000deadbeef]
                                 SIM:[Data:00000000deadbeef]

```

## Type definition
```sh
typedef unsigned char size1ByteT;
typedef unsigned short int size2BytesT;
typedef unsigned int size4BytesT;
typedef unsigned long long int size8BytesT;

typedef size2BytesT threadT;
typedef size2BytesT stateIdT;
typedef size2BytesT sizenBitT;
typedef size8BytesT fuzzMaskT;
typedef size8BytesT unitDataT;
```

## How to define architectural states to check
In external.h, we define state ID, state size, state reset value and state symbol.
## Run unit tests
Unit tests are based on google test

```sh
cd cboard
bazel test --cxxopt='-std=c++17' //tests:testCBoard
bazel test --cxxopt='-std=c++17' //tests:basicClassTest
```

