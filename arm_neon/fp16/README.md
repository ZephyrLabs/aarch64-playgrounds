# Experiments with the ARMv8.2-a + fp16 instruction set
idk having some fun ig.

## Intrinsics Guide:
Refer to the official ARM Neon Intrinsics guide for [FP16 subset](https://developer.arm.com/architectures/instruction-sets/intrinsics/#f:@navigationhierarchieselementbitsize=[16]&f:@navigationhierarchiesarchitectures=[A64]&f:@navigationhierarchiesreturnbasetype=[float]&f:@navigationhierarchiessimdisa=[Neon])

## Compilation:
To run these examples, you must have a ARMv8.2 based system that also has the additional fp16 neon instructions enabled.

based on your compiler you will also need to enable the  "```-march=armv8.2-a+fp16```" flag.

## Further Notes:
Check this repository for more experimentation with the usage of this instruction set:
[ZephyrLabs/arm-neon-nn](https://github.com/ZephyrLabs/arm-neon-nn)