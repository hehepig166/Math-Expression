#ifndef HEHEPIG_ALGORITHM
#define HEHEPIG_ALGORITHM

namespace my_algorithm {

	template<typename T1, typename T2>
	T1 qpow(T1 X, T2 k)
	{
		T1 ret=1;
		while (k>0) {
			if (k%2 == 1)
				ret*=X;
			X*=X;
			k/=2;
		}
		return ret;
	}
	
}



#endif