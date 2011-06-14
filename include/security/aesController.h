#ifndef AESCONTROLLER_H_
#define AESCONTROLLER_H_

#include <system/config.h>
#include <utility/string.h>
#include <utility/vector.h>
#include <semaphore.h>
#include "asmController.h"
#include "ByteArray.h"

__USING_SYS
//using namespace std;
class AESController {

	
public:
    AESController();
    virtual ~AESController();

    void setKey(ByteArray key);
    void setKey(const unsigned char* key);

    void setData(ByteArray * data);

    char* cipher();

    char* decipher();
	

    ByteArray* getCipheredData();

    //char* toChar();

    //char* toHex();
    

private:
    ByteArray * data;
    ByteArray * ciphered_data;
    char* key;
    ASMController * controller;
    Semaphore * buff;


};
#endif
