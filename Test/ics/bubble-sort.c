int bubble_sort(int a[100], int N) {
int i, j = 0, t;
	while(j < N) {
		i = 0;
		while(i < N - 1) {
			if(a[i] > a[i + 1]) {
				t = a[i];
				a[i] = a[i + 1];
				a[i + 1] = t;
			}
			i = i + 1;
		}
		j = j + 1;
	}
	return 0;
}

int main() {
	int i = 0;
	int a[100];
	int N = 100;
	while(i < N) {
		a[i] = N - i;
		i = i + 1;
	}

	bubble_sort(a, 100);

	i = 0;
	while(i < N) {
	    write(a[i]);	
		i = i + 1;
	}

	return 0;
}
