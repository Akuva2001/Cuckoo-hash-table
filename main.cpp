#include <iostream>
#include <mutex>
#include <cassert>
#include <atomic>
#include <cstdio>
#include "hash.h"
#include <thread>
#include <vector>
#include <chrono>

//#define ONE_THREAD_DEBUG
//#define WTF_happens

typedef unsigned int Table_el_type;



class Hash {
    Table_el_type mod;
    CRCCoder<unsigned> crc;
    std::hash<Table_el_type> stdhash;
public:
    Hash (Table_el_type mod) : mod(mod) {}
    size_t operator() (Table_el_type in) const{
        return (mod==0)? crc.calc(&in, sizeof(in)):stdhash(in);
    }
};

#define TRESHOLD 2
#define PROBSIZE 4
#define relocate_LIMIT 10

class Probe{
public:
    int capacity = 0, begin = 0, end = 0;
    Table_el_type arr[PROBSIZE] {};
    bool push(Table_el_type in){
        if(capacity>=PROBSIZE) return false;
        end%=PROBSIZE;
        arr[end] = in;
        ++end;
        ++capacity;
        return true;
    }
    Table_el_type pop(){
        assert(capacity>0);
        Table_el_type r = arr[begin];
        arr[begin] = 0;
        begin = (begin+1)%PROBSIZE;
        --capacity;
        return r;
    }
    Table_el_type back(){
        assert(capacity>0);
        return arr[begin];
    }
    void print(){
        for (int i = 0; i<PROBSIZE; ++i)
            std::cout<<arr[i]<<' ';
        std::cout<<'\n';
    }
    bool contains(Table_el_type x){
        if (capacity == 0) return false;
        for (int i = begin; i!=end; i++) {
            i%=PROBSIZE;
            if (x == arr[i])
                return true;
        }
        return false;
    }
    bool erase(Table_el_type x){
        if (capacity == 0) return false;
        Probe c;
        bool t = false;
        for (int i = begin; i!=end; i++) {
            i %= PROBSIZE;
            if (x != arr[i])
                c.push(arr[i]);
            else
                t = true;
        }
        *this = c;
        return t;
    }
};

class Table {
public:
    int size, lock_size;
    Probe * arr;
    Hash hash;
    int position(Table_el_type x) const{
        return hash(x)%size;
    }
    std::mutex * lock_arr;
    Table(int size, int lock_size, int hashmode) : size(size), lock_size(lock_size), hash(hashmode){
        arr = new Probe[size];
        lock_arr = new std::mutex[lock_size];
    }
    ~Table(){
        delete[] arr;
        delete[] lock_arr;
    }
    void lockn(int h) const{
        lock_arr[h%lock_size].lock();
    }
    void unlockn(int h) const{
        lock_arr[h%lock_size].unlock();
    }
    bool contains_without_lock(Table_el_type x, int pos) const{
        bool r = arr[pos].contains(x);
        return r;
    }
    bool erase_without_lock(Table_el_type x, int pos) const{
        bool r = arr[pos].erase(x);
        return r;
    }
    bool put_without_lock(Table_el_type x, int pos) const{
        bool r = arr[pos].push(x);
        return r;
    }
    int capacity_without_lock(int pos) const{
        int r = arr[pos].capacity;
        return r;
    }
};


struct PositionPack{
    Table_el_type x;
    int pos[2];

};

class CuckooHash{
public:
    Table t0, t1;
    int size, lock_size;
    std::atomic<int> version {0};
    std::mutex resize_lock;
    bool print_resize = true;
    CuckooHash(int size, int lock_size, bool print_resize = true) :
        size(size), lock_size(lock_size),
        print_resize(print_resize),
        t0(size, lock_size, 0),
        t1(size, lock_size, 1) {}
    PositionPack pospack(Table_el_type x) const {
        return PositionPack{x, {t0.position(x), t1.position(x)}};
    }
    void acquire(PositionPack h, bool block = true) const{
        if (!block) return;
        t0.lockn(h.pos[0]);
        t1.lockn(h.pos[1]);
    }
    void release(PositionPack h, bool block = true) const{
        if (!block) return;
        t1.unlockn(h.pos[1]);
        t0.unlockn(h.pos[0]);
    }
    bool contains(Table_el_type x) const{
        int vers = version;
        PositionPack h{pospack(x)};
        acquire(h);//###!!!!
        if (vers<version){
            release(h);
            return contains(x);
        }

        bool r = t0.contains_without_lock(x, h.pos[0]) || t1.contains_without_lock(x, h.pos[1]);
        release(h);
        return r;
    }
    bool erase(Table_el_type x){
        int vers = version;
        PositionPack h{pospack(x)};
        acquire(h);//###!!!!
        if (vers<version){
            release(h);
            return erase(x);
        }

        bool r = t0.erase_without_lock(x, h.pos[0]) || t1.erase_without_lock(x, h.pos[1]);
        release(h);
        return r;
    }
    bool add(Table_el_type x, bool block = true){
        int vers = version;
        PositionPack h{pospack(x)};
        acquire(h, block);//###!!!!
        if (vers<version){
            release(h, block);
            return add(x, block);
        }

        if (t0.contains_without_lock(x, h.pos[0]) || t1.contains_without_lock(x, h.pos[1])) {
            release(h, block);
            return true;//already exists
        }
        bool result = false;
        bool must_resize = false;
        int i = -1, pos = -1;
        if (t0.capacity_without_lock(h.pos[0])<TRESHOLD) {
            result = t0.put_without_lock(x, h.pos[0]);
            release(h, block);//###!!!!
            return result;
        } else if (t1.capacity_without_lock(h.pos[1])<TRESHOLD) {
            result = t1.put_without_lock(x, h.pos[1]);
            release(h, block);//###!!!!
            return result;
        } else if (t0.capacity_without_lock(h.pos[0])<PROBSIZE) {
            t0.put_without_lock(x, h.pos[0]);
            i = 0; pos = h.pos[0];
        } else if (t1.capacity_without_lock(h.pos[1])<PROBSIZE) {
            t1.put_without_lock(x, h.pos[1]);
            i = 1; pos = h.pos[1];
        } else{
            must_resize = true;
        }
        release(h, block);
        if (must_resize) {
            assert(block);
            return resize() && add(x);
        }
        assert(i>=0 && pos >=0);
        if (!(result = relocate(i, pos, block))){
            assert(block);
            return resize();
        }
        return result;

    }
    bool resize(){
#ifdef ONE_THREAD_DEBUG
        printf("resize\n");
#endif

        int vers = version;
        resize_lock.lock();
        if (vers<version){
            resize_lock.unlock();
            return true;
        }

        for (int i=0; i<t0.lock_size; ++i){
            t0.lock_arr[i].lock();
        }
        for (int i=0; i<t0.lock_size; ++i){
            t1.lock_arr[i].lock();
        }//locked all
        if (print_resize){
            printf("\n");
            print();
        }
        ++version;
        Probe *old_Tables[2] = {t0.arr, t1.arr};
        int old_size = size;
        size *=2;
        t0.arr = new Probe[size];
        t1.arr = new Probe[size];
        t0.size = t1.size = size;
        for (int j = 0; j<2; ++j)
            for (int i=0; i<old_size; ++i)
                if (old_Tables[j][i].capacity>0)
                    for (int k = old_Tables[j][i].begin; k!=old_Tables[j][i].end; k++) {
                        k%=PROBSIZE;
                        assert(add(old_Tables[j][i].arr[k], false));
                    }
        delete[] old_Tables[0];
        delete[] old_Tables[1];
        for (int i=0; i<t0.lock_size; ++i){
            t0.lock_arr[i].unlock();
        }
        for (int i=0; i<t0.lock_size; ++i){
            t1.lock_arr[i].unlock();
        }//unlocked all
        resize_lock.unlock();


        return true;
    }
    bool relocate(int i, int posi, bool block = true){
#ifdef ONE_THREAD_DEBUG
        (block)?printf("relocate blocked\n"): printf("relocate nonblocked\n");
#endif

        int vers = version;
        Table *tables[2] {&t0, &t1};
        int j = 1 - i;
        for(int k = 0; k<relocate_LIMIT; ++k){
            PositionPack h;
            Table_el_type y;
            for(;;) {//catch y
                if (block) tables[i]->lockn(posi);
                if (vers<version || tables[i]->arr[posi].capacity <= TRESHOLD){
                    if (block) tables[i]->unlockn(posi);
                    return true;
                }
                y = tables[i]->arr[posi].back();
                h = pospack(y);
                if (posi!=h.pos[i])
                    printf("catched y is strange, y = %d", y);
                assert(posi == h.pos[i]);
                if (block) tables[i]->unlockn(posi);
                acquire(h, block);

                if (vers<version){
                    release(h, block);
                    return true;
                }
                if (tables[i]->arr[posi].capacity <= TRESHOLD){
                    release(h, block);
                    return true;
                }
                if (y == tables[i]->arr[posi].back()){
                    break;
                } else {
                    release(h, block);//someone cheat our "y" while we've slept
                }
            }//y and h are correct
            Probe &jProbe = tables[j]->arr[h.pos[j]], &iProbe = tables[i]->arr[h.pos[i]];
            assert(iProbe.pop() == y);
            if (jProbe.capacity < TRESHOLD){
                jProbe.push(y);
                release(h, block);
                return true;
            } else if (jProbe.capacity < PROBSIZE){
                jProbe.push(y);
                i = 1-i;
                posi = h.pos[i];
                j = 1-i;
                release(h, block);
                continue;
            } else {
                iProbe.push(y);
                release(h, block);
                continue;
                return false;//it might be work - another item could have another variant;
            }
        }
        
        
        return false;
    }
    void print(){
        for (int i=0; i<size; ++i){
            printf("%3d) t0 ", i);
            for (int j = 0; j<PROBSIZE; ++j)
                printf("%3d ", t0.arr[i].arr[j]);
            printf("b%d e%d c%d   t1 ", t0.arr[i].begin, t0.arr[i].end, t0.arr[i].capacity);
            for (int j = 0; j<PROBSIZE; ++j)
                printf("%3d ", t1.arr[i].arr[j]);
            printf("b%d e%d c%d\n", t1.arr[i].begin, t1.arr[i].end, t1.arr[i].capacity);
        }
    }
};

CuckooHash Cuc{8, 8};

void foo(int i, CuckooHash * Cuc){
    int k = i;
    //i = 0;
    bool r = true;
    for(int j = 1000*i + 1; j<1000*i + 100; ++j){
        //printf("I'm %2d add   %3d\n", k, j);
        r = r && Cuc->add(j);
        //printf("I'm %2d added %3d res %d\n", k, j, r);
        //Cuc.print();
    }
    /*for(int j = 1000*i + 1; j<1000*i + 100; ++j){
        //printf("I'm %2d erase   %3d\n", k, j);
        r = r && Cuc->erase(j);
        //printf("I'm %2d erase %3d res %d\n", k, j, r);
        //Cuc.print();
    }*/
    return;
}

class Timer {
public:
    Timer() {
        begin = std::chrono::steady_clock::now();
    }

    ~Timer() {
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        //std::cout << "Time is " << elapsed_ms.count()*0.1e-8 << " s\n";
        printf("%9d\n", elapsed_ms.count());
        //std::cout << elapsed_ms.count()<<"\n";
    }

private:
    std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1, 1000000000>>>
            begin;

};


int main() {
    std::cout << "Hello, World!" << std::endl;
    std::mutex mut;

    Table x(5, 5, 11);
    Probe& p = x.arr[1];
    p.contains(99);
    p.push(1);
    p.print();

    p.push(2);
    p.print();

    p.push(3);
    p.print();

    p.push(4);
    p.print();
    std::cout<<p.contains(7)<<'\n'<<p.contains(2)<<'\n';
    p.erase(2);
    p.print();
    p.pop();
    p.print();


    p.push(5);
    p.print();
    p.pop();
    p.print();
    p.pop();
    p.print();

    x.arr[1].push(6);
    x.arr[1].print();

    std::cout<<Cuc.contains(7)<<'\n';
    std::cout<<Cuc.contains(1)<<'\n';

    for (Table_el_type i = 1; i<50; ++i){
        std::cout<<"add "<<i<<" "<<Cuc.add(i)<<'\n';
        Cuc.print();
    }
    for (Table_el_type i = 1; i<50; ++i){
        std::cout<<"erase "<<i<<" "<<Cuc.erase(i)<<'\n';
        Cuc.print();
    }

    for (int j = 1; j<=64; j*=2) {
        int num_threads = j;
        CuckooHash Cuc(512, 128, false);
        std::vector<std::thread> th(num_threads);
        Timer t;
        for (int i = 0; i < num_threads; ++i) {
            th[i] = std::thread(foo, i, &Cuc);
        }
        for (int i = 0; i < num_threads; ++i) {
            th[i].join();
        }
        printf("%3d ", num_threads);
    }


    return 0;
}
