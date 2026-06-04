# CoreArchChecker
CoreArchChecker is a framework to check CPU DUT's registers and memory against an architectural simulator (Whisper) in lock step. After every instruction retire, the DUT and simulator's state is collected. CoreArchChecker maintains a snapshot and provides API for comparison.
