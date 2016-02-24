
int fact(int f[15], int n) {
	if(n == 0 || n == 1) return 1;
	else return fact(f, n - 1) * n;
}

int main() {
	int f[15];
	int i = 0;
	while(i < 13) {
		f[i] = fact(f, i);
		write(f[i]);
		i = i + 1;
	}
	return 0;
}

