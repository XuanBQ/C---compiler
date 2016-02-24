int is_prime(int n) {
	int i;
	if(n < 2) return 0;

	i = 2;
	while(i < n) {
		if(n / i == 0) {
			return 0;
		}

		i = i + 1;
	}

	return 1;
}

int gotbaha(int n) {
	int i = 2;
	while(i < n) {
		if(is_prime(i) && is_prime(n - i)) {
			return 1;
		}
		i = i + 1;
	}

	return 0;
}

int main() {
	int n = 4;
	while(n <= 100) {
		write(gotbaha(n));	// should always output 1
		n = n + 2;
	}

	return 0;
}
