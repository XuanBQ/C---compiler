struct dummy {
	int pad1[100];
} d;

struct dummy fun(struct dummy a) {
	return a;
}

int main() {
	int N = 100;
	int i = 0;
	struct dummy t;
	while(i < N) {
		d.pad1[i] = i + 128;
		i = i + 1;
	}

	t = fun(d);

	i = 0;
	while(i < N) {
		put_int(d.pad1[i] - 128);
		i = i + 1;
	}

	return 0;
}
