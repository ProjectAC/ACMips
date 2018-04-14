#pragma once

#include "definitions.h"

class Memory
{
    unsigned char *mem;
    Word32 *reg;
    Word32 pc, size;
    
public:
    
    // 创建内存空间
    Memory(Word32 size = 0x00100000);
    
    Word32 getReg(Word32 id);
    void setReg(Word32 id, Word32 w);
    Word32 getWord(Word32 id);
    void setWord(Word32 id, Word32 w);
    Word32 getByte(Word32 id);
    void setByte(Word32 id, Word32 w);
    Word32 getPC();
    void setPC(Word32 id);
    void step();
};