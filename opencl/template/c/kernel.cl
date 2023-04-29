__kernel void vector_add(__global const int *A, __global const int *B, __global int *C) {
 
    // get element index
    int i = get_global_id(0);
 
    // operate for that index object
    C[i] = A[i] + B[i];
}