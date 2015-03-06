bool is_pal(int n){
	int lim = 10;
	int order = 1;
	while (lim <= n){
		lim *= 10;
		order++;
	}
	int tmp = 1;
	int i;
	for(i=0; i<order/2; i++){
		 if ((n%(10*tmp) - n%tmp)/tmp != (n%lim - n%(lim/10))/(lim/10))
		 	return false;
		tmp *= 10;
		lim /= 10;
	}
	return true;
}

__kernel void largest_pal(__global int *largest_pals){
	const int idx = get_global_id(0); //get id of executing thread
	const int n = idx+1;
	int max = 0;
	int i;
	for(i=1; i<= n; i++)
		if (is_pal(i*n))
			max = i*n;
	largest_pals[idx] = max;
}
