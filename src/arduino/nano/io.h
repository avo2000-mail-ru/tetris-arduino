/*
* io.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 04:08:57
*/

#ifndef _id_0d504ee0_0b16_4859_92b9_a75c8953b1db
#define _id_0d504ee0_0b16_4859_92b9_a75c8953b1db

#ifdef __cplusplus
extern "C" {
#endif

unsigned long io_now();

int write(const char *buffer, int size);
int available();
int getchr();

#ifdef __cplusplus
}
#endif

#endif /* _id_0d504ee0_0b16_4859_92b9_a75c8953b1db */
