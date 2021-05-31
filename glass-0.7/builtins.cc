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
#include <sstream>
using namespace std;

#include "builtins.h"
#include "func.h"
#include "glass.h"
#include "klass.h"
#include "parseq.h"
#include "variable.h"

#define POP \
{ \
    if (mainStack.size() != 0) { \
        delete mainStack[0]; \
        mainStack.pop_front(); \
    } \
}

char builtinDefinitions[] = "{A \
[a~A.a~] \
[s~A.s~] \
[m~A.m~] \
[d~A.d~] \
[(mod)~A.mod~] \
[f~A.f~] \
[e~A.e~] \
[(ne)~A.ne~] \
[(lt)~A.lt~] \
[(le)~A.le~] \
[(gt)~A.gt~] \
[(ge)~A.ge~] \
} \
{S \
[l~S.l~] \
[i~S.i~] \
[(si)~S.si~] \
[a~S.a~] \
[(ns)~S.ns~] \
[(sn)~S.sn~] \
} \
{V \
[n~V.n~] \
[d~V.d~] \
} \
{O \
[o~O.o~] \
[(on)~O.on~] \
} \
{(Debug) \
[(cl)~Debug.cl~] \
[(fl)~Debug.fl~] \
[(fc)~Debug.fc~] \
}";


#define BUILTIN(A, B) else if (which == A && mainStack.size() >= B)
void doBuiltin(string which)
{
    if (0) {
    } BUILTIN("A.a", 2) {
        mainStack[1]->nval = mainStack[1]->nval + mainStack[0]->nval;
        POP;
    } BUILTIN("A.s", 2) {
        mainStack[1]->nval = mainStack[1]->nval - mainStack[0]->nval;
        POP;
    } BUILTIN("A.m", 2) {
        mainStack[1]->nval = mainStack[1]->nval * mainStack[0]->nval;
        POP;
    } BUILTIN("A.d", 2) {
        if (mainStack[0]->nval != 0)
            mainStack[1]->nval = mainStack[1]->nval / mainStack[0]->nval;
        POP;
    } BUILTIN("A.mod", 2) {
        long long a = (long long) mainStack[1]->nval;
        long long b = (long long) mainStack[0]->nval;
        if (b != 0)
            mainStack[1]->nval = a % b;
        POP;
    } BUILTIN("A.f", 1) {
        mainStack[0]->nval = (long long) mainStack[0]->nval;
    } BUILTIN("A.e", 2) {
        mainStack[1]->nval = mainStack[1]->nval == mainStack[0]->nval;
        POP;
    } BUILTIN("A.ne", 2) {
        mainStack[1]->nval = mainStack[1]->nval != mainStack[0]->nval;
        POP;
    } BUILTIN("A.lt", 2) {
        mainStack[1]->nval = mainStack[1]->nval < mainStack[0]->nval;
        POP;
    } BUILTIN("A.le", 2) {
        mainStack[1]->nval = mainStack[1]->nval <= mainStack[0]->nval;
        POP;
    } BUILTIN("A.gt", 2) {
        mainStack[1]->nval = mainStack[1]->nval > mainStack[0]->nval;
        POP;
    } BUILTIN("A.ge", 2) {
        mainStack[1]->nval = mainStack[1]->nval >= mainStack[0]->nval;
        POP;
    } BUILTIN("S.l", 1) {
        mainStack[0]->nval = mainStack[0]->sval.length();
        mainStack[0]->type = VAR_NUMBER;
        mainStack[0]->sval = "";
    } BUILTIN("S.i", 2) {
        if (mainStack[0]->nval >= 0 &&
            mainStack[0]->nval < mainStack[1]->sval.length()) {
            mainStack[1]->sval = mainStack[1]->sval[(int) mainStack[0]->nval];
        } else {
            mainStack[1]->sval = "";
        }
        POP;
    } BUILTIN("S.si", 3) {
        if (mainStack[1]->nval >= 0 &&
            mainStack[1]->nval < mainStack[2]->sval.length() &&
            mainStack[0]->sval.length() > 0) {
            mainStack[2]->sval[(int) mainStack[1]->nval] = mainStack[0]->sval[0];
        }
        POP; POP;
    } BUILTIN("S.a", 2) {
        mainStack[1]->sval += mainStack[0]->sval;
        POP;
    } BUILTIN("S.ns", 1) {
        mainStack[0]->sval = (char) mainStack[0]->nval;
        mainStack[0]->type = VAR_STRING;
    } BUILTIN("S.sn", 1) {
        if (mainStack[0]->sval.length() > 0) {
            mainStack[0]->nval = mainStack[0]->sval[0];
        } else {
            mainStack[0]->nval = 0;
        }
        mainStack[0]->type = VAR_NUMBER;
        mainStack[0]->sval = "";
    } BUILTIN("V.n", 0) {
        // find one
        unsigned int i;
        stringstream nm;
        for (i = 1; i != 0; i++) {
            // check this name ...
            nm.str("");
            nm << "Anonymous" << i;
            if (globalVars.find(nm.str()) == globalVars.end()) {
                // it's free
                globalVars[nm.str()] = new Variable();
                mainStack.push_front(new Variable(VAR_VARIABLEP, nm.str()));
                return;
            }
        }
        // uh oh!
        mainStack.push_front(new Variable());
    } BUILTIN("V.d", 1) {
        // TODO
#ifndef IRC
    } BUILTIN("O.o", 1) {
        cout << mainStack[0]->sval;
        POP;
    } BUILTIN("O.on", 1) {
        cout << mainStack[0]->nval;
        POP;
#else
    } BUILTIN("O.o", 1) {
        IRC_o += mainStack[0]->sval;
        POP;
    } BUILTIN("O.on", 1) {
        stringstream IRC_os;
        IRC_os << mainStack[0]->nval;
        IRC_o += IRC_os.str();
        POP;
#endif
    } BUILTIN("Debug.cl", 0) {
        map<string,Variable *>::iterator svari;
        for (svari = globalVars.begin(); svari != globalVars.end(); svari++) {
            if (svari->second->type == VAR_KLASS) {
#ifndef IRC
                cout << svari->first << " ";
#else
                IRC_o += svari->first + " ";
#endif
            }
        }
    } BUILTIN("Debug.fl", 1) {
        if (mainStack[0]->type != VAR_STRING) { POP; return; }
        if (globalVars.find(mainStack[0]->sval) != globalVars.end()) {
            Klass *tolist = globalVars[mainStack[0]->sval]->kval;
            map<string,Func *>::iterator sfuni;
            for (sfuni = tolist->functions.begin(); sfuni != tolist->functions.end(); sfuni++) {
#ifndef IRC
                cout << sfuni->first << " ";
#else
                IRC_o += sfuni->first + " ";
#endif
            }
        }
        POP;
    } BUILTIN("Debug.fc", 2) {
        if (mainStack[0]->type != VAR_STRING ||
            mainStack[1]->type != VAR_STRING) { POP; POP; return; }
        
        if (globalVars.find(mainStack[1]->sval) != globalVars.end()) {
            Klass *tolist = globalVars[mainStack[1]->sval]->kval;
            if (tolist->functions.find(mainStack[0]->sval) != tolist->functions.end()) {
                Func *ftolist = tolist->functions[mainStack[0]->sval];
                
                // now output it all
                string cont = ftolist->contents->dump();
#ifndef IRC
                cout << cont;
#else
                IRC_o += cont;
#endif
            }
        }
        POP; POP;
    }
}
