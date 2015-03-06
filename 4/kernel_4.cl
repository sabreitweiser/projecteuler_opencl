__kernel void fib(__global int *fib, const int num){
	const int idx = get_global_id(0); //get id of executing thread
	int tmp;
	int fib1 = 1;
	int fib2 = 1;
	int i;
	for (i=2; (i<idx) && (fib2<num); i++){
		  tmp = fib2;
		  fib2 = fib1+fib2;
		  fib1 = tmp;
	}
	if (fib2<num){
		if (fib2%2 == 0){
			fib[idx] = fib2;
			return;
		}
	}
	fib[idx] = 0;
}
