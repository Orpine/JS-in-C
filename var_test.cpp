
#include "var.h"


int main(){
   Var *a=new Var(1);
   Var *b=new Var(2);
   Var *c=new Var(1.0);
   std::cout<<a->mathOp(b,TK_GREATER)->getBool()<<endl;
   std::cout<<a->mathOp(c,TK_TYPEEQUAL)->getBool()<<endl;
   std::cout<<a->mathOp(c,TK_EQUAL)->getBool()<<endl;
   std::cout<<a->equals(c)<<endl;
   Var* array=new Var("",VAR_ARRAY);
   for(int i=0;i<10;i++){
       array->setAtIndex(i,new Var(i));
   }
   std::cout<<array->getArrayLength()<<endl;
   Var *obj=new Var("",VAR_OBJECT);
   auto obj_a=obj->findChildOrCreate("a",VAR_INTEGER);
   auto obj_b=obj->addChild("b",new Var(1));
   
   obj_b->var->addChilds(obj);
   std::cout<<obj_b->var->checkChild("a");
   
   return 0;
}