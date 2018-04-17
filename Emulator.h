#pragma once

#include <string>
#include "Memory.h"

class Emulator
{
    Memory memory;

    void mainLoop();
    
    void showRegester(Word32 id);
    void showByte(Word32 id);
    void showWord(Word32 id);
    void showCommand(Word32 id);
    void writeMemory(Word32 id, Command cmd);
    void loadMIPS(Word32 id, std::string filename);
    bool run(bool show);

public:

    void start();
    Emulator();
};