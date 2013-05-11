/**
 * This file is for test.
 * Can't compile
 */
void main( int argc, char** argv ){
	/*
	pebble(c_alpha1);
	pebble(c_beta1);
	*/
	pebble(c_gamma1);
	// pebble(c_delta1);
	pebble(c_epsilon1);

#if 0
	/*
	pebble(c_alpha2);
	pebble(c_beta2);
	*/
	pebble(c_gamma2);
	// pebble(c_delta2);
	pebble(c_epsilon2);
#elif defined(DEBUG)

	/*
	pebble(c_alpha3);
	pebble(c_beta3);
	*/
	pebble(c_gamma3);
	// pebble(c_delta3);
	pebble(c_epsilon3);
#else
	#if 0
		/*
		pebble(c_alpha4);
		pebble(c_beta4);
		*/
		pebble(c_gamma4);
		// pebble(c_delta4);
		pebble(c_epsilon4);
	#else
		/*
		pebble(c_alpha5);
		pebble(c_beta5);
		*/
		pebble(c_gamma5);
		// pebble(c_delta5);
		pebble(c_epsilon5);
	#endif
#endif
	/*
	pebble(c_alpha6);
	pebble(c_beta6);
	*/
	pebble(c_gamma6);
	// pebble(c_delta6);
	pebble(c_epsilon6);

	return 0;
}

