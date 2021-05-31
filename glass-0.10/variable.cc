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

#include "variable.h"

void Variable::init()
{
    type = VAR_NUMBER;
    nval = 0.0;
    sval = "";
    kval = NULL;
    kival = NULL;
    fval = NULL;
}

Variable::Variable() { init(); }

Variable::Variable(int stype, double val)
{
    init();
    if (stype == VAR_NUMBER) {
        nval = val;
    }
}

Variable::Variable(int stype, string val)
{
    init();
    if (stype == VAR_VARIABLEP ||
        stype == VAR_STRING) {
        type = stype;
        sval = val;
    }
}

Variable::Variable(int stype, void *val)
{
    init();
    if (stype == VAR_KLASS) {
        type = stype;
        kval = (Klass *) val;
    } else if (stype == VAR_KLASSI) {
        type = stype;
        kival = (KlassI *) val;
    } else if (stype == VAR_FUNC) {
        type = stype;
        fval = (Func *) val;
    }
}

Variable::Variable(int stype, void *vala, void *valb)
{
    init();
    if (stype == VAR_FUNCI) {
        type = stype;
        kival = (KlassI *) vala;
        fval = (Func *) valb;
    }
}

Variable::Variable(Variable &copy)
{
    mkcopy(copy);
}

void Variable::mkcopy(Variable &copy)
{
    type = copy.type;
    nval = copy.nval;
    sval = copy.sval;
    kval = copy.kval;
    kival = copy.kival;
    fval = copy.fval;
}
