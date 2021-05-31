/*
 * Copyright (c) 2005  Gregor Richards
 *
 * This file is part of Glass.
 * 
 * Glass is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Glass is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Glass; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <iostream>
#include <map>
#include <string>
using namespace std;

#include <stdlib.h>

#include "builtins.h"
#include "func.h"
#include "glass.h"
#include "klass.h"
#include "klassi.h"
#include "parseq.h"
#include "variable.h"

ParseQElement::ParseQElement()
{
    type = PQT_GLOBAL;
    value = "";
    next = NULL;
}

ParseQElement::ParseQElement(int st, string sv)
{
    type = st;
    value = sv;
    next = NULL;
}

ParseQElement::ParseQElement(int st, char sv)
{
    string svs;
    svs += sv;
    
    type = st;
    value = svs;
    next = NULL;
}

ParseQElement::ParseQElement(ParseQElement &copy)
{
    type = copy.type;
    value = copy.value;
    next = NULL;
}

ParseQ::ParseQ()
{
    head = NULL;
}

ParseQ::~ParseQ()
{
    ParseQElement *cur, *pre;
    cur = head;
    pre = head;
    while (cur) {
        cur = cur->next;
        delete pre;
        pre = cur;
    }
}
    
int ParseQ::len()
{
    int i;
    ParseQElement *cur;
    
    cur = head;
    i = 0;
    while (cur) {
        i++;
        cur = cur->next;
    }
    
    return i;
}

ParseQElement *ParseQ::get(int n)
{
    ParseQElement *cur = head;
    
    for (; cur && n; n--) cur = cur->next;
    return cur;
}

void ParseQ::add(ParseQElement *a)
{
    ParseQElement *cur;
    
    if (!head) {
        head = a;
    } else {
        cur = head;
        while (cur->next) cur = cur->next;
        
        cur->next = a;
        a->next = NULL;
    }
}

string ParseQ::dump()
{
    string toret;
    ParseQElement *cur = head;
    
    while (cur) {
        switch (cur->type) {
            case PQT_GLOBAL:
            case PQT_CLASSWIDE:
            case PQT_LOCAL:
            case PQT_STACK:
                if (cur->value.length() == 1) {
                    toret += cur->value;
                } else {
                    toret += "(" + cur->value + ")";
                }
                break;
                
            case PQT_NUMBER:
                toret += "<" + cur->value + ">";
                break;
                
            case PQT_STRING:
                toret += "\"" + cur->value + "\"";
                break;
                
            case PQT_COMMAND:
                toret += cur->value;
                break;
                
            case PQT_BUILTIN:
                toret += "~" + cur->value + "~";
                break;
        }
        
        cur = cur->next;
    }
    
    return toret;
}

ParseQ *ParseQ::cutParseQ(int s, int l)
{
    ParseQ *toret = new ParseQ;
    ParseQElement *cur;
    int i;
    
    // there are two very different cases:
    // 1) s is 0 (need to change head)
    // 2) s is >0 (need to find it, not change head)
    if (s == 0) {
        cur = head;
        for (i = 1; cur && i < l; i++) cur = cur->next;
        if (!cur) return toret;
        // now we're on top of the last element
        toret->head = head;
        head = cur->next;
        cur->next = NULL;
        return toret;
    } else {
        ParseQElement *top;
        
        cur = head;
        for (i = 1; cur && i < s; i++) cur = cur->next;
        if (!cur) return toret;
        // now we're immediately before the first element
        top = cur;
        cur = cur->next;
        
        // now find the last element
        for (i = 1; cur && i < l; i++) cur = cur->next;
        if (!cur) return toret;
        // now we're on top of the last element
        toret->head = top->next;
        top->next = cur->next;
        cur->next = NULL;
        return toret;
    }
}

void ParseQ::parseKlasses()
{
    int i, topi;
    ParseQElement *cur, *top;
    ParseQ *klass;
    
    // look for a {
    while (true) {
        cur = head;
        for (i = 0;
             cur &&
             (cur->type != PQT_COMMAND || cur->value != "{");
             i++) cur = cur->next;
        if (!cur) break;
        // we are now on top of the {, one further is the actual start point
        cur->value = "";
        i++; cur = cur->next;
        if (!cur) break;
        topi = i;
        top = cur;
        
        // now find the end
        for (;
             cur &&
             (cur->type != PQT_COMMAND || cur->value != "}");
             i++) cur = cur->next;
        if (!cur) break;
        // we are now on top of the }
        klass = cutParseQ(topi, i - topi);
        klass->parseKlass();
        delete klass;
    }
}

void ParseQ::parseKlass()
{
    ParseQElement *origh, *cur, *top;
    Func *func;
    ParseQ *funcpq;
    string var;
    Klass *klass;
    Variable *kvar;
    int i, topi;
    
    // the first element should be a global variable name
    if (!head || head->type != PQT_GLOBAL) return;
    
    // get rid of the current head
    var = head->value;
    origh = head;
    head = head->next;
    delete origh;
    
    // now start making the actual class
    klass = new Klass();
    kvar = new Variable();
    
    klass->name = var;
    
    kvar->type = VAR_KLASS;
    kvar->kval = klass;
    
    // delete the current one if necessary
    if (globalVars.find(var) != globalVars.end()) {
        if (globalVars[var]->type == VAR_KLASS) {
            delete globalVars[var]->kval;
        }
        delete globalVars[var];
    }
    globalVars[var] = kvar;
    
    while (true) {
        // look for a [
        cur = head;
        for (i = 0;
             cur &&
             (cur->type != PQT_COMMAND || cur->value != "[");
             i++) cur = cur->next;
        if (!cur) break;
        // we are now on top of the [, one further is the actual start point
        cur->value = "";
        i++; cur = cur->next;
        if (!cur) break;
        topi = i;
        top = cur;
        
        // now find the end
        for (;
             cur &&
             (cur->type != PQT_COMMAND || cur->value != "]");
             i++) cur = cur->next;
        if (!cur) break;
        // we are now on top of the ]
        funcpq = cutParseQ(topi, i - topi);
        
        // turn it into a function
        if (!funcpq->head || funcpq->head->type != PQT_CLASSWIDE) {
            delete funcpq;
            continue;
        }
        func = new Func();
        func->name = funcpq->head->value;
        origh = funcpq->head;
        funcpq->head = origh->next;
        func->contents = funcpq;
        klass->functions[origh->value] = func;
        delete origh;
    }
}

#define POP \
{ \
    if (mainStack.size() != 0) { \
        delete mainStack[0]; \
        mainStack.pop_front(); \
    } \
}


void ParseQ::runFunc(KlassI *of, Func *which)
{
    map<string,Variable *> locals;
    map<string,Variable *>::iterator locali;
    
    vector<KlassI *> localInstant;
    ParseQElement *cur;
    unsigned int sloc;
    int depth;
    Variable *toset, *kvar, *fvar;
    KlassI *klassi;
    Func *funci;
    bool doadvance;
    char fchar;
    
    // just run through the elements
    int i;
    cur = head;
    for (i = 0; cur; i++) {
        doadvance = true;
        
        //cout << cur->type << " " << cur->value << endl;
        
        // do something different for each type
        switch (cur->type) {
            case PQT_GLOBAL:
            case PQT_CLASSWIDE:
            case PQT_LOCAL:
                mainStack.push_front(new Variable(VAR_VARIABLEP, cur->value));
                break;
                
            case PQT_STACK:
                // copy a stack location
                sloc = atoi(cur->value.c_str());
                if (mainStack.size() <= sloc) {
                    mainStack.push_front(new Variable());
                } else {
                    mainStack.push_front(new Variable(*mainStack[sloc]));
                }
                break;
                
            case PQT_NUMBER:
                mainStack.push_front(new Variable(VAR_NUMBER, atof(cur->value.c_str())));
                break;
                
            case PQT_STRING:
                mainStack.push_front(new Variable(VAR_STRING, cur->value));
                break;
                
            case PQT_BUILTIN:
                doBuiltin(cur->value);
                break;
                
            case PQT_COMMAND:
                // now we have to switch for each command
                if (cur->value == ",") { // pop
                    if (mainStack.size() > 0) POP;
                    break;
                } else if (cur->value == "^") { // return
                    goto runFuncReturn;
                } else if (cur->value == "=") { // set
                    if (mainStack.size() > 1) {
                        toset = mainStack[1];
                        if (toset->type != VAR_VARIABLEP) {
                            // ERROR
                            return;
                        }
                        toset = getVar(of, &locals, toset->sval);
                        toset->mkcopy(*mainStack[0]);
                        POP; POP;
                    }
                } else if (cur->value ==  "!") { // instantiate
                    if (mainStack.size() > 1) {
                        // two variable pointers ...
                        if (mainStack[0]->type != VAR_VARIABLEP ||
                            mainStack[1]->type != VAR_VARIABLEP) {
                            // ERROR
                            return;
                        }
                        
                        // the second should be to a class
                        kvar = getVar(of, &locals, mainStack[0]->sval);
                        if (kvar->type != VAR_KLASS) {
                            // ERROR
                            return;
                        }
                            
                        // now make a klassi ...
                        klassi = new KlassI();
                        klassi->of = kvar->kval;
                            
                        // and set up the reference
                        toset = getVar(of, &locals, mainStack[1]->sval);
                        toset->type = VAR_KLASSI;
                        toset->kival = klassi;
                        
                        // mark the ownership of the klassi
                        fchar = mainStack[1]->sval[0];
                        if (fchar == '_') {
                            localInstant.push_back(klassi);
                        } else if (fchar >= 'a' && fchar <= 'z') {
                            of->localInstant.push_back(klassi);
                        } else if (fchar >= 'A' && fchar <= 'Z') {
                            globalInstant.push_back(klassi);
                        }
                            
                        POP; POP;
                        
                        // run the constructor
                        if (kvar->kval->functions.find("c__") != kvar->kval->functions.end()) {
                            kvar->kval->functions["c__"]->contents->runFunc(klassi, kvar->kval->functions["c__"]);
                        }
                    }
                    break;
                } else if (cur->value == ".") { // function pointer
                    if (mainStack.size() > 1) {
                        // two variable pointers ...
                        if (mainStack[0]->type != VAR_VARIABLEP ||
                            mainStack[1]->type != VAR_VARIABLEP) {
                            // ERROR
                            return;
                        }
                            
                        // the first should be a klassi
                        kvar = getVar(of, &locals, mainStack[1]->sval);
                        if (kvar->type != VAR_KLASSI) {
                            // ERROR
                            return;
                        }
                            
                        // the second should be a function in that klassi
                        fvar = getVar(kvar->kival, &locals, mainStack[0]->sval);
                        if (fvar->type != VAR_FUNC) {
                            // ERROR
                            return;
                        }
                            
                        // now make the pointer to the funci
                        toset = new Variable(VAR_FUNCI, kvar->kival, fvar->fval);
                        POP; POP;
                        mainStack.push_front(toset);
                    }
                    break;
                } else if (cur->value == "?") { // execute
                    if (mainStack.size() > 0) {
                        // one funci pointer
                        if (mainStack[0]->type != VAR_FUNCI) {
                            // ERROR
                            return;
                        }
                        
                        funci = mainStack[0]->fval;
                        klassi = mainStack[0]->kival;
                        POP;
                        funci->contents->runFunc(klassi, funci);
                    }
                    break;
                } else if (cur->value == "*") { // dereference
                    if (mainStack.size() > 0) {
                        // one variable pointer
                        if (mainStack[0]->type != VAR_VARIABLEP) {
                            // ERROR
                            return;
                        }
                        toset = new Variable(*getVar(of, &locals, mainStack[0]->sval));
                        POP;
                        mainStack.push_front(toset);
                    }
                    break;
                } else if (cur->value == "$") { // make a variable point to "this"
                    if (mainStack.size() > 0) {
                        // one variable pointer
                        if (mainStack[0]->type != VAR_VARIABLEP) {
                            // ERROR
                            return;
                        }
                        toset = getVar(of, &locals, mainStack[0]->sval);
                        toset->type = VAR_KLASSI;
                        toset->kival = of;
                        POP;
                    }
                    break;
                } else if (cur->value == "/") { // loop open, one of the most difficult ones
                    i++;
                    cur = cur->next;
                    if (cur->type > PQT_LOCAL) {
                        // ERROR
                        return;
                    }
                        
                    // skip if the value of this variable is 0/""
                    toset = getVar(of, &locals, cur->value);
                    if ((toset->type == VAR_NUMBER && toset->nval == 0.0) ||
                        (toset->type == VAR_STRING && toset->sval == "") ||
                        (toset->type != VAR_NUMBER && toset->type != VAR_STRING)) {
                        // the really hard part: find the matching backslash
                        depth = 0;
                        i++;
                        cur = cur->next;
                        for (; i < len(); i++) {
                            if (cur->type == PQT_COMMAND) {
                                if (cur->value == "/") {
                                    depth++;
                                } else if (cur->value == "\\") {
                                    if (depth == 0) {
                                        break;
                                    } else {
                                        depth--;
                                    }
                                }
                            }
                            cur = cur->next;
                        }
                        if (i == len()) {
                            // ERROR
                            return;
                        }
                    }
                    break;
                } else if (cur->value == "\\") { // end of loop
                    // just jump back (very inefficient)
                    depth = 0;
                    i--;
                    for (; i >= 0; i--) {
                        cur = get(i);
                        if (cur->type == PQT_COMMAND) {
                            if (cur->value == "\\") {
                                depth++;
                            } else if (cur->value == "/") {
                                if (depth == 0) {
                                    break;
                                } else {
                                    depth--;
                                }
                            }
                        }
                    }
                    if (i == -1) {
                        // ERROR
                        return;
                    }
                        
                    // we are now pointing to the beginning - so don't advance yet
                    doadvance = false;
                }
        }
        
        if (doadvance) {
            cur = cur->next;
        } else {
            i--;
        }
        
#ifdef IRC
        // IRC users have a maximum alotted time
        if (progTimer <= 1) {
            progTimer = 0;
            IRC_o = "Maximum time exceeded.";
            goto runFuncReturn;
        }
        progTimer--;
#endif
    }
runFuncReturn:
    
    // clean up garbage
    for (locali = locals.begin(); locali != locals.end(); locali++) {
        delete locali->second;
    }
    for (unsigned int ui = 0; ui < localInstant.size(); ui++) {
        delete localInstant[ui];
    }
}
