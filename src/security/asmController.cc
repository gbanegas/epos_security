#include <security/asmController.h>

ASMController* ASMController::instance = 0;

ASMController::ASMController()
{

	this->key = 0;
	this->data = 0;
	this->ciphered_data = 0;
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
