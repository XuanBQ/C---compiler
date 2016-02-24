/*
int ans[] = {6, 28, 496};
*/

int main() {
	int n, sum, i;
	n = 1;
	while(n < 500) {
		sum = 0;
		i = 1;
		while(i < n) {
			if(n % i == 0) {
				sum = sum + i;
			}
			i = i + 1;
		}
		
		if(sum == n) {
			put_int(n);
		}

		n = n + 1;
	}

	return 0;
}
