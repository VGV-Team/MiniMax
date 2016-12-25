__kernel void vector_add(__global const int *A,
	__global const int *B,
	__global int *C,
	int size)
{
	// globalni indeks elementa							
	int i = get_global_id(0);
	// izracun											
	while (i < size)
	{
		C[i] = A[i] + B[i];
		i += get_global_size(0);
	}
}