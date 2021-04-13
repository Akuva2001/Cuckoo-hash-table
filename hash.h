//
// Created by ivan on 13.04.2021.
//

#ifndef CUCKOOHASH_HASH_H
#define CUCKOOHASH_HASH_H


//#pragma once
// Èñïîëüçîâàíèå:
// CRCCoder<unsigned> crc32;
//    auto hash = crc32.calc("MyString", 8);
//
//   Âòîðîé âàðèàíò:
//      for (auto c: "MyString") {
//          crc32.update(c);
//      }
//      auto hash = crc32.final();
template<typename T>
class CRCCoder {
public:
    CRCCoder() {
        init();
    }

    static T calc(const void *in, size_t byte_size) {
        const char * p = (const char *) in;
        T ctmp = T(-1);
        for (int len = 0; len<byte_size; len++) {
            ctmp ^= (T)(p[len]);
            ctmp = (ctmp >> 8) ^ _table[ctmp & 0xFF];
        }
        return ctmp ^ T(-1);
    }

    void init() {
        initTable();
        _value = T(-1);
    }


private:
    T _value;
    static T _table[256];
    static T POLY;
    static void initTable() {
        if (_table[0] == 0) {
            T *p = _table;
            int             len, j;
            for (j = 0; j < 256; j++) {
                T x = j;
                for (len = 0; len < 8; len++) {
                    if ((x & 1) != 0) {
                        x = (x >> 1) ^ POLY;
                    }
                    else {
                        x = (x >> 1);
                    }
                }
                *p++ = x;
            }
        }
    }
};

template<> unsigned long long CRCCoder<unsigned long long>::POLY = 0xC96C5795D7870F42ULL;
template<> unsigned CRCCoder<unsigned>::POLY = 0xEDB88320U;
template<> unsigned long long CRCCoder<unsigned long long>::_table[256] = { 0 };
template<> unsigned CRCCoder<unsigned>::_table[256] = { 0 };


#endif //CUCKOOHASH_HASH_H
