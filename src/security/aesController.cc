#include <security/aesController.h>

AESController::AESController()
{

	this->key = 0;
	this->data = 0;
	this->ciphered_data = 0;
	this->controller = ASMController::getInstance();
	this->buff = new Semaphore();
}

AESController::~AESController()
{
	delete[] this->key;
	delete[] this->data;
	delete[] this->ciphered_data;
}

void AESController::setKey(ByteArray key)
{

	this->key = key.toString();
}

void AESController::setKey(const unsigned char* key)
{
	this->key = (char*)key;
}

ByteArray* AESController::getCipheredData()
{

	return this->ciphered_data;

}

void AESController::setData(ByteArray* data)
{

	this->data = data;
}

char* AESController::cipher()
{
	
	this->buff->p();

        this->controller->initialize();

        this->controller->setKey(this->key);

        this->controller->setCRTMode();

        this->controller->setData(this->data);

        this->controller->cipher();
	
        this->ciphered_data = this->controller->getCipheredData();

	this->buff->v();	

	return this->ciphered_data->toString();

}

char* AESController::decipher()
{
	return this->cipher();
}

