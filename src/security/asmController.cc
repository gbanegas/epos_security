#include <security/asmController.h>

__BEGIN_SYS
ASMController* ASMController::instance = 0;

ASMController::ASMController()
{

	this->key = 0;
	this->data = 0;
	initialize();
}

ASMController* ASMController::getInstance()
{
	if(instance == 0)
	    instance = new ASMController();

	return instance;
}

void ASMController::initialize()
{
	OStream cout;
        cout << "Passei pelo Initialize() Status = " << ASM_BITS->STATUS.TEST_PASS << "\n";
	ASM_BITS->CONTROL1.ON = 1;
	ASM_BITS->CONTROL1.SELF_TEST = 1;
	ASM_BITS->CONTROL0.START = 1;

	for(unsigned int i = 0; i < 3330; i++) { continue; }
        cout << "Passei pelo Initialize() Status = " << ASM_BITS->STATUS.TEST_PASS << "\n";

	ASM_BITS->CONTROL1.SELF_TEST = 0;
	ASM_BITS->CONTROL1.NORMAL_MODE = 1;
        ASM_BITS->CONTROL0.CLEAR = 1;
	ASM_BITS->CONTROL0.CLEAR_IRQ = 1;
        cout <<" estou saindo Initialize() " << "\n";
}

void ASMController::setKey(char* key)
{
    OStream cout;
    uint32_t temp, temp1, temp2, temp3, tempOu = 0 ;
    ASM_BITS->KEY0, ASM_BITS->KEY1, ASM_BITS->KEY2, ASM_BITS->KEY3 = 0;
    char temp_key[16];
    uint32_t i = 0;

    for(uint32_t j = 0 ; j < 17 ; j++)
    	{
		temp_key[j] = '\0';
    	}

    while(key[i] != '\0') { i++; }
    
    for(uint32_t j = 0 ; j < i ; j++)
    {
	temp_key[j] = key[j];
    }
    i++;
    if(i < 17){
    	for(uint32_t j = i ; j < 17 ; j++)
    	{
		temp_key[j] = '\0';
    	}
    }
    cout << temp_key << "\n";
    temp = (uint32_t)temp_key[0];
    temp1 = (uint32_t)temp_key[1] << 8;
    temp2 = (uint32_t)temp_key[2] << 16;
    temp3 = (uint32_t)temp_key[3] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->KEY0 = tempOu;
    temp, temp1, temp2, temp3, tempOu = 0;
    temp = (uint32_t)temp_key[5];
    temp1 = (uint32_t)temp_key[6] << 8;
    temp2 = (uint32_t)temp_key[7] << 16;
    temp3 = (uint32_t)temp_key[8] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->KEY1 = tempOu;
    temp, temp1, temp2, temp3, tempOu = 0;
    temp = (uint32_t)temp_key[9];
    temp1 = (uint32_t)temp_key[10] << 8;
    temp2 = (uint32_t)temp_key[11] << 16;
    temp3 = (uint32_t)temp_key[12] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->KEY2 = tempOu;
    temp, temp1, temp2, temp3, tempOu = 0;
    temp = (uint32_t)temp_key[13];
    temp1 = (uint32_t)temp_key[14] << 8;
    temp2 = (uint32_t)temp_key[15] << 16;
    temp3 = (uint32_t)temp_key[16] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->KEY3 = tempOu;


    cout << "KEY[0]" << (uint32_t *)ASM_BITS->KEY0 << "\n";
    cout << "KEY[1]" << (uint32_t *)ASM_BITS->KEY1 << "\n";
    cout << "KEY[2]" << (uint32_t *)ASM_BITS->KEY2 << "\n";
    cout << "KEY[3]" << (uint32_t *)ASM_BITS->KEY3 << "\n";
}

void ASMController::setData( char* data)
{
    OStream cout;
    ASM_BITS->CTR0 = 0x33333333;
    ASM_BITS->CTR1 = 0x22222222;
    ASM_BITS->CTR2 = 0x11111111;
    ASM_BITS->CTR3 = 0x00000000;
    uint32_t temp, temp1, temp2, temp3, tempOu = 0 ;
    ASM_BITS->DATA0, ASM_BITS->DATA1, ASM_BITS->DATA2, ASM_BITS->DATA3 = 0;
    char temp_data[16];
    uint32_t i = 0;

    for(uint32_t j = 0 ; j < 17 ; j++)
    	{
		temp_data[j] = '\0';
    	}

    while(data[i] != '\0') { i++; }
    
    for(uint32_t j = 0 ; j < i ; j++)
    {
	temp_data[j] = data[j];
    }
    i++;
    if(i < 17){
    	for(uint32_t j = i ; j < 17 ; j++)
    	{
		temp_data[j] = '\0';
    	}
    }
    cout << temp_data << "\n";
    temp = (uint32_t)temp_data[0];
    temp1 = (uint32_t)temp_data[1] << 8;
    temp2 = (uint32_t)temp_data[2] << 16;
    temp3 = (uint32_t)temp_data[3] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->DATA0 = tempOu;
    temp, temp1, temp2, temp3, tempOu = 0;
    temp = (uint32_t)temp_data[5];
    temp1 = (uint32_t)temp_data[6] << 8;
    temp2 = (uint32_t)temp_data[7] << 16;
    temp3 = (uint32_t)temp_data[8] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->DATA1 = tempOu;
    temp, temp1, temp2, temp3, tempOu = 0;
    temp = (uint32_t)temp_data[9];
    temp1 = (uint32_t)temp_data[10] << 8;
    temp2 = (uint32_t)temp_data[11] << 16;
    temp3 = (uint32_t)temp_data[12] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->DATA2 = tempOu;
    temp, temp1, temp2, temp3, tempOu = 0;
    temp = (uint32_t)temp_data[13];
    temp1 = (uint32_t)temp_data[14] << 8;
    temp2 = (uint32_t)temp_data[15] << 16;
    temp3 = (uint32_t)temp_data[16] << 24;
    tempOu = temp | temp1 | temp2 | temp3;
    ASM_BITS->DATA3 = tempOu;

    cout << "Data[0]" << (uint32_t *)ASM_BITS->DATA0 << "\n";
    cout << "Data[1]" << (uint32_t *)ASM_BITS->DATA1 << "\n";
    cout << "Data[2]" << (uint32_t *)ASM_BITS->DATA2 << "\n";
    cout << "Data[3]" << (uint32_t *)ASM_BITS->DATA3 << "\n";
}

void ASMController::setCRTMode()
{
    ASM_BITS->CONTROL1.CTR = 1;
}

void ASMController::cipher() {
    ASM_BITS->CONTROL0.START = 1;
    while(ASM_BITS->STATUS.DONE == 0) { continue; }
}

char* ASMController::getCipheredData() {
     char temp[20];
     uint32_t j = 0;
     uint32_t k = 0;

     for(unsigned int i = 0 ; i < 20 ; i ++){ this->ciphered_data[i] = '\0';}

     for(unsigned int i = 0 ; i < 20 ; i ++){ temp[i] = '\0';}
     utoa(ASM_BITS->CTR0_RESULT, temp);
     while(temp[j] != '\0'){ ciphered_data[j] = temp[j]; j++;}

     for(unsigned int i = 0 ; i < 20 ; i ++){ temp[i] = '\0';}
     utoa(ASM_BITS->CTR1_RESULT, temp);
     while(temp[k] != '\0'){ ciphered_data[j] = temp[k]; j++; k++;}

     for(unsigned int i = 0 ; i < 20 ; i ++){ temp[i] = '\0';}
     utoa(ASM_BITS->CTR2_RESULT, temp);
     k = 0;
     while(temp[k] != '\0'){ ciphered_data[j] = temp[k]; j++;k++;}

     for(unsigned int i = 0 ; i < 20 ; i ++){ temp[i] = '\0';}
     utoa(ASM_BITS->CTR3_RESULT, temp);
     k = 0;
     while(temp[k] != '\0'){ ciphered_data[j] = temp[k]; j++; k++;}
    
     j++;
     this->ciphered_data[j] = '\0';
   
    return this->ciphered_data;
}


int ASMController::utoa(unsigned int v, char * s, unsigned int i)
{
    OStream cout;
    unsigned int j,temp,temp1,temp2,temp3 = 0;
    int _base = 16;
    const char _digits[] = "0123456789abcdef";
    
    temp = (v & 0xff000000) >> 24;
    temp1 = (v & 0xff0000) >> 16;
    temp2 = (v  & 0xff00) >> 8;
    temp3 = (v & 0xff) ;
    cout << "temp: " << (char)temp << " " << (char)temp1 << " " << (char)temp2  << " " << (char)temp3   << "\n" ;

    if(!v) {
	s[i++] = '0';
	return i;
    }
    if(v > 256) {
	if(_base == 8 || _base == 16)
	    s[i++] = '0';
	if(_base == 16)
	    s[i++] = 'x';
    }

    for(j = v; j != 0; i++, j /= _base);
    for(j = 0; v != 0; j++, v /= _base)
	s[i - 1 - j] = _digits[v % _base];

    return i;
}


__END_SYS
