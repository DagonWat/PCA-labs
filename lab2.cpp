#include <windows.h>
#include <stdio.h>
#include <time.h>

void show_result(int sums_arr[], int length, float time) {
	for (int i = 0; i < length; i++) {
		printf("%d ", sums_arr[i]);
	}
	printf("\nTime:%.40f\n", time / CLOCKS_PER_SEC);
}

int main() {
	int count = 1000000, length = 16;
	int temp_sum;
	clock_t start, stop;
	int matrix[16][16];  // initialization of matrix
	int sums_arr[16] = { 0 }; // initialization of answer array
	// fill matrix with random values
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < length; j++) {
			matrix[i][j] = -999 + rand() % 1998;
		}
	}
	// show matrix
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < length; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
	// sum up lines values using C
	start = clock();
	for (int t = 0; t < count; t++) {
		for (int i = 0; i < length; i++) {
			temp_sum = 0;
			for (int j = 0; j < length; j++) {
				temp_sum += matrix[i][j];
			}
			sums_arr[i] = temp_sum;
		}
	}
	stop = clock();
	// calculate processing time
	float res_time = stop - start;
	printf("C:\n");
	// showing result
	show_result(sums_arr, length, res_time);

	clock_t startAsm, stopAsm;
	// sum up lines values using Asm
	startAsm = clock();
	for (int t = 0; t < count; t++) {
		_asm {
			pusha
			mov ecx, 16	// 16 iterations for outer loop
			xor esi, esi
			xor edi, edi
			form_array :
				push ecx
				mov cx, 16	// 16 iterations for inner loop
				xor eax, eax
			sum_str :
				add eax, matrix[esi]
				add esi, 4	// 4 bytes per step for int
			loop sum_str
				mov sums_arr[edi], eax
				add edi, 4
				pop ecx
			loop form_array
			popa
		}
	}
	stopAsm = clock();
	printf("Assembly:\n");
	res_time = stopAsm - startAsm;
	show_result(sums_arr, length, res_time);

	clock_t startMMX, stopMMX;
	// sum up lines values using Asm with MMX
	startMMX = clock();
	for (int t = 0; t < count; t++) {
		_asm {
			pusha
			mov ecx, 16
			xor esi, esi
			xor edi, edi
			form_array_mmx :
				push ecx
				mov ecx, 8
				pxor MM7, MM7
			sum_str_mmx :
				paddd MM7, matrix[esi]
				add esi, 8	// 8 bytes per step for 2 int values 4 bytes each
			loop sum_str_mmx
				movq MM0, MM7
				psrlq MM7, 32	// MM7 contains sum in 2 parts
				paddd MM7, MM0	// sum up 2 parts
				movd sums_arr[edi], MM7	// write sum value
				pop ecx
				add edi, 4 // 4 bytes per step for int
			loop form_array_mmx
			emms
			popa
		}
	}
	stopMMX = clock();
	printf("Assembly with MMX:\n");
	res_time = stopMMX - startMMX;
	show_result(sums_arr, length, res_time);
	system("pause");
	return 0;
}
