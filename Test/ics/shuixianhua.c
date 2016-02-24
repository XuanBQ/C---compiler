/*
int ans[] = {153, 370, 371, 407};
*/

int cube(int n) {
	return n * n * n;
}

int main() {
	int n, n2, n1, n0;
	n = 100;
	while(n < 1000) {
		n2 = n / 100;
		n1 = (n / 10) % 10;
		n0 = n % 10;

		if(n == cube(n2) + cube(n1) + cube(n0)) {
			put_int(n);
		}

		n = n + 1;
	}

	return 0;
}
