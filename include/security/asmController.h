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

    void setData(ByteArray &data);

    ByteArray& doFinal();
	

    ByteArray getCipheredData();
    char* toChar();

    char* toHex();
    

private:
    void initialize();
    ByteArray* data;
    ByteArray* ciphered_data;
    char* key;
    static ASMController* instance;
    //ASMBits * bits;

    



};

#endif
