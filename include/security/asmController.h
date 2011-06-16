#ifndef ASMCONTROLLER_H_
#define ASMCONTROLLER_H_

#include <system/config.h>
#include <utility/string.h>
#include <utility/vector.h>

#include "ByteArray.h"
#include "asm.h"

//using namespace std;
class ASMController {

protected:

    ASMController();
    virtual ~ASMController();
	
public:
    
    static ASMController* getInstance();

    void setKey(ByteArray key);
    void setKey(const unsigned char* key);

    void setData(ByteArray *data);
    void setData(const unsigned char* key);

    void setCRTMode();

    void cipher();

    ByteArray* getCipheredData();
    char* toChar();

    char* toHex();

     void initialize();

private:
   
    ByteArray* data;
    ByteArray* ciphered_data;
    char* key;
    static ASMController* instance;
    typedef unsigned int uint32_t;
};

#endif
