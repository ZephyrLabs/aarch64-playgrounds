/**
 * @file fp16_test.c
 * @author Sravan Senthilnathan
 * @brief example sketch to test the ARMv8.2-a + fp16 instruction set
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <arm_neon.h>

int main(void){

    float16x8_t a = {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f};
    float16x8_t b = {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f};

    float16x8_t c = vaddq_f16(a, b);

    printf("%f %f %f %f %f %f %f %f\n", c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);

    return(0);
}