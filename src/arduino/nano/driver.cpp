/*
* driver.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 02:45:49
*/

#include <stdlib.h> 

extern void Main();

void* operator new(short unsigned int size) noexcept { return malloc(size); }
void* operator new[](short unsigned int size) noexcept { return malloc(size); }
void operator delete(void* p) noexcept { free(p); }
//void operator delete(void* p,short unsigned int size) noexcept { free(p); }
void operator delete[](void* p) noexcept { free(p); }

void _main() {
    Main();
}