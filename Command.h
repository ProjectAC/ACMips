#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "definitions.h"
#include <map>

enum Type
{
    // R
    R = 0x00,
    // J
    J = 0x03,
    // I
    LW = 0x23,
    SW = 0x2B,
    BEQ = 0x04,
    LUI = 0x0F,
    ORI = 0x0D,
    ADDI = 0x08
};

// 00000 +
// 00001 -
// 00010 *
// 00011 /
// 00100 f+
// 00101 f-
// 00110 f*
// 00111 f/
// 01000 &
// 01001 |
// 01010 ~
// 01011 ^
// 01100 <
// 01101 >
// 01110 ==
// 01111 %
// 10000 <<
// 10001 >>
// 10010 <=
// 10011 >=

enum Operation
{
    ADD = 0x00,
    SUB = 0x01,
    SLT = 0x0C,
    AND = 0x08,
    OR  = 0x09
};

class Translator
{
    static std::string name[32];
    static std::map<std::string, Word32> value; 

    static bool notIm(std::string s);
    static bool notOf(std::string s);
    static Word32 getIm(std::string s);
    static std::pair<Word32, Word32> getOf(std::string s);
    static std::string removeLast(std::string s);

public:
    static bool notID(std::string s);
    static Word32 getID(std::string s);
    static void setVal(Word32 &a, int r, int l, Word32 val);
    static Word32 getVal(Word32 a, int r, int l);

    static std::vector<Command> s2c(std::string s);
    static std::string c2s(Command c);
    static Type getType(Command c);
    static Type getR1(Command c);
    static Type getR2(Command c);
    static Type getR3(Command c);
    static Type getIm(Command c);
    static Type getOp(Command c);
    static void display(Command c);

    static void init();
};

class CommandLoader
{
    std::ifstream fin;

    std::vector<Command> nextCommand();

public:

    std::vector<Command> load(std::string filename);
};