#include <security/aesController.h>

__BEGIN_SYS
AESController::AESController()
{

	this->key = 0;
	this->data = 0;
	this->ciphered_data = 0;
	this->controller = ASMController::getInstance();
	this->buff = new Semaphore();
}



void AESController::setKey( char* key)
{
	this->key = key;
}

char* AESController::getCipheredData()
{

	return this->ciphered_data;

}

void AESController::setData( char* data)
{
	this->data = data;
}

char* AESController::cipher()
{
	
	this->buff->p();

        //this->controller->initialize();

        this->controller->setKey(this->key);

        this->controller->setCRTMode();

        this->controller->setData(this->data);

        if(this->controller->cipher()){
	
        	this->ciphered_data = this->controller->getCipheredData();
	}
	else
	{
		this->ciphered_data = '\0';
	}
	this->buff->v();	

	return this->ciphered_data;

}

char* AESController::decipher()
{
	return this->cipher();
}

__END_SYS

