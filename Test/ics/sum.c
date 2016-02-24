int main() {
	int i = 1, sum = 0;
	while(i <= 100) {
		sum = sum + i;
		i = i + 1;
	}

	put_int(sum);  // nemu_assert(sum == 5050);

	return 0;
}
