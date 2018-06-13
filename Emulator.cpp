#include "Emulator.h"
#include "Command.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

union Union
{
    char c;
    int i;
    float f;
    Word32 w;
};

void Emulator::showRegester(Word32 id)
{
    cout << "寄存器" << id << ": " << memory.getReg(id) << endl;
}

void Emulator::showByte(Word32 id)
{
    Word32 t = memory.getByte(id); 
    cout << "字节" << id << ": " << t << " (";
    for (int i = 7; i >= 0; i--)
        cout << ((t >> i) & 1);
    cout << ")" << endl;
}

void Emulator::showWord(Word32 id)
{
    Word32 t = memory.getWord(id); 
    cout << "字" << id << ": " << t << " (";
    for (int i = 31; i >= 0; i--)
        cout << ((t >> i) & 1);
    cout << ")" << endl;
}

void Emulator::showCommand(Word32 id)
{
    cout << "指令" << setw(10) << id << ": " << setw(0);
    cout << Translator::c2s(memory.getWord(id)) << endl;
}

void Emulator::writeMemory(Word32 id, Command cmd)
{
    memory.setWord(id, cmd);
}

void Emulator::loadMIPS(Word32 id, string filename)
{
    CommandLoader cl;
    auto prog = cl.load(filename);
    for(auto &x : prog)
    {
        memory.setWord(id, x);
        id += 4;
    }
}

bool Emulator::run(bool show)
{
    Word32 id = memory.getPC();
    Command s = memory.getWord(id);
    Word32 val = Translator::getType(s);

    /*
    老版本退出
    if (s == 0xFFFFFFFFu)
        return false;
    */
    if (show)
        showCommand(id);
    memory.step();

    // 系统调用
    if (s == 0x0000000c)
    {
        Word32 action = memory.getReg(2);   // $v0
        Union t;

        memory.getReg(4);
        if (action == 1)
        {
            t.w = memory.getReg(4);
            printf("%d", t.i);
        }
        else if (action == 2)
        {
            t.w = memory.getReg(4);
            printf("%f", t.f);
        }
        else if (action == 4)
        {
            t.w = memory.getReg(4);
            printf("%s", memory.getString(t.w));
        }
        else if (action == 5)
        {
            scanf("%d", &t.i);
            memory.setReg(4, t.w);
        }
        else if (action == 6)
        {
            scanf("%f", &t.f);
            memory.setReg(4, t.w);
        }
        else if (action == 8)
        {
            t.w = memory.getReg(4);
            scanf("%s", memory.getString(t.w));
        }
        else if (action == 10)
        {
            return false;
        }
        else if (action == 11)
        {
            t.w = memory.getReg(4);
            printf("%c", t.c);
        }
        else if (action == 12)
        {
            t.w = 0;
            scanf("%c", &t.c);
            memory.setReg(4, t.w);
        }
    }
    // I 类型
    // 存取
    else if(val == LW)  // lw
        memory.setReg(Translator::getR2(s), memory.getWord(memory.getReg(Translator::getR1(s)) + Translator::getIm(s)));
    else if(val == SW)  // sw
        memory.setWord(memory.getReg(Translator::getR1(s)) + Translator::getIm(s), memory.getReg(Translator::getR2(s)));
    else if(val == LUI) // lui
        memory.setReg(Translator::getR2(s), Translator::getIm(s) << 16);
    // 跳跃
    else if(val == BEQ)  // beq
    {
        if(memory.getReg(Translator::getR1(s)) == memory.getReg(Translator::getR2(s)))
            memory.setPC(memory.getPC() + (((int)(short)Translator::getIm(s)) << 2));
    }
    // 运算
    else if(val == ORI) // ori
        memory.setReg(Translator::getR2(s), memory.getReg(Translator::getR1(s)) | Translator::getIm(s));
    else if(val == ADDI) // addi
        memory.setReg(Translator::getR2(s), memory.getReg(Translator::getR1(s)) + Translator::getIm(s));
    // R类型
    else if(val == R)
    {
        Word32 type = Translator::getOp(s);
        if(type == ADD)
            memory.setReg(Translator::getR3(s), memory.getReg(Translator::getR1(s)) + memory.getReg(Translator::getR2(s)));
        else if(type == SUB)
            memory.setReg(Translator::getR3(s), memory.getReg(Translator::getR1(s)) - memory.getReg(Translator::getR2(s)));
        else if(type == SLT)
            memory.setReg(Translator::getR3(s), memory.getReg(Translator::getR1(s)) < memory.getReg(Translator::getR2(s)));
    }
    // J类型
    else if(val == J)  // j
        memory.setPC((memory.getPC() & 0xF0000000) | (Translator::getVal(s, 25, 0) << 2) );

    return true;
}

void Emulator::mainLoop()
{
    string cmd, c, tmp;
    Word32 id, t;

    while(true)
    {
        cout << ">>> ";
        getline (cin, tmp);
        stringstream ss(tmp);

        ss >> cmd;
        
        if (cmd == "R")
        {
            if (!ss.good())
            {
                cout << "错误的指令。" << endl;
                continue;
            }
            ss >> c;
        } 
        else if(cmd != "Q" && cmd != "H" && cmd != "T")
        {
            if (!ss.good())
            {
                cout << "错误的指令。" << endl;
                continue;
            }
            ss >> id;
        }

        if(cmd == "R")
        {
            if (Translator::notID(c))
            {
                cout << "错误的指令。" << endl;
                continue;
            }
            id = Translator::getID(c);
            showRegester(id);
        }
        else if(cmd == "D")
        {
            if((id & 3) == 0)  // 对齐
                showWord(id);
            showByte(id);
        }
        else if(cmd == "U")
        {
            if(id & 3) 
                cout << "地址无意义，请检查是否对齐。" << endl; 
            else
                showCommand(id);
        }
        else if(cmd == "A")
        {
            getline(ss, c);
            if(id & 3) 
                cout << "地址无意义，请检查是否对齐。" << endl; 
            else 
            {
                auto cmds = Translator::s2c(c);
                for (auto &t : cmds)
                {
                    writeMemory(id, t);
                    id += 4;
                }
            }
        }
        else if(cmd == "W")
        {
            ss >> t;
            if (ss.good())
            {
                cout << "错误的指令。" << endl;
                continue;
            }   

            if(id & 3) 
                cout << "地址无意义，请检查是否对齐。" << endl; 
            else 
                writeMemory(id, t);
        }
        else if(cmd == "L")
        {
            ss >> c;
            if (ss.good())
            {
                cout << "错误的指令。" << endl;
                continue;
            }

            if(id & 3) 
                cout << "地址无意义，请检查是否对齐。" << endl; 
            else
                loadMIPS(id, c);
        }
        else if(cmd == "T")
        {
            if (ss.good())
            {
                ss >> id;
                if(id & 3)
                {
                    cout << "地址无意义，请检查是否对齐。" << endl;
                    continue;
                }
                memory.setPC(id);
            }
            
            run(1);
        }
        else if(cmd == "G")
        {
            if(id & 3) 
                cout << "地址无意义，请检查是否对齐。" << endl; 
            else
            {
                memory.setPC(id);
                while (run(0));
                getline (cin, tmp);
            }
        }
        else if(cmd == "Q")
        {
            break;
        }
        else if(cmd == "H")
        {
            cout << "\
======================================================\n\
R <寄存器号>  看寄存器\n\
D <内存号>  看内存\n\
U <内存号>  看指令\n\
W <内存号> <内容>  写入32位数据\n\
A <内存号> <指令>  写入指令（注意是字符指令不是二进制）\n\
L <内存号> <文件名>  从文件加载指令序列\n\
T <内存号>  执行指令\n\
G <内存号>  执行指令直到遇到结束符0xFFFFFFFFu\n\
H  查看本帮助\n\
Q  退出\n\
======================================================\n" << endl;
        }
        else
        {
            cout << "错误的指令。" << endl;
        }
    }
}

void Emulator::start()
{
    mainLoop();
}

Emulator::Emulator() :
    memory()
{
    Translator::init();
}