/**
 * This file is for test.
 * Can't compile
 */
void main( int argc, char** argv ){
	/*
	pebble(alpha1);
	pebble(beta1);
	*/
	pebble(gamma1);
	// pebble(delta1);
	pebble(epsilon1);

#if 0
	/*
	pebble(alpha2);
	pebble(beta2);
	*/
	pebble(gamma2);
	// pebble(delta2);
	pebble(epsilon2);
#elif defined(DEBUG)

	/*
	pebble(alpha3);
	pebble(beta3);
	*/
	pebble(gamma3);
	// pebble(delta3);
	pebble(epsilon3);
#else
	#if 0
		/*
		pebble(alpha4);
		pebble(beta4);
		*/
		pebble(gamma4);
		// pebble(delta4);
		pebble(epsilon4);
	#else
		/*
		pebble(alpha5);
		pebble(beta5);
		*/
		pebble(gamma5);
		// pebble(delta5);
		pebble(epsilon5);
	#endif
#endif
	/*
	pebble(alpha6);
	pebble(beta6);
	*/
	pebble(gamma6);
	// pebble(delta6);
	pebble(epsilon6);

	return 0;
}

