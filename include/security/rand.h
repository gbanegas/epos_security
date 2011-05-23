#include <system/config.h>


typedef struct rand_meth_st RAND_METHOD;

struct rand_meth_st
	{
	void (*seed)(const void *buf, int num);
	int (*bytes)(unsigned char *buf, int num);
	void (*cleanup)(void);
	int (*pseudorand)(unsigned char *buf, int num);
	int (*status)(void);
	};

#define RAND_F_RAND_GET_RAND_METHOD			 101
#define RAND_F_SSLEAY_RAND_BYTES			 100

/* Reason codes. */
#define RAND_R_PRNG_NOT_SEEDED				 100

int RAND_set_rand_method(const RAND_METHOD *meth);
const RAND_METHOD *RAND_get_rand_method(void);

void RAND_cleanup(void );
int  RAND_bytes(unsigned char *buf,int num);
int  RAND_pseudo_bytes(unsigned char *buf,int num);
void RAND_seed(const void *buf,int num);
//int  RAND_load_file(const char *file,long max_bytes);
int RAND_status(void);
