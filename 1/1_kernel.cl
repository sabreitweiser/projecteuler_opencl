__kernel void divisible(__global float *div, const int num){
	size_t idx = get_global_id(0); //get id of executing thread
	num = idx+1; //Want to check if i+1 is divisible
	if (idx < num){
		if (num%3 == 0){
			div[idx] = 1;
		} else if (num%5 == 0){
			div[idx] = 1;
		} else {
			div[idx] = 0;
		}
	}
}
