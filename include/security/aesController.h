#ifndef AESCONTROLLER_H_
#define AESCONTROLLER_H_

#include <system/config.h>
#include <semaphore.h>
#include "asmController.h"

__BEGIN_SYS
class AESController {


protected:
     AESController();
	
public:
    static AESController* getInstance();
    //virtual ~AESController();

    bool cipher(char* data, char* key, char* ciphered_data);

    bool decipher(char* data, char* key, char* ciphered_data);
	
    char* getCipheredData();


private:
    static AESController* instance;
    ASMController * controller;
    Semaphore * buff;
};
__END_SYS
#endif
