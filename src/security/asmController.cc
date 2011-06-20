#include <security/asmController.h>

__BEGIN_SYS
ASMController* ASMController::instance = 0;

ASMController::ASMController()
{

	this->key = 0;
	this->data = 0;
	this->ciphered_data = 0;
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
	cout << "ASM Bits " << (unsigned int) ASM_BITS << "\n";
	ASM_BITS->CONTROL1.ON = 1;
	ASM_BITS->CONTROL1.SELF_TEST = 1;
	ASM_BITS->CONTROL0.START = 1;

	for(unsigned int i = 0; i < 3330; i++) { continue; }
        cout << "Passei pelo Initialize() Status = " << ASM_BITS->STATUS.TEST_PASS << "\n";

	ASM_BITS->CONTROL1.SELF_TEST = 0;
	ASM_BITS->CONTROL1.NORMAL_MODE = 1;
        cout <<"estou saindo " << "\n";
}

void ASMController::setKey(char* key)
{
    OStream cout;
    cout << "Passei pelo Key()" << key << "\n";
    ASM_BITS->KEY0 = *(uint32_t *)key[0];
    ASM_BITS->KEY1 = *(uint32_t *)key[1];
    ASM_BITS->KEY2 = *(uint32_t *)key[2];
    ASM_BITS->KEY3 = *(uint32_t *)key[3];
}

void ASMController::setData( char* key)
{
    OStream cout;
    cout << "Passei pelo Data()" << key << "\n";
    ASM_BITS->CTR0 = *(uint32_t *)key[0];
    ASM_BITS->CTR1 = *(uint32_t *)key[4];
    ASM_BITS->CTR2 = *(uint32_t *)key[8];
    ASM_BITS->CTR3 = *(uint32_t *)key[12];

    ASM_BITS->DATA0 = *(uint32_t *)key[16];
    ASM_BITS->DATA1 = *(uint32_t *)key[20];
    ASM_BITS->DATA2 = *(uint32_t *)key[24];
    ASM_BITS->DATA3 = *(uint32_t *)key[28];
}

void ASMController::setCRTMode()
{
    ASM_BITS->CONTROL1.CTR = 1;
}

void ASMController::cipher() {
    ASM_BITS->CONTROL0.START = 1;
    OStream cout;
    cout << "Passei pelo cipher() DONE = " << ASM_BITS->STATUS.DONE << "\n";
    while(ASM_BITS->STATUS.DONE == 0) { continue; }
    cout << "saindo do cipher() DONE = " << ASM_BITS->STATUS.DONE << "\n";
}

char* ASMController::getCipheredData() {
    this->ciphered_data[0] =  ASM_BITS->CTR0_RESULT       & 0xff;
    this->ciphered_data[1] =  ASM_BITS->CTR0_RESULT >> 8  & 0xff;
    this->ciphered_data[2] =  ASM_BITS->CTR0_RESULT >> 16 & 0xff;
    this->ciphered_data[3] =  ASM_BITS->CTR0_RESULT >> 24 & 0xff;

    this->ciphered_data[4] =  ASM_BITS->CTR1_RESULT       & 0xff;
    this->ciphered_data[5] =  ASM_BITS->CTR1_RESULT >> 8  & 0xff;
    this->ciphered_data[6] =  ASM_BITS->CTR1_RESULT >> 16 & 0xff;
    this->ciphered_data[7] =  ASM_BITS->CTR1_RESULT >> 24 & 0xff;

    this->ciphered_data[8] =  ASM_BITS->CTR2_RESULT       & 0xff;
    this->ciphered_data[9] =  ASM_BITS->CTR2_RESULT >> 8  & 0xff;
    this->ciphered_data[10] = ASM_BITS->CTR2_RESULT >> 16 & 0xff;
    this->ciphered_data[11] = ASM_BITS->CTR2_RESULT >> 24 & 0xff;

    this->ciphered_data[12] = ASM_BITS->CTR3_RESULT       & 0xff;
    this->ciphered_data[13] = ASM_BITS->CTR3_RESULT >> 8  & 0xff;
    this->ciphered_data[14] = ASM_BITS->CTR3_RESULT >> 16 & 0xff;
    this->ciphered_data[15] = ASM_BITS->CTR3_RESULT >> 24 & 0xff;

    return this->ciphered_data;
}
__END_SYS
