__kernel void digits_kernel(__global long *digits, const int max){
	const int idx = get_global_id(0);
	if (idx >= max) return;
	const int n = idx+1;
	int i;
	long ord = 10000000000;
	long tot = 1;
	for(i=0; i<n; i++){
		 tot *= n;
		 tot %= ord;
	}
	digits[idx] = tot;
}
