//
// Created by user on 2015/12/30.
//
#ifndef _var_h_
#define _var_h_


#include <unordered_map>
#include <memory>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <sstream>

#include <iostream>
#include "Lex.h"

class Var;

class VarLink;


#define JS_RETURN_VAR   "__builtin__return"
#define JS_FUNCBODY_VAR "__builtin__body"
#define JS_ARGC_VAR     "__builtin__argc"
#define JS_ARGV_VAR     "__builtin__argv"
#define JS_SCOPE        "__builtin__scope"
#define JS_SCOPE_NUM    "__builtin__scope__num"
#define JS_THIS_VAR     "this"
#define ANONYMOUS_VAR   ""
#define VAR_BLANK       ""


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

class Var {
protected:
    int refNum;
    std::string stringData;
    int intData; //int and bool
    double doubleData;

    void init();

public:
    int type;
    std::shared_ptr<VarLink> firstChild;
    std::shared_ptr<VarLink> lastChild;

    Var();

    //create undefined
    Var(const std::string &varData, int varType = VAR_STRING);

    Var(bool varData);

    Var(int varData);

    Var(double varData);

    ~Var();

    bool isInt() { return (type == VAR_INTEGER); }

    bool isDouble() { return (type == VAR_DOUBLE); }

    bool isString() { return (type == VAR_STRING); }

    bool isNumber() { return (type == VAR_INTEGER || type == VAR_DOUBLE); }

    bool isFunction() { return (type == VAR_FUNCTION); }

    bool isObject() { return (type == VAR_OBJECT); }

    bool isArray() { return (type == VAR_ARRAY); }

    bool isBoolean() { return (type == VAR_BOOLEAN); }

    bool isUndefined() { return (type == VAR_UNDEFINED); }

    bool isNull() { return (type == VAR_NULL); }

    bool isBasic() { return (firstChild == nullptr); }

    int getInt();

    bool getBool();

    double getDouble();

    std::string getString();

    void setInt(int _int);

    void setBool(bool _bool);

    void setDouble(double _double);

    void setString(const std::string &_string);

    void setUndefined();

    Var *mathOp(Var *b, TOKEN_TYPES op);

    Var *copyThis();

    void copy(Var *var);

    std::shared_ptr<VarLink> findChild(const std::string &childName);

    std::shared_ptr<VarLink> findChildOrCreate(const std::string &childName, int childType = VAR_UNDEFINED);

    std::shared_ptr<VarLink> addChild(const std::string &childName, Var *child = NULL);

    std::shared_ptr<VarLink> addUniqueChild(const std::string &childName, Var *child = NULL);

    void removeChild(Var *child);

    void removeLink(std::shared_ptr<VarLink> link);

    void removeAllChildren();

    int getArrayLength(); //



    Var *ref();

    //get a reference to this variable
    void unref();

};


class VarLink {
public:
    std::string name;
    std::shared_ptr<VarLink> prevSibling;
    std::shared_ptr<VarLink> nextSibling;
    Var *var;
    bool owned;

    VarLink(Var *var, const std::string &name = ANONYMOUS_VAR);

    //copy constructor
    VarLink(const VarLink &link);

    ~VarLink();

    void replaceWith(Var *var);

    void replaceWith(shared_ptr<VarLink> varLink);

};


#endif 
