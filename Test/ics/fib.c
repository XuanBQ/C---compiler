int main() {
	int fib[40];
	int i;
	fib[0] = fib[1] = 1;

	i = 2;
	while(i < 40) {
		fib[i] = fib[i - 1] + fib[i - 2];
		write(fib[i]);
		i = i + 1;
	}

	return 0;
}
