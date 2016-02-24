int a[100];

int partition(int p, int q) {
	int pivot = a[p];
	int i = p, j = q;
	while(i < j) {
		while(i < j && a[j] > pivot) j = j - 1;
		a[i] = a[j];

		while(i < j && a[i] <= pivot) i = i + 1;
		a[j] = a[i];
	}

	a[i] = pivot;
	return i;
}

int quick_sort(int p, int q) {
	int m;
	if(p >= q) return 0;

	m = partition(p, q);
	quick_sort(p, m - 1);
	quick_sort(m + 1, q);
}

int main() {
	int N = 100;
	int i = 0;
	while(i < N) {
		a[i] = N - i;
		i = i + 1;
	}

	quick_sort(a, 0, N - 1);

	i = 0;
	while(i < N) {
		put_int(a[i]);
		i = i + 1;
	}

	return 0;
}
