// EPOS-- MC13224V Default Application

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <alarm.h>
#include <security/aesController.h>

__USING_SYS

void wait() {
    for(unsigned int i = 0; i < 0x000fffff; i++);
    Alarm::delay(2000000);
}

void turn_on_led1() {
    unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
    *GPIO_BASE = 1 << 23;
}

void turn_on_led2() {
    unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
    *GPIO_BASE = 1 << 24;
}




void testCipher()
{
   OStream cout;
   AESController controller;
   controller.setKey("0x12345678");
   controller.setData("aaaaaaaaaaaaaa");
   char * test = controller.cipher();
   cout << "cifrado: " << test << "\n";
   //controller.setData(test);
   //test = controller.cipher();
   //cout <<"decifrado: " <<  test << "\n";
   delete[] test;
}


int main() {
    unsigned int i = 0;
    turn_on_led1();
    wait();
    turn_on_led2();
    wait();
    testCipher();
  

//    source();
}
