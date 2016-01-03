//
// Created by user on 2015/12/30.
//

#ifndef TINYJS_VAR_H
#define TINYJS_VAR_H

#include <unordered_map>
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

    bool native;//is native function
    Callback callback;// callback for native functions
    void *callbackData;//

    void init();

public:
    int type;
    shared_ptr<VarLink> firstChild;
    shared_ptr<VarLink> lastChild;
//    vector<Token*> valueTokens;
//    vector<VarLink*> args;
    int len;

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
    bool isBasic(){return(firstChild==nullptr);}

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

    Var *mathOp(Var *b, TOKEN_TYPES op);
    void copyValueFrom(Var *var);
    Var *copyThis();
    void copy(Var* var);

    shared_ptr<VarLink> findChild(const std::string& childName);
    shared_ptr<VarLink> findChildOrCreate(const std::string& childName,int childType=VAR_UNDEFINED);
    shared_ptr<VarLink> findChildByPath(const std::string& path);
    shared_ptr<VarLink> addChild(const std::string& childName,Var* child=NULL);
    shared_ptr<VarLink> addUniqueChild(const std::string& childName,Var* child=NULL);
    void removeChild(Var* child);
    void removeLink(shared_ptr<VarLink> link);
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

const std::string ANONYMOUS = "";

class VarLink{
public:
    std::string name;
    shared_ptr<VarLink> prevSibling;
    shared_ptr<VarLink> nextSibling;
    Var *var;
    bool owned;

    VarLink(Var *var,const std::string &name = ANONYMOUS);
    VarLink(const VarLink& link);//copy constructor
    ~VarLink();
    void replaceWith(Var* var);
    void replaceWith(VarLink* varLink);
    int getIntName();
    void setIntNmae(int idx);
};


class Scope{
    /*
     * @name:the symbol of the scope
     * @vars:the list of the variabes of the scope
     * */
public:
    std::string name;
    map<std::string, shared_ptr<VarLink> > vars;
//    vector<VarLink*> vars;
    Scope(string name){
        this->name=name;
    }
};

#endif //TINYJS_VAR_H
