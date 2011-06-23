#ifndef ASMCONTROLLER_H_
#define ASMCONTROLLER_H_

#include <system/config.h>
#include "asm.h"
__BEGIN_SYS
class ASMController {

protected:

    ASMController();
    //virtual ~ASMController();
	
public:
    
    static ASMController* getInstance();

    void setKey(char* key);

    void setData(char* key);

    void setCRTMode();

    bool cipher();

    char* getCipheredData();
    char* toChar();

    char* toHex();

     void initialize();

private:
    //int strlen(char * str);
    int utoa(unsigned int v, char * s, unsigned int i = 0);
    char* data;
    char ciphered_data[128];
    char* key;
    static ASMController* instance;
    typedef unsigned int uint32_t;
};
__END_SYS
#endif
