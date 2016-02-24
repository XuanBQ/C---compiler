int add(int a, int b) {
	int c = a + b;
	write(c);
	return c;
}

int main() {
	int i, j;
	int test_data[8];
	int NR_DATA = 8;
	test_data[0] = 0;
	test_data[1] = 1;
	test_data[2] = 2;
	test_data[3] = 3;
	test_data[4] = 4;
	test_data[5] = 5;
	test_data[6] = 6;
	test_data[7] = 7;

	i = 0;
	while(i < NR_DATA) {
		j = 0;
		while(j < NR_DATA) {
			add(test_data[i], test_data[j]);
			j = j + 1;
		}
		i = i + 1;
	}	

	return 0;
}
