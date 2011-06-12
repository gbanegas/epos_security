#include <security/aesController.h>


AESController::AESController()
{

	this->key = 0;
	this->data = 0;
	this->ciphered_data = 0;
}

AESController::~AESController()
{
	delete[] this->key;
	delete[] this->data;
	delete[] this->ciphered_data;
}
