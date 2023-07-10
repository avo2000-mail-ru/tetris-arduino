/*
* TetrisLib.h
* Copyright 2023 Aleksandr Orlov
* avo2000@mail.ru
* created: 2023-07-05 12:13:40
*/

#ifndef _id_1932c18a_3257_4711_9925_988c3cbfd57f
#define _id_1932c18a_3257_4711_9925_988c3cbfd57f

#ifdef _MSC_VER
  #ifdef _DLL
    #ifdef __cplusplus
      #define _EXPORTS(type) extern "C" __declspec(dllexport) type __stdcall
    #else
      #define _EXPORTS(type) __declspec(dllexport) type __stdcall
    #endif
  #else
    #ifdef __cplusplus
      #define _EXPORTS(type) extern "C" __declspec(dllimport) type __stdcall
    #else
      #define _EXPORTS(type) __declspec(dllimport) type __stdcall
    #endif
  #endif
#else
  #ifdef __cplusplus
    #define _EXPORTS(type) extern "C" type
  #else
    #define _EXPORTS(type) type
  #endif
#endif

#endif /* _id_1932c18a_3257_4711_9925_988c3cbfd57f */
