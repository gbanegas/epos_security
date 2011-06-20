#ifndef AESCONTROLLER_H_
#define AESCONTROLLER_H_

#include <system/config.h>
#include <semaphore.h>
#include "asmController.h"

__BEGIN_SYS
class AESController {

	
public:
    AESController();
    //virtual ~AESController();

    void setKey(char* key);

    void setData(char* data);

    char* cipher();

    char* decipher();
	
    char* getCipheredData();


private:
    char * data;
    char * ciphered_data;
    char* key;
    ASMController * controller;
    Semaphore * buff;
};
__END_SYS
#endif
