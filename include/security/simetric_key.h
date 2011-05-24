#ifndef SIMETRICKEY_H_
#define SIMETRICKEY_H_

#include <system/config.h>
#include <utility/string.h>
#include <utility/vector.h>

#include "ByteArray.h"

using namespace std;

class Simetric_Key {
	
public:
    Simetric_Key();
    virtual ~Simetric_Key();
    Simetric_Key(ByteArray key);
    Simetric_Key(const unsigned char* key);

    void setKey(ByteArray key);
    void setKey(const unsigned char* key);

    ByteArray getSimetricKey();
    char* toChar();

    char* toHex();
    

private:
    ByteArray key;



};

#endif
