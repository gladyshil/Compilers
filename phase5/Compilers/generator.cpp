# include <cstdlib>
# include <iostream>
#include "Type.h"
#include "Tree.h"
#include "Symbol.h"
#include "checker.h"


using namespace std; 

unsigned max_bytes = 0;

static ostream &operator <<(ostream &ostr, Expression *expr){
    expr->operand(ostr);
    return ostr;
}

void Block::generate(){
    for(auto stmt:_stmts){
        stmt->generate();
    }
}

/*void Function::generate(){
    int offsetee = 0;
    Parameters *paramees = _id->type().parameters();
    int localOffsetee = 0;
    Symbols localSymbols = _body->declarations()->symbols();
    for(unsigned i = 0; i < paramees->types.size(); i++){
            Type paramTypee = paramees->types[i];
            offsetee += paramTypee.size();
            localSymbols[i]->_offset = offsetee;

    }


    
    for(unsigned i = paramees->types.size(); i < localSymbols.size(); i++)
    {
        Type localType = localSymbols[i]->type();
        localOffsetee -= localType.size();
        localSymbols[i]->_offset = localOffsetee;
    }    

    cout <<".globl "<< _id->name() <<endl;
     cout<< _id->name() <<": pushl   %ebp" <<endl;
     cout << "movl  %esp, %ebp"<<endl;
     cout << "subl  $" << offsetee << ", %esp" << endl;

     _body->generate();

    cout<< "movl %ebp, %esp " << endl;
     cout << "popl %ebp" << endl;
     cout << "ret" << endl; 
    
}*/

void Function::generate(){
    int offset = 0;
    int localOffsetee = 0;
    int paramsOffset = 0;
    unsigned nums_params = _id->type().parameters()->types.size();
    const Symbols &symbols = _body->declarations()->symbols();
    for(unsigned i = 0; i < symbols.size(); i++)
    {
        if(i < nums_params){
            symbols[i]->_offset = 8+4*i; 
            paramsOffset = symbols[i]->_offset;
            offset += paramsOffset;
            //cout << "parameter offset: " << offset << endl; 
            
        }
        else{
            symbols[i]->_offset =  localOffsetee-symbols[i]->type().size();
            localOffsetee += symbols[i]->_offset;
            //offset += localOffsetee;
           //cout << "local offset: " << localOffsetee << endl;
        }
    }

    offset = offset - localOffsetee;

    //cout << "offset after params and local: " << offset << endl;
    
   if(nums_params > max_bytes)

        max_bytes = nums_params*4;

    offset += max_bytes;
    //cout << "offset after adding max_bytes: " << offset << endl;

    while (offset % 16 != 8)
      {
          offset++;
        //  cout << "offset in while: " << offset << endl;
      }  

    
    //offset += offset;


    cout <<".globl "<< _id->name() <<endl;
     cout<< _id->name() <<": pushl   %ebp" <<endl;
     cout << "movl  %esp, %ebp"<<endl;
     cout << "subl  $" << offset << ", %esp" << endl;
     offset = 0;

     _body->generate();

    cout<< "movl %ebp, %esp " << endl;
     cout << "popl %ebp" << endl;
     cout << "ret" << endl; 
}


void Statement::generate(){
    cout << "statement generate has been called" << endl;
}

void Assignment::generate(){
  //  cout << "assignment generate has been called" << endl;
  cout << "movl " << _right << "," << _left << endl;
}

void Call::generate(){
   // cout << "call generate has been called" << endl;

    Expressions temp_args = _args;
   int counter = 0;
   unsigned max_args = temp_args.size();

   if(max_bytes < max_args)
      max_bytes = max_args; 

   for(unsigned i = 0; i < _args.size(); i++)
   {
       cout << "movl   " <<  temp_args[i] << ", "<< "%eax "<< endl;
       cout << "movl   " << " %eax, " << counter << "(%esp)" << endl;
       counter += temp_args[i]->type().size();
   }   

   cout << "call " << _id->name() << endl;

   
}


void generateGlobals(Scope * topLevel){
    Symbols globalSymbols = topLevel->symbols();
    for(unsigned i = 0; i < globalSymbols.size(); i++)
    {
        if(!globalSymbols[i]->type().isFunction())
        {
           cout<<".comm    " << globalSymbols[i]->name() << "," << globalSymbols[i]->type().size() << endl;
        }
    }
}
void Integer::operand(ostream &ostr) const{
    ostr << "$" << _value;
}
void Identifier::operand(ostream &ostr) const{
    if(_symbol->_offset == 0)
        ostr << _symbol->name();
    else
        ostr << _symbol->_offset<< "(%ebp) ";
}
void Expression::operand(ostream &ostr) const{

}
