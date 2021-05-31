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

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
using namespace std;

class Func;
class Klass;
class KlassI;

#define VAR_VARIABLEP 0
#define VAR_NUMBER 1
#define VAR_STRING 2
#define VAR_KLASS 3
#define VAR_KLASSI 4
#define VAR_FUNC 5
#define VAR_FUNCI 6

class Variable {
    public:
    void init();
    Variable();
    Variable(int stype, double val);
    Variable(int stype, string val);
    Variable(int stype, void *val);
    Variable(int stype, void *vala, void *valb);
    Variable(Variable &copy);
    
    void mkcopy(Variable &copy);
    
    int type;
    
    double nval;
    string sval; // doubles as variable-pointer value
    Klass *kval;
    KlassI *kival;
    Func *fval; // must also have kival set for a function in a klassi
};

#endif
