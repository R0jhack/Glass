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
using namespace std;

#include "builtins.h"
#include "glass.h"
#include "variable.h"

#define POP \
{ \
    delete mainStack[0]; \
    mainStack.pop_front(); \
}

char builtinDefinitions[] = "{A \
[a~A.a~] \
[s~A.s~] \
[m~A.m~] \
[d~A.d~] \
[e~A.e~] \
[(ne)~A.ne~] \
[(lt)~A.lt~] \
[(le)~A.le~] \
[(gt)~A.gt~] \
[(ge)~A.ge~] \
} \
{O \
[o~O.o~] \
[(on)~O.on~] \
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
        mainStack[1]->nval = mainStack[1]->nval / mainStack[0]->nval;
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
    } BUILTIN("O.o", 1) {
        cout << mainStack[0]->sval;
        POP;
    } BUILTIN("O.on", 1) {
        cout << mainStack[0]->nval;
        POP;
    }
}

