
__BEGIN_SYS
#define ASMADDR ((struct ASM_struct *)0x80008000)

typedef unsigned int u32;

 struct	ASM_struct {
        	 u32 KEY0;
        	 u32 KEY1;
        	 u32 KEY2;
        	 u32 KEY3;
        	 u32 DATA0;
        	 u32 DATA1;
        	 u32 DATA2;
        	 u32 DATA3;
        	 u32 CTR0;
        	 u32 CTR1;
        	 u32 CTR2;
        	 u32 CTR3;
        	 u32 CTR0_RESULT;
        	 u32 CTR1_RESULT;
        	 u32 CTR2_RESULT;
        	 u32 CTR3_RESULT;
        	 u32 CBC0_RESULT;
        	 u32 CBC1_RESULT;
        	 u32 CBC2_RESULT;
        	 u32 CBC3_RESULT;


                         struct ASM_CONTROL0 {
        	          u32 :24;
        	          u32 START:1;
        	          u32 CLEAR:1;
	             	  u32 LOAD_MAC:1;
	             	  u32 :4;
	             	  u32 CLEAR_IRQ:1;
	         	} CONTROL0;



	        	struct ASM_CONTROL1 {
	        	     u32 ON:1;
	        	     u32 NORMAL_MODE:1;
	        	     u32 BYPASS:1;
	        	     u32 :21;
	        	     u32 CBC:1;
	        	     u32 CTR:1;
	        	     u32 SELF_TEST:1;
	        	     u32 :4;
	        	     u32 MASK_IRQ:1;
	        	 } CONTROL1;


	         	struct ASM_STATUS {
	        	     u32 :24;
	        	     u32 DONE:1;
	         	     u32 TEST_PASS:1;
	         	     u32 :6;
	         	} STATUS;

 
	         u32 reserved;
	         u32 MAC0;
	         u32 MAC1;
	         u32 MAC2;
	         u32 MAC3;
};

static volatile struct ASM_struct * const ASM_BITS = ASMADDR;
__END_SYS
