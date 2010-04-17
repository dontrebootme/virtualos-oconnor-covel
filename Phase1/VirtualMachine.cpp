/******************************************************************
mapFuncPtr2.cpp
K Zemoudeh
4/5/10

 THIS HAS BEEN CHANGED
 * 
This program improves on mapFuncPtr.cpp by incorporating Exception
Handling. The original program fails on an invalid opcode; this
program doesn't.
*******************************************************************/

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

class NullPointerException: public runtime_error {
public:
	NullPointerException(): runtime_error("Null Pointer!") { }
};

void VirtualMachine::load()
{
}
void VirtualMachine::loadi()
{
}
void VirtualMachine::store()
{
}
void VirtualMachine::add()
{
}
void VirtualMachine::addi()
{
}
void VirtualMachine::addc()
{
}
void VirtualMachine::addci()
{
}
void VirtualMachine::sub()
{
}
void VirtualMachine::subi()
{
}
void VirtualMachine::subc()
{
}
void VirtualMachine::subci()
{
}
void VirtualMachine::and()
{
}
void VirtualMachine::andi()
{
}
void VirtualMachine::xor()
{
}
void VirtualMachine::xori()
{
}
void VirtualMachine::compl()
{
}
void VirtualMachine::shl()
{
}
void VirtualMachine::shla()
{
}
void VirtualMachine::shr()
{
}
void VirtualMachine::shra()
{
}
void VirtualMachine::compr()
{
}
void VirtualMachine::compri()
{
}
void VirtualMachine::getstat()
{
}
void VirtualMachine::putstat()
{
}
void VirtualMachine::jump()
{
}
void VirtualMachine::jumpl()
{
}
void VirtualMachine::jumpe()
{
}
void VirtualMachine::jumpg()
{
}
void VirtualMachine::call()
{
}
void VirtualMachine::return()
{
}
void VirtualMachine::read()
{
}
void VirtualMachine::write()
{
}
void VirtualMachine::halt()
{
}
void VirtualMachine::noop()
{
}


