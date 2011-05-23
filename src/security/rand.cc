#include<security/rand.h>



static const RAND_METHOD *default_RAND_meth = 0;

int RAND_set_rand_method(const RAND_METHOD *meth)
{
	default_RAND_meth = meth;
	if(default_RAND_meth != 0)
		return 1;
	else
		return 0;
}

const RAND_METHOD *RAND_get_rand_method(void)
{
	return default_RAND_meth;
}

void RAND_cleanup(void)
	{
	const RAND_METHOD *meth = RAND_get_rand_method();
	if (meth && meth->cleanup)
		meth->cleanup();
	RAND_set_rand_method(0);
	}

void RAND_seed(const void *buf, int num)
	{
	const RAND_METHOD *meth = RAND_get_rand_method();
	if (meth && meth->seed)
		meth->seed(buf,num);
	}


int RAND_bytes(unsigned char *buf, int num)
	{
	const RAND_METHOD *meth = RAND_get_rand_method();
	if (meth && meth->bytes)
		return meth->bytes(buf,num);
	return(-1);
	}

int RAND_pseudo_bytes(unsigned char *buf, int num)
	{
	const RAND_METHOD *meth = RAND_get_rand_method();
	if (meth && meth->pseudorand)
		return meth->pseudorand(buf,num);
	return(-1);
	}

int RAND_status(void)
	{
	const RAND_METHOD *meth = RAND_get_rand_method();
	if (meth && meth->status)
		return meth->status();
	return 0;
	}
