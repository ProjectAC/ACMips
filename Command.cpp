#include "Command.h"
#include <cstring>
#include <iostream>
#include <sstream>

#define MAXN 110

using namespace std;


string Translator::name[32];
map<std::string, Word32> Translator::value; 

void Translator::init()
{
    name[ 0] = "$zero";
    name[ 1] = "$at";
    name[ 2] = "$v0";
    name[ 3] = "$v1";
    name[ 4] = "$a0";
    name[ 5] = "$a1";
    name[ 6] = "$a2";
    name[ 7] = "$a3";
    name[ 8] = "$t0";
    name[ 9] = "$t1";
    name[10] = "$t2";
    name[11] = "$t3";
    name[12] = "$t4";
    name[13] = "$t5";
    name[14] = "$t6";
    name[15] = "$t7";
    name[16] = "$s0";
    name[17] = "$s1";
    name[18] = "$s2";
    name[19] = "$s3";
    name[20] = "$s4";
    name[21] = "$s5";
    name[22] = "$s6";
    name[23] = "$s7";
    name[24] = "$t8";
    name[25] = "$t9";
    name[26] = "$k0";
    name[27] = "$k1";
    name[28] = "$gp";
    name[29] = "$sp";
    name[30] = "$fp";
    name[31] = "$ra";

    for (int i = 0; i < 32; i++)
        value[name[i]] = i;
}

void Translator::setVal(Word32 &a, int r, int l, Word32 val)
{
    val <<= l;
    Word32 mask = ((r == 31 ? 0 : 1u << (r + 1)) - 1) - ((1u << l) - 1);
    val &= mask;
    (a &= ~mask) |= val;
}

string Translator::removeLast(string s)
{    
    if (s[s.length() - 1] != ',')
        return "";
    return s.substr(0, s.length()-1);
}

Word32 Translator::getVal(Word32 a, int r, int l)
{
    return (a & (((r == 31 ? 0 : 1u << (r + 1))) - 1)) >> l; 
}

bool Translator::notID(string s)
{
    if(s.length() < 1)
        return true;
    if (s[0] != '$')
        return true;
    if (value.find(s) != value.end())
        return false;
    if (s.length() > 3)
        return true;
    if (s[1] < '0' || s[1] > '9' || s.length() == 3 && (s[2] < '0' || s[2] > '9'))
        return true;

    Word32 t;
    sscanf(s.c_str() + 1, "%u", &t);
    return t < 0 || t > 31;
}

bool Translator::notIm(string s)
{
    if(s.length() < 1)
        return true;
    if(s[0] == '-')
        if (s.length() < 2) return true;
        else s = s.substr(1);
    for(auto &x : s)
        if(x < '0' || x > '9')
            return true;
    return false;
}

bool Translator::notOf(string s)
{
    int pos = s.find('(');
    if (pos == string::npos)
        return true;
    return notID(s.substr(pos + 1, s.length() - pos - 2)) || notIm(s.substr(0, pos));
}

Word32 Translator::getID(string s)
{
    if (value.find(s) != value.end())
        return value[s];

    Word32 res;
    sscanf(s.c_str() + 1, "%u", &res);
    return res;
}

Word32 Translator::getIm(string s)
{
    Word32 res;
    sscanf(s.c_str(), "%u", &res);
    return res;
}

pair<Word32, Word32> Translator::getOf(string s)
{
    int pos = s.find('(');
    return make_pair(getID(s.substr(pos + 1, s.length() - pos - 2)), getIm(s.substr(0, pos)));
}

vector<Command> Translator::s2c(std::string s)
{
    vector<Command> ret;
    Command res;
    string t;
    stringstream flow(s);
    string ss, st, sd;

    flow >> t;

    // I类型
    // 存取
    if(t == "lw")
    {
        flow >> st >> sd;
        if ((st = removeLast(st)) == "" || notID(st) || notOf(sd))
            return ret;

        auto x = getOf(sd);
        res = 0; 
        setVal(res, 31, 26, LW);
        setVal(res, 25, 21, x.first);
        setVal(res, 20, 16, getID(st));
        setVal(res, 15, 0, x.second);
        ret.push_back(res);
    }
    else if(t == "sw")
    {
        flow >> st >> sd;
        if ((st = removeLast(st)) == "" || notID(st) || notOf(sd))
            return ret;

        auto x = getOf(sd); 
        res = 0;
        setVal(res, 31, 26, SW);
        setVal(res, 25, 21, x.first);
        setVal(res, 20, 16, getID(st));
        setVal(res, 15, 0, x.second);
        ret.push_back(res);
    }
    else if(t == "li")
    {
        flow >> st >> sd;
        if ((st = removeLast(st)) == "" || notID(st) || notIm(sd))
            return ret;

        res = 0;
        // lui
        setVal(res, 31, 26, LUI);
        setVal(res, 25, 21, 0);
        setVal(res, 20, 16, getID(st));
        setVal(res, 15, 0, getIm(sd) >> 16);
        ret.push_back(res);
        // ori
        res = 0;
        setVal(res, 31, 26, ORI);
        setVal(res, 25, 21, getID(st));
        setVal(res, 20, 16, getID(st));
        setVal(res, 15, 0, getIm(sd) & 0xFFFF);
        ret.push_back(res);
    }
    // 跳跃
    else if(t == "beq")
    {
        flow >> ss >> st >> sd;
        if ((ss = removeLast(ss)) == "" || (st = removeLast(st)) == "" || notID(ss) || notID(st) || notIm(sd))
            return ret;

        res = 0;
        setVal(res, 31, 26, BEQ);
        setVal(res, 25, 21, getID(ss));
        setVal(res, 20, 16, getID(st));
        setVal(res, 15, 0, getIm(sd));
        ret.push_back(res);
    }
    // 运算
    else if(t == "addi")
    {
        flow >> st >> ss >> sd;
        if ((st = removeLast(st)) == "" || (ss = removeLast(ss)) == "" || notID(ss) || notID(st) || notIm(sd))
            return ret;
        
        res = 0;
        setVal(res, 31, 26, ADDI);
        setVal(res, 25, 21, getID(ss));
        setVal(res, 20, 16, getID(st));
        setVal(res, 15, 0, getIm(sd) & 0xFFFF);
        ret.push_back(res);
    }
    // R类型
    else if(
        t == "add" ||
        t == "sub" ||
        t == "slt")
    {
        flow >> sd >> ss >> st;

        if ((sd = removeLast(sd)) == "" || (ss = removeLast(ss)) == "" || notID(ss) || notID(st) || notID(sd))
            return ret;

        res = 0;
        setVal(res, 31, 26, R);
        setVal(res, 25, 21, getID(ss));
        setVal(res, 20, 16, getID(st));
        setVal(res, 15, 11, getID(sd));
        
        setVal(res, 10, 6, 0);
        if(t == "add")
            setVal(res, 5, 0, ADD);
        else if(t == "sub")
            setVal(res, 5, 0, SUB);
        else if(t == "slt")
            setVal(res, 5, 0, SLT);

        ret.push_back(res);
    }
    // J类型
    else if(t == "j")
    {
        flow >> ss;
        if (notIm(ss))
            return ret;

        res = 0;
        setVal(res, 31, 26, J);
        setVal(res, 25, 0, getIm(ss));
        ret.push_back(res);
    }
    // 系统调用
    else if(t == "syscall")
    {
        res = 0x0000000c;
        ret.push_back(res);
    }

    /*
    else if () {
    
    }
    else if () {
    
    }
    else if () {
    
    }
    */
    
    if(flow.good() && (flow >> t, t[0] != ';' && t != ""))
        ret.clear();
    
    return ret;
}

string Translator::c2s(Command s)
{
    // 系统调用
    if(s == 0x0000000c)
        return "syscall";

    ostringstream res;
    Word32 val = getVal(s, 31, 26);

    // I类型
    // 存取
    if(val == 0x23)  // lw
    {
        res << "lw " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0) << "(" << name[getVal(s, 25, 21)] << ")";
    }
    else if(val == 0x2B)  // sw
    {
        res << "sw " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0) << "(" << name[getVal(s, 25, 21)] << ")";
    }
    else if(val == LUI) // lui
    {
        res << "lui " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0);
    }
    // 跳跃
    else if(val == 0x04)  // beq
    {
        res << "beq " << name[getVal(s, 25, 21)] << ", " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0);
    }
    else if(val == ORI) // ori
    {
        res << "ori " << name[getVal(s, 20, 16)] << ", " << name[getVal(s, 25, 21)] << ", " << getVal(s, 15, 0);
    }
    // 运算
    else if(val == ADDI) // addi
    {
        res << "addi " << name[getVal(s, 20, 16)] << ", " << name[getVal(s, 25, 21)] << ", " << getVal(s, 15, 0);
    }
    // R类型
    else if(val == 0x00)
    {
        Word32 type = getVal(s, 5, 0);
        if(type == ADD)
            res << "add";
        else if(type == SUB)
            res << "sub";
        else if(type == SLT)
            res << "slt";

        res << " " << name[getVal(s, 15, 11)] << ", " << name[getVal(s, 25, 21)] << ", " << name[getVal(s, 20, 16)];
    }
    // J类型
    else if(val == 0x03)
    {
        res << "j " << getVal(s, 25, 0);
    }
    
    return res.str();
}

Type Translator::getType(Command c)
{
    return (Type)getVal(c, 31, 26);
}

Type Translator::getR1(Command c)
{
    return (Type)getVal(c, 25, 21);
}

Type Translator::getR2(Command c)
{
    return (Type)getVal(c, 20, 16);
}

Type Translator::getR3(Command c)
{
    return (Type)getVal(c, 15, 11);
}

Type Translator::getIm(Command c)
{
    return (Type)getVal(c, 15, 0);
}

Type Translator::getOp(Command c)
{
    return (Type)getVal(c, 5, 0);
}

void Translator::display(Command word)
{
    for(Word32 i = 1u << 31; i; i>>=1)
        cout << ((word & i) != 0);
    cout << endl;
}

vector<Command> CommandLoader::nextCommand()
{
    if(!fin.good())
        return vector<Command>();
    
    string s;
    do
    {
        getline(fin, s);
    } while(fin.good() && (s.length() == 0 || s[0] == ';'));

    if (s.length() == 0 || s[0] == ';')
        return vector<Command>();
    return Translator::s2c(s);
}

vector<Command> CommandLoader::load(string filename)
{
    vector<Command> res, ret;

    fin.open(filename.c_str(), ifstream::in);
    if(!fin.good())
    {
        cout << "载入失败：打开文件" << filename << "失败。" << endl; 
        return ret;
    }

    int cnt = 1;
    while(fin.good() && (res = nextCommand()).size())
    {
        cnt++;
        for (auto &t : res)
            ret.push_back(t);
    }

    if(fin.eof())
        cout << "载入成功。" << endl;
    else
        cout << dec << "载入失败：在第" << cnt << "行发现错误。" << endl, ret.clear();
    fin.close();

    //ret.push_back(0xFFFFFFFFu);

    return ret;
}