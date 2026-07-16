# CoreArchChecker
## The Core Architectural Checker 
....

CoreArchChecker is a framework to check a CPU DUT's (Device Under Test) registers and memory against an architectural simulator (Whisper) in lock step. After each instruction retires or event gets triggered, the DUT and simulator's state is collected. CoreArchChecker maintains a snapshot of the current register states for both DUT and simulator separately, updating the register snapshot values after each step. If there is a mismatch in the state after the step, an error will return to bridge and the test will end. CoreArchChecker is also designed to provide memory synchronization features to enable multi-threaded run with memory sharing.

## Requirements
- **Build tool:** [Bazel](https://bazel.build) (tested with Bazel 5.x)
- **Compiler:** A C++ toolchain supporting **C++17**
- **Platforms:** Linux and macOS
- **Test framework:** [GoogleTest](https://github.com/google/googletest) (fetched automatically by Bazel)

## Getting Started: Build CoreArchChecker
Build the CoreArchChecker based on Bazel

```sh
cd CoreArchChecker
bazel build //src:caccore
# bazel-out/darwin-fastbuild/bin/src/libcaccore.so
```

## Supported API
```sh
// Constructor
CacCore(threadT tNum);
// Dut API to update Register
void updateRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>&& data);
// Simulator API to update Register
void updateRefRegister(threadT threadId, stateIdT id, const std::vector<unitDataT>&& data);
// make a lock step
void step(threadT threadId);
// get if mismatch
bool getStatus(threadT threadId);
```

## Example in Unit test
```sh
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
    cac.updateRefRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue0);
    cac.updateRefRegister(tid0, CAC_STATE_RegX11_ID, RegXXValue3);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue1);
    cac.updateRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue0);
    cac.updateRegister(tid0, CAC_STATE_RegX11_ID, RegXXValue3);
    cac.updateRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue1);
    //Single Step
    cac.step(tid0);
    EXPECT_TRUE(cac.getStatus(tid0));
    // step 3 
    //From Simulator:
    cac.updateRefRegister(tid0, CAC_STATE_PC_ID, PCValue2);
    cac.updateRefRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue0);
    cac.updateRefRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue2);
    //From DUT
    cac.updateRegister(tid0, CAC_STATE_PC_ID, PCValue3);
    cac.updateRegister(tid0, CAC_STATE_RegX1_ID, RegXXValue2);
    cac.updateRegister(tid0, CAC_STATE_RegX0_ID, RegXXValue0);
    //Single Step
    cac.step(tid0);
    EXPECT_FALSE(cac.getStatus(tid0));
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
cd CoreArchChecker
bazel test //tests:testCacCore
bazel test //tests:basicClassTest
```

## Contributing

Contributions are welcome! Bug reports and feature requests are handled via
[GitHub Issues](https://github.com/tenstorrent/CoreArchChecker/issues), and
changes are submitted via pull requests (reviewed weekly). See
[CONTRIBUTING.md](CONTRIBUTING.md) for build/test instructions and contribution
standards, and note that this project follows the
[Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). To report a security
vulnerability, follow the process in [SECURITY.md](SECURITY.md).

## License

- [LICENSE](LICENSE) (Apache-2.0) — Overall license for this project, except where specified.
- [LICENSE-DOCS](LICENSE-DOCS) (CC-BY-4.0) — License for all documentation and images only.
- [LICENSE_understanding.txt](LICENSE_understanding.txt) — Tenstorrent's clarification of how the Apache-2.0 license applies to this repository.

This repository is [REUSE](https://reuse.software) compliant; per-file license
and copyright information is provided via inline SPDX headers and
[REUSE.toml](REUSE.toml). Third-party components retain their own licenses as
recorded in [NOTICE](NOTICE).
