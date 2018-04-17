#include "Memory.h"
#include <iostream>

using namespace std;

char* Memory::getString(Word32 id)
{
    return (char *)(mem + id);
}

Word32 Memory::getReg(Word32 id)
{
    return reg[id];
}

void Memory::setReg(Word32 id, Word32 w)
{
    if(0 <= id && id < 32) 
        reg[id] = w;
}

Word32 Memory::getWord(Word32 id)
{
    if(0 <= id && id + 4 <= size)
        return (getByte(id + 3) << 24) | (getByte(id + 2) << 16) | (getByte(id + 1) << 8) | getByte(id);
    return 0;
}

void Memory::setWord(Word32 id, Word32 w)
{
    if(0 <= id && id + 4 <= size)
        for(int i = 0; i < 4; i++)
            setByte(id + i, w), w >>= 8;
}

Word32 Memory::getByte(Word32 id)
{
    if(0 <= id && id < size)
        return mem[id];
     return 0;
}

void Memory::setByte(Word32 id, Word32 w)
{
    if(0 <= id && id < size)
        mem[id] = w & 0xFF;
}

Word32 Memory::getPC()
{
    return pc;
}

void Memory::setPC(Word32 id)
{
    pc = id;
}

void Memory::step()
{
    pc += 4;
}

Memory::Memory(Word32 size) : 
    size(size),
    pc(0)
{
    mem = new unsigned char[size];
    reg = new Word32[size];
}