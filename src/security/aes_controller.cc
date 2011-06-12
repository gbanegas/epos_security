#include <security/aes_controller.h>

AES_Controller::AES_Controller()
{
    key = 0;
}

AES_Controller::~AES_Controller()
{
    delete[] key;
}
