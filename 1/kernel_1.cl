__kernel void divisible(__global int *div, const int num){
	const int idx = get_global_id(0); //get id of executing thread
	int n;
	if (idx < num){
		n = idx+1;
		if (n%3 == 0){
			div[idx] = 1;
		} else if (n%5 == 0){
			div[idx] = 1;
		} else {
			div[idx] = 0;
		}
	}
}
