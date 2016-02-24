int min3(int x, int y, int z) {
	int m;
	if(x < y) { m = x; }
	else { m = y; }
	if(z < m) m = z;
	return m;
}

int test_data[8];

/*
int ans[] = {0, 0, 0, 0, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0, 0, 0, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0, 0, 0, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0, 0, 0, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0, 0, 0, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x1, 0x1, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x1, 0x1, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x1, 0x1, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0, 0, 0, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x1, 0x1, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x2, 0x2, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x2, 0x2, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0, 0, 0, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x1, 0x1, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x2, 0x2, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0, 0x1, 0x2, 0x7fffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000000, 0x80000001, 0x80000001, 0x80000001, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0xfffffffe, 0x80000000, 0x80000001, 0xfffffffe, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff};
*/

int main() {
	int i, j, k;
	int NR_DATA = 8;
	test_data[0] = 0;
	test_data[1] = 1;
	test_data[2] = 2;
	test_data[3] = 0x7fffffff;
	test_data[4] = 0x80000000;
	test_data[5] = 0x80000001;
	test_data[6] = 0xfffffffe;
	test_data[7] = 0xffffffff;

	i = 0;
	while(i < NR_DATA) {
		j = 0;
		while(j < NR_DATA) {
			k = 0;
			while(k < NR_DATA) {
				put_int(min3(test_data[i], test_data[j], test_data[k]));
				k = k + 1;
			}
			j = j + 1;
		}
		i = i + 1;
	}	

	return 0;
}
