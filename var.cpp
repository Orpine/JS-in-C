#ifndef _var_cpp_
#define _var_cpp_
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include "./js_in_c_lex/Lex.hpp"
class Var;
class VarLink;
typedef void (*Callback)(Var *var, void *data);
#define JS_IN_C_RETURN_VAR "return"
enum VAR_TYPES {
	VAR_UNDEFINED,
	VAR_NULL,
	VAR_BOOLEAN,
	VAR_STRING,
	VAR_DOUBLE,
	VAR_INTEGER,
	VAR_FUNCTION,
	VAR_OBJECT,
	VAR_ARRAY
	


  };

class Var{
protected:
	int refNum; //
	std::string stringData;//
	int intData; //int and bool
	double doubleData;
	int type;
	bool native;//is native function
	Callback callback;// callback for native functions
	void *callbackData;//

	void init();



public:
	VarLink *firstChild;
	VarLink *lastChild;



	Var();//create undefined
	Var(const std::string &varData, int varType=VAR_STRING);
	Var(double varData);
	Var(int varData);
	Var(bool varData);
	~Var();

	bool isInt(){return(type==VAR_INTEGER);}
	bool isDouble(){return(type==VAR_DOUBLE);}
	bool isString(){return(type==VAR_STRING);}
	bool isNumber(){return(type==VAR_INTEGER||type==VAR_DOUBLE);}
	bool isFunction(){return(type==VAR_FUNCTION);}
	bool isObject(){return(type==VAR_OBJECT);}
	bool isArray(){return(type==VAR_ARRAY);}
	bool isBoolean(){return(type==VAR_BOOLEAN);}
	bool isNative(){return(native);}
	bool isUndefined(){return(type==VAR_UNDEFINED);}
	bool isNull(){return(type==VAR_NULL);}
	bool isBasic(){return(firstChild==0);}
	
	int getInt();
	bool getBool();
	double getDouble();
	std::string getString();
	std::string getParsableString();
	void setInt(int _int);
	void setBool(bool _bool);
	void setDouble(double _double);
	void setString(const std::string& _string);
	void setUndefined();
	void setArray();
	bool equals(Var *var);

	Var *mathOp(Var *b, int op);
	void copyValueFrom(Var *var);
	Var *copyThis();


	VarLink* findChild(const std::string& childName);
	VarLink* findChildOrCreate(const std::string& childName,int childType=VAR_UNDEFINED);
	VarLink* findChildByPath(const std::string& path);
	VarLink* addChild(const std::string& childName,Var* child=NULL);
	VarLink* addUniqueChild(const std::string& childName,Var* child=NULL);
	void removeChild(Var* child);
	void removeLink(VarLink* link);
	void removeAllChildren();
    Var *getAtIndex(int idx); // 
    void setAtIndex(int idx, Var *var); //
    int getArrayLength(); //
    int getNumberOfChildren(); //

    Var* getReturnVar();
    void setReturnVar(Var* var);
    Var* getParameter(const std::string& name);

	Var *ref();//get a reference to this variable
	void unref();//unreference
	int getRefNum(){return refNum;}
};


class VarLink{
public:
	std::string name;
	VarLink *prevSibling;
	VarLink *nextSibling;
	Var *var;
	bool owned;

	VarLink(Var *var,const std::string &name);
	VarLink(const VarLink& link);//copy constructor
	~VarLink();
	void replaceWith(Var* var);
	void replaceWith(VarLink* varLink);
	int getIntName();
	void setIntNmae(int idx);
};


VarLink::VarLink(Var *var,const std::string &name){
	this->name=name;
	this->prevSibling=0;
	this->nextSibling=0;
	this->var=var->ref();
	this->owned=false;
}
VarLink::VarLink(const VarLink& link){
	this->name=link.name;
	this->prevSibling=0;
	this->nextSibling=0;
	this->var=link.var->ref();
	this->owned=false;
}
VarLink::~VarLink(){
	var->unref();
}
void VarLink::replaceWith(Var* var){
	this->var->unref();
	this->var=var->ref();
}
void VarLink::replaceWith(VarLink* varLink){
	if(varLink)
		replaceWith(varLink->var);
	else
		replaceWith(new Var());
}
int VarLink::getIntName(){
	return atoi(name.c_str());
}
void VarLink::setIntNmae(int idx){
	std::stringstream ss;
	std::string str;
	ss<<idx;
	ss>>str;
	name=str;
}

void Var::init(){
	refNum=0;
	firstChild=0;
	lastChild=0;
	type=0;
	intData=0;
	doubleData=0;
	stringData="";
	callback=0;
	callbackData=0;
	native=false;
}

Var* Var::ref(){
	refNum++;
	return this;
}

void Var::unref(){
	if(--refNum==0)
		delete this;
}

Var::Var(){
	init();
	type=VAR_UNDEFINED;
}

Var::Var(const std::string &varData, int varType){
	init();
	type=varType;
	switch(type){
		case VAR_DOUBLE:
			doubleData=atof(varData.c_str());
			break;
		case VAR_INTEGER:
			intData=atoi(varData.c_str());
			break;
		case VAR_BOOLEAN:
			if(varData=="false")
				intData=0;
			else
				intData=1;
			break;
		default:
			stringData=varData;
	}
}
Var::Var(double varData){
	init();
	setDouble(varData);	
}
Var::Var(int varData){
	init();
	setInt(varData);
}
Var::Var(bool varData){
	init();
	setBool(varData);
}
Var::~Var(){
	removeAllChildren();
}



int Var::getInt(){
	if(isInt()) return intData;
	if(isDouble()) return (int)doubleData;
	return 0;
}

double Var::getDouble(){
	if(isInt()) return (double)intData;
	if(isDouble()) return doubleData;
	return 0;
}

bool Var::getBool(){
	return (intData!=0);
}

std::string Var::getString(){
	std::stringstream ss;
	std::string ret;
	if(isInt()){
		ss<<intData;
		ss>>ret;
		return ret;
	}
	if(isDouble()){
		ss<<doubleData;
		ss>>ret;
		return ret;
	}
	if(isBoolean()){
		if(intData!=0)
			return "true";
		else
			return "false";
	}
	if(isNull()){
		return "null";
	}
	if(isUndefined()){
		return "undefined";
	}
	return stringData;		
}
std::string Var::getParsableString(){
	if(isNumber()||isBoolean())
		return getString();
	if(isFunction()){
		stringstream ss;
		ss<< "function (";
		VarLink *link=firstChild;
		while(link){
			ss<< link->name;
			if(link->nextSibling) ss<<",";
			link=link->nextSibling;
		}
		ss<<") "<<getString();
		return ss.str();
	}
	if(isString()){
		stringstream ss;
		//TODO
		return getString();
	}

}

void Var::setInt(int _int){
	type=VAR_INTEGER;
	intData=_int;
	doubleData=0;
	stringData="";	
}
void Var::setDouble(double _double){
	type=VAR_DOUBLE;
	intData=0;
	doubleData=_double;
	stringData="";
}
void Var::setBool(bool _bool){
	type=VAR_BOOLEAN;
	intData=_bool;
	doubleData=0;
	stringData="";
}
void Var::setString(const std::string& _string){
	type=VAR_STRING;
	intData=0;
	doubleData=0;
	stringData=_string;
}
void Var::setUndefined(){
	type=VAR_UNDEFINED;
	intData=0;
	doubleData=0;
	stringData="";
	removeAllChildren();
}
void Var::setArray(){
	type=VAR_ARRAY;
	intData=0;
	doubleData=0;
	stringData="";
	removeAllChildren();
}
bool Var::equals(Var* var){
	Var* temp=mathOp(var,TK_EQUAL);
	bool ret=temp->getBool();
	delete temp;
	return ret;
}

Var *Var::mathOp(Var *b, int op){
	Var* a=this;
	if(op==TK_TYPEEQUAL||op==TK_N_TYPEEQUAL){
		bool eql=(a->type==b->type);
		if(eql){
			Var *temp=a->mathOp(b,TK_EQUAL);
			if(!temp->getBool()) eql=false;
			delete temp;
		}
		
		if(op==TK_TYPEEQUAL)
			return new Var(eql);
		else 
			return new Var(!eql);
	}

	if(a->isUndefined()&&b->isUndefined()){
		if(op==TK_EQUAL)return new Var("true",VAR_BOOLEAN);
		if(op==TK_N_EQUAL)return new Var("false",VAR_BOOLEAN);
		return new Var();
	}
	else if((a->isNumber()||a->isUndefined())&&(b->isNumber()||b->isUndefined())){
		if(!a->isDouble()&&!b->isDouble()){
			int aa=a->getInt();
			int bb=b->getInt();
			switch(op){
				case TK_PLUS:return new Var(aa+bb);
				case TK_MINUS:return new Var(aa-bb);
				case TK_MULTIPLY:return new Var(aa*bb);
				case TK_DIVIDE:return new Var(aa/bb);
				case TK_BITWISE_AND:return new Var(aa&bb);
				case TK_BITWISE_OR:return new Var(aa|bb);
				case TK_BITWISE_XOR:return new Var(aa^bb);
				case TK_MOD:return new Var(aa%bb);
				case TK_EQUAL:return new Var(aa==bb);
				case TK_N_EQUAL:return new Var(aa!=bb);
				case TK_LESS:return new Var(aa<bb);
				case TK_GREATER:return new Var(aa>bb);
				case TK_L_EQUAL:return new Var(aa<=bb);
				case TK_G_EQUAL:return new Var(aa>=bb);
				default:;//fault TODO
			}
		}
		else{
			double aa=a->getDouble();
			double bb=b->getDouble();
			switch(op){
				case TK_PLUS:return new Var(aa+bb);
				case TK_MINUS:return new Var(aa-bb);
				case TK_MULTIPLY:return new Var(aa*bb);
				case TK_DIVIDE:return new Var(aa/bb);
				case TK_EQUAL:return new Var(aa==bb);
				case TK_N_EQUAL:return new Var(aa!=bb);
				case TK_LESS:return new Var(aa<bb);
				case TK_GREATER:return new Var(aa>bb);
				case TK_L_EQUAL:return new Var(aa<=bb);
				case TK_G_EQUAL:return new Var(aa>=bb);
				default:;//fault TODO
			}
		}
	}
	else if(a->isArray()||a->isObject()){
		switch(op){
			case TK_EQUAL: return new Var(a==b);
			case TK_N_EQUAL:return new Var(a!=b);
			default:;//fault TODO
		}
	}
	else{
		string aa=a->getString();
		string bb=b->getString();
		switch(op){
				case TK_PLUS:return new Var(aa+bb,VAR_STRING);
				case TK_EQUAL:return new Var(aa==bb);
				case TK_N_EQUAL:return new Var(aa!=bb);
				case TK_LESS:return new Var(aa<bb);
				case TK_GREATER:return new Var(aa>bb);
				case TK_L_EQUAL:return new Var(aa<=bb);
				case TK_G_EQUAL:return new Var(aa>=bb);
				default:;//fault TODO
		} 
	}
	assert(0);
	return 0;
}

VarLink* Var::findChild(const std::string& childName){
	VarLink* v=firstChild;
	while(v){
		if(v->name==childName)
			return v;
		v=v->nextSibling;
	}
	return 0;
}
VarLink* Var::findChildOrCreate(const std::string& childName,int childType){
	VarLink* v=findChild(childName);
	if(v) 
		return v;
	else
		return addChild(childName,new Var("",childType));
}
VarLink* Var::findChildByPath(const std::string& path){
	int pos=path.find('.');
	if(pos==string::npos)
		return findChildOrCreate(path);
	else
		return findChildOrCreate(path.substr(0,pos),VAR_OBJECT)->var->findChildByPath(path.substr(pos+1));
}
VarLink* Var::addChild(const std::string& childName,Var* child){
	if(isUndefined())
		type=VAR_OBJECT;
	if(!child)
		child=new Var();

	VarLink* link=new VarLink(child,childName);
	link->owned=true;
	if(lastChild){
		lastChild->nextSibling=link;
		link->prevSibling=lastChild;
		lastChild=link;
	}
	else{
		firstChild=link;
		lastChild=link;
	}
	return link;

}
VarLink* Var::addUniqueChild(const std::string& childName,Var* child){
	if(!child)
		child=new Var();

	VarLink* link=findChild(childName);
	if(!link){
		link=addChild(childName,child);
	}
	else{
		link->replaceWith(child);
	}
	return link;
}
void Var::removeChild(Var* child){
	VarLink* link=firstChild;
	while(link){
		if(link->var==child)
			break;
		link=link->nextSibling;
	}
	removeLink(link);
}
void Var::removeLink(VarLink* link){
	if(!link) return;
	if (link->nextSibling)
      link->nextSibling->prevSibling = link->prevSibling;
    if (link->prevSibling)
      link->prevSibling->nextSibling = link->nextSibling;
    if (lastChild == link)
        lastChild = link->prevSibling;
    if (firstChild == link)
        firstChild = link->nextSibling;
    delete link;
}
void Var::removeAllChildren(){
	VarLink* link=firstChild;
	while(link){
		VarLink* temp=link;
		link=link->nextSibling;
		delete temp;
	}
	firstChild=0;
	lastChild=0;
}
Var *Var::getAtIndex(int idx){
	stringstream ss;
	string strIdx;
	ss<<idx;
	ss>>strIdx;
	VarLink* link=findChild(strIdx);
	if(link)
		return link->var;
	else
		return new Var("",VAR_NULL);
}
void Var::setAtIndex(int idx, Var *var){
	stringstream ss;
	string strIdx;
	ss<<idx;
	ss>>strIdx;
	VarLink* link=findChild(strIdx);
	if(link)
		link->replaceWith(var);
	else{
		addChild(strIdx,var);
	}
}
//
int Var::getArrayLength(){
	int max=-1;
	if(!isArray()){
		return 0;
	}
	VarLink* link=firstChild;
	while(link){
		int idx=atoi(link->name.c_str());
		if (idx>max)
			max=idx;
		link=link->nextSibling;
	}
	return max+1;

}
int Var::getNumberOfChildren(){
	int count=0;
	VarLink* link=firstChild;
	while(link){
		count++;
		link=link->nextSibling;
	}	
	return count;
}
Var* Var::getReturnVar(){
	return getParameter(JS_IN_C_RETURN_VAR);
}
void Var::setReturnVar(Var* var){
	findChildOrCreate(JS_IN_C_RETURN_VAR)->replaceWith(var);
}
Var* Var::getParameter(const std::string& name){
	return findChildOrCreate(name)->var;
}

void Var::copyValueFrom(Var* var){
	if(var){
		stringData=var->stringData;
		intData=var->intData;
		doubleData=var->doubleData;
		type=var->type;
		
		removeAllChildren();
		VarLink* link=var->firstChild;
		while(link){
			Var* copiedVar=link->var->copyThis();
			addChild(link->name,copiedVar);
			link=link->nextSibling;
		}
	}
	else{
		setUndefined();
	}
}
Var *Var::copyThis(){
	Var* ret=new Var();

	ret->stringData=stringData;
	ret->intData=intData;
	ret->doubleData=doubleData;
	ret->type=type;

	VarLink* link=firstChild;
		while(link){
			Var* copiedVar=link->var->copyThis();

			ret->addChild(link->name,copiedVar);
			link=link->nextSibling;
		}
	return ret;	
}

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
	std::cout<<array->getArrayLength();

}
#endif