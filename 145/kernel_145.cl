int is_rev(int num){
	if (num%10 == 0)
		return 0;
	int rev_order = 1;
	while (rev_order <= num)
		rev_order *= 10;
	rev_order /= 10;
	int rev = num;
	int num_divisor = 10;
	int num_order = 1;
	int rem = 0;
	int num_digit, rev_digit, res;
	while (num > 0){
		num_digit = (num%num_divisor)/num_order;
		rev_digit = rev/rev_order;
		res = num_digit + rev_digit + rem;
		if (res%2 == 0)
			return 0;
		rem = res/10;
		num -= (num_digit)*num_order;
		rev -= (rev_digit)*rev_order;
		rev_order /= 10;
		num_order *= 10;
		num_divisor *= 10;
	}
	return 1;
}

__kernel void rev_count(__global int *rev, const int max){
	const int idx = get_global_id(0); //get id of executing thread
	if (idx >= max)
		return;
	int n;
	rev[idx] = 0;
	
	for (n = idx*1000; n < (idx+1)*1000; n++){
			rev[idx] += is_rev(n);
	}
}
