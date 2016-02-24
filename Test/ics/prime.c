/*
int ans[] = {101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199};
*/

int is_prime(int n) {
	int i;
	if(n < 2) return 0;

	i = 2;
	while(i < n) {
		if(n % i == 0) {
			return 0;
		}

		i = i + 1;
	}

	return 1;
}

int main() {
	int m, n = 0;
	
	m = 101;
	while(m <= 200) {
		if(is_prime(m)) {
			put_int(m);
			n = n + 1;
		}
		m = m + 2;
	}

	// nemu_assert(n == 21);
	put_int(n);

	return 0;
}
