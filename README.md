# HustPC_Asm

功能：解释以类Mips的HPC指令集编写的程序。

### 模块

- Translator类：
指令翻译类，可以完成机器码和汇编码的双向转换  

- CommandLoader类：
加载汇编代码  

- Emulator类：
模拟器类，可以直接解释执行  

- Memory类：
内存类，模拟内存条，用于Emulator  

### 一些详细说明

目前已经支持的指令在Command.h里的两个Enum里可以看到。  
编写好的新的指令也请加到那个里面去（R类指令的编码已经塞在里面了）。  
已经支持伪指令的汇编了。  
  
Memory类的模拟内存：字节大小8位，字长32位，小端存储且对齐。  
默认的内存大小为2^20B。  

### 使用

一个支持更多指令的模拟器。  
  
3种基本指令：  
I类，R类，和J类  
  
伪指令已经在添加。目前已经有加入了li（赋值）。后期还会根据实际编程需求随时添加。  
  
使用方法：  
字符界面，运行后显示“>>>”后可以输入。  
```
R <寄存器号>  看寄存器
D <内存号>  看内存
U <内存号>  看指令
W <内存号> <内容>  写入32位数据
A <内存号> <指令>  写入指令（注意是字符指令不是二进制）
L <内存号> <文件名>  从文件加载指令序列
T <内存号>  执行指令
G <内存号>  执行指令直到遇到结束符0xFFFFFFFFu
H  查看帮助
Q  退出
```

一个测试用例：
```
W 0 1
W 4 2 
A 8 lw $1 0($31)
L 12 test.aca
G 8
D 8
```

它通过L命令加载了test.aca（A Complex Assembler，可以逐行或连续执行。

一个更好的例子是直接加载fibo.aca运行。这个程序的功能更为全面。

### 更新

__增加了syscall的功能，可以通过syscall进行IO和退出程序__  
__修正了lw和sw指令中寄存器顺序弄反的问题__  
__增加了斐波那契程序fibo.aca，可以测试循环程序__
更新了指令集   
模拟器加入了可以查看内存二进制码的功能（原来只能看10进制并不方便）  
现在可以通过寄存器名字（而不只能寄存器号）来访问一个寄存器了  
进一步增加程序的稳定性，修复了会不停输出“错误的指令”的bug  
对错误指令的审核更加严格，尽可能避免任何不符合格式的指令被执行  
现在Mips汇编指令中必须要加入逗号，更加标准了  