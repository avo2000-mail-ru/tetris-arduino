/*
* arch.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-31 14:48:44
*/

#ifndef _id_aff6639a_2141_4f08_a5de_88632478b17b
#define _id_aff6639a_2141_4f08_a5de_88632478b17b

#define lo(addr) (*((unsigned short *)&(addr) + 1))
#define hi(addr) (*((unsigned short *)&(addr)))

static inline int GT_ULONG(unsigned long a, unsigned long b) {
    if ( hi(a) == hi(b) )
        return lo(a) > lo(b);
    return hi(a) > hi(b);
}

static inline int GT_LONG(unsigned long a, unsigned long b) {
    if ( hi(a) == hi(b) )
        return lo(a) > lo(b);
    return (int)hi(a) > (int)hi(b);
}

static inline int GE_ULONG(unsigned long a, unsigned long b) {
    if ( hi(a) == hi(b) )
        return lo(a) >= lo(b);
    return hi(a) >= hi(b);
}

static inline int GE_LONG(unsigned long a, unsigned long b) {
    if ( hi(a) == hi(b) )
        return lo(a) >= lo(b);
    return (int)hi(a) >= (int)hi(b);
}

#endif /* _id_aff6639a_2141_4f08_a5de_88632478b17b */
