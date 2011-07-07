#include <security/aesController.h>

__BEGIN_SYS
AESController* AESController::instance = 0;

AESController::AESController()
{
        this->controller = ASMController::getInstance();
	this->buff = new Semaphore();
}

AESController* AESController::getInstance()
{
	if(instance == 0)
	    instance = new AESController();

	return instance;
}
bool AESController::cipher(char* data, char * key, char* ciphered_data)
{
	
	this->buff->p();

        this->controller->setCRTMode();
        
	this->controller->setKey(key);
	
	this->controller->setData(data);

        if(this->controller->cipher(ciphered_data)){
		return true;
	}
	else
	{
		ciphered_data = '\0';
		return false;
	}
	this->buff->v();	

}

bool AESController::decipher(char* data, char* key, char* ciphered_data)
{
	return this->cipher(data, key, ciphered_data);
}

__END_SYS

