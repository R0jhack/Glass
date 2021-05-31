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

#ifndef PARSEQ_H
#define PARSEQ_H

#include <string>
using namespace std;

class Func;
class KlassI;

#define PQT_GLOBAL 0
#define PQT_CLASSWIDE 1
#define PQT_LOCAL 2
#define PQT_STACK 3
#define PQT_NUMBER 4
#define PQT_STRING 5
#define PQT_COMMAND 6
#define PQT_BUILTIN 7

class ParseQElement {
    public:
    ParseQElement();
    ParseQElement(int st, string sv);
    ParseQElement(int st, char sv);
    ParseQElement(ParseQElement &copy);
    
    int type;
    
    string value;
    
    ParseQElement *next;
};

class ParseQ {
    public:
    ParseQ();
    ~ParseQ();
    
    int len();
    ParseQElement *get(int n);
    void add(ParseQElement *a);
    
    ParseQ *cutParseQ(int s, int l);
    
    void parseKlasses();
    void parseKlass();
    void runFunc(KlassI *of, Func *which);
    
    protected:
    ParseQElement *head;
};

#endif
