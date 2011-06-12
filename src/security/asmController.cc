#include <security/asmController.h>

ASMController* ASMController::instance = 0;

ASMController::ASMController()
{

	this->key = 0;
	this->data = 0;
	this->ciphered_data = 0;
	initialize();
}

ASMController::~ASMController()
{
	delete[] this->key;
	delete[] this->data;
	delete[] this->ciphered_data;
}

ASMController* ASMController::getInstance()
{
	if(instance == 0)
	    instance = new ASMController();

	return instance;

}

void ASMController::initialize()
{

	//ASM_BITS->CONTROL1bits.ON = 1;
	//ASM_BITS->CONTROL1bits.SELF_TEST = 1;
	//ASM_BITS->CONTROL1bits.START = 1;

	//for(unsigned int i = 0; i < 3330; i++) { continue; }

	//ASM_BITS->CONTROL1bits.SELF_TEST = 0;
	//ASM_BITS->CONTROL1bits.NORMAL_MODE = 1;
	
}

