/* This file is part of the KDE libraries
    Copyright (C) 1999 Ilya Baran (ibaran@acs.bu.edu)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
// $Id$

#include "kformula.h"
#include "box.h"
#include "matrixbox.h"
#include "math.h"
#include <stdio.h>
#include <ctype.h>

//initialize the static members:
QString *KFormula::SPECIAL = NULL;
QString *KFormula::DELIM = NULL;
QString *KFormula::INTEXT = NULL;
QString *KFormula::LOC = NULL;
QString *KFormula::BIGOP = NULL;
QString *KFormula::EVAL = NULL;

void KFormula::initStrings(void)
{
  if(SPECIAL) return;
  SPECIAL = new QString();
  DELIM = new QString();
  LOC = new QString();
  INTEXT = new QString();
  BIGOP = new QString();
  EVAL = new QString();
  
  *SPECIAL += (QChar('{'));
  *SPECIAL += (QChar('}'));
  *SPECIAL += (QChar(PLUS));
  *SPECIAL += (QChar(MINUS));
  *SPECIAL += (QChar(TIMES));
  *SPECIAL += (QChar(DIVIDE));
  *SPECIAL += (QChar(POWER));
  *SPECIAL += (QChar(SQRT));
  *SPECIAL += (QChar(ABS));
  *SPECIAL += (QChar(BRACKET));
  *SPECIAL += (QChar(SUB));
  *SPECIAL += (QChar(LSUB));
  *SPECIAL += (QChar(LSUP));
  *SPECIAL += (QChar(PAREN));
  *SPECIAL += (QChar(EQUAL));
  *SPECIAL += (QChar(MORE));
  *SPECIAL += (QChar(LESS));
  *SPECIAL += (QChar(CAT));
  *SPECIAL += (QChar(SLASH));
  *SPECIAL += (QChar(ABOVE));
  *SPECIAL += (QChar(BELOW));
  *SPECIAL += (QChar(L_GROUP));
  *SPECIAL += (QChar(R_GROUP));
  *SPECIAL += (QChar(MATRIX));
  *SPECIAL += (QChar(SEPARATOR));

  *INTEXT += (QChar(PLUS));
  *INTEXT += (QChar(MINUS));
  *INTEXT += (QChar(TIMES));
  *INTEXT += (QChar(EQUAL));
  *INTEXT += (QChar(MORE));
  *INTEXT += (QChar(LESS));
  *INTEXT += (QChar(SLASH));

  *LOC += (QChar(POWER));
  *LOC += (QChar(SUB));
  *LOC += (QChar(LSUB));
  *LOC += (QChar(LSUP));
  *LOC += (QChar(ABOVE));
  *LOC += (QChar(BELOW));

  *DELIM += (QChar(ABS));
  *DELIM += (QChar(PAREN));
  *DELIM += (QChar(BRACKET));

  *BIGOP += (QChar(INTEGRAL));
  *BIGOP += (QChar(SUM));
  *BIGOP += (QChar(PRODUCT));

  *EVAL += (QChar(PLUS));
  *EVAL += (QChar(MINUS));
  *EVAL += (QChar(TIMES));
  *EVAL += (QChar(DIVIDE));
  *EVAL += (QChar(SLASH));
  *EVAL += (QChar(POWER));
  *EVAL += (QChar(PAREN));
  *EVAL += (QChar(ABS));
  *EVAL += (QChar(SQRT));
}

//This class stores and displays the formula

//---------------------CONSTRUCTORS AND DESTRUCTORS-----------------
KFormula::KFormula(bool r)
{
  posx = posy = 0;
  restricted = r;
}

KFormula::KFormula(int x, int y, bool r)
{
  posx = x;
  posy = y;
  restricted = r;
}

KFormula::~KFormula()
{
  while(boxes.size()) {
    delete boxes[boxes.size() - 1];
    boxes.resize(boxes.size() - 1);
  }
}

//---------------------------GET CURSOR POS-------------------------
//simply calls the function on the last (top level) box.
QRect KFormula::getCursorPos(charinfo i)
{
  QRect tmp = boxes[boxes.size() - 1]->getRect();
  return boxes[boxes.size() - 1]->getCursorPos(i, posx - tmp.center().x(),
				 posy - tmp.center().y());
}

//--------------------------------SIZE------------------------------
QSize KFormula::size()
{
  if(boxes.size() == 0) return QSize(0, 0);

  return QSize(boxes[boxes.size() - 1]->getRect().width(),
	       boxes[boxes.size() - 1]->getRect().height());
}

//--------------------------------REDRAW----------------------------
//first call calculate, figure out the center, and draw the boxes
void KFormula::redraw(QPainter &p)
{
  if(boxes.size() == 0) return;
  boxes[boxes.size() - 1]->calculate(p, p.font().pointSize());
  QRect tmp = boxes[boxes.size() - 1]->getRect();
  boxes[boxes.size() - 1]->draw(p, posx - tmp.center().x(),
			       posy - tmp.center().y());

  return;
}

//-------------------------------SET BOXES-------------------------
//duh
void KFormula::setBoxes(QArray<box *> newBoxes)
{
  while(boxes.size()) {
    delete boxes[boxes.size() - 1];
    boxes.resize(boxes.size() - 1);
  }
  boxes = newBoxes.copy();
}

//--------------------------------GET BOXES------------------------
//an even bigger DUH than the previous one.
QArray<box *> KFormula::getBoxes()
{
  return boxes.copy();
}

void KFormula::setPos(int x, int y)
{
  posx = x;
  posy = y;
}

//-------------------------------UNPARSE----------------------------

//recursively output a string that, when parsed, results in the
//formula.  Pretty self-explanatory.
//it's not used anywhere now, but it might be useful.
//it will need to be fixed before it's used

QString KFormula::unparse(box *b)
{
  if(b == NULL) b = boxes[boxes.size() - 1];

  if(b->type == TEXT) return b->text;

  QString x;
  
  if(b->b1 != NULL && b->b2 != NULL) {
    x.sprintf("{}%c{}", b->type);

    x.insert(1, unparse(b->b1));
    x.insert(x.length() - 1, unparse(b->b2));
    return x;
  }

  if(b->b1 != NULL) {
    x.sprintf("{}%c{}", b->type);
    x.insert(1, unparse(b->b1));
    return x;
  }

  if(b->b2 != NULL) {
    x.sprintf("{}%c{}", b->type);
    x.insert(x.length() - 1, unparse(b->b2));
    return x;
  }
  //if both are NULL
  x.sprintf("{}%c{}", b->type);
  return x;
}

//------------------------------EVALUATE--------------------------------
//default value of b is NULL
//if it's a text box, looks up variables vars and their values
//in vals and returns them.  otherwise, evaluates the children and
//does whatever is necessary to them.
double KFormula::evaluate(QStrList &vars, const QArray<double> &vals,
			  int *error, box *b)
{
  if(!restricted) return 0; // evaluate only if restricted

  int err_dummy;

  if(!error)
    error = &err_dummy;

  *error = NO_ERROR;

  if(!b) b = boxes[boxes.size() - 1];

  if(b->type == TEXT) {
    QString temptext = b->text.stripWhiteSpace();

    if(temptext.length() > 0) { //is it empty?
      double x;
      bool ok;
      char varname[1024];

      x = temptext.toDouble(&ok);
      if(ok) return x; // we have a number

      strcpy(varname, temptext.ascii());

      int i = vars.find(varname);
      if(i != -1) return vals[i];
      else { // variable not found
        *error = UNDEFINED_VARIABLE;
        return 0;
      }
    }
    if(!b->parent) {
      *error = EMPTY_BOX;
      return 0;
    }
    if(b->parent->type == SQRT && b->parent->b1 == b) return 2;
    if(b->parent->type == MINUS && b->parent->b1 == b) return 0;
    if(b->parent->type == PLUS && b->parent->b1 == b) return 0;
    if(delim().contains(QChar(b->parent->type)) && b->parent->b1 == b)
      return 0;

    if(b->parent->type == CAT) return 1; // cat is multiplication

    *error = EMPTY_BOX;
    return 0;
  }

  double b1 = 0, b2 = 0;
  int undefined_in_b1 = 0; // whether b1 had an undefined variable

  if(b->b1) b1 = evaluate(vars, vals, error, b->b1);

  if(*error == UNDEFINED_VARIABLE && b->type == CAT) {
    *error = 0; // it may be a function!
    undefined_in_b1 = 1;
  }

  if(*error) return 0;

  if(b->b2) b2 = evaluate(vars, vals, error, b->b2);

  if(*error) return 0;

  switch(b->type) {
  case PLUS:
    return b1 + b2;
    break;
  case MINUS:
    return b1 - b2;
    break;
  case TIMES:
    return b1 * b2;
    break;
  case SLASH:
  case DIVIDE:
    if(b2)
      return b1 / b2;
    else {
      *error = DIVISION_BY_ZERO;
      return 0;
    }
    break;
  case POWER:
    return pow(b1, b2);
    break;
  case SQRT:
    if(!b1) {
      *error = DIVISION_BY_ZERO;
      return 0;
    }

    if(b2 < 0) {
      *error = ROOT_OF_NEGATIVE;
      return 0;
    }

    return pow(b2, 1 / b1);

    break;
  case PAREN:
    return b2;
    break;
  case ABS:
    return fabs(b2);
    break;
  case CAT: // multiply variables or evaluate functions
    if(!undefined_in_b1) return b1 * b2;

    if(b->b1->type != TEXT) {
      *error = PARSE_ERROR;
      return 0;
    }

    QString fun = b->b1->text.stripWhiteSpace();

    if(!strcmp(fun.ascii(), "sqrt")) return sqrt(b2);
    if(!strcmp(fun.ascii(), "log")) return log(b2);
    if(!strcmp(fun.ascii(), "exp")) return exp(b2);
    if(!strcmp(fun.ascii(), "floor")) return floor(b2);
    if(!strcmp(fun.ascii(), "ceil")) return ceil(b2);
    if(!strcmp(fun.ascii(), "abs")) return fabs(b2);

    if(!strcmp(fun.ascii(), "sin")) return sin(b2);
    if(!strcmp(fun.ascii(), "cos")) return cos(b2);
    if(!strcmp(fun.ascii(), "tan")) return tan(b2);
    if(!strcmp(fun.ascii(), "sinh")) return sinh(b2);
    if(!strcmp(fun.ascii(), "cosh")) return cosh(b2);
    if(!strcmp(fun.ascii(), "tanh")) return tanh(b2);

    if(!strcmp(fun.ascii(), "asin")) return asin(b2);
    if(!strcmp(fun.ascii(), "acos")) return acos(b2);
    if(!strcmp(fun.ascii(), "atan")) return atan(b2);
    if(!strcmp(fun.ascii(), "asinh")) return asinh(b2);
    if(!strcmp(fun.ascii(), "acosh")) return acosh(b2);
    if(!strcmp(fun.ascii(), "atanh")) return atanh(b2);

    break;
  }

  return 0;
}

//INSERTED goes through the charinfo array and increments posinstr
//for all the characters after the one that was inserted.

#define INSERTED(xxi) { int tmpx; if(info) for(tmpx = 0; \
					       tmpx < (int)info->size(); \
					       tmpx++) \
  if((*info)[tmpx].posinstr >= (xxi)) (*info)[tmpx].posinstr++; \
}

// The parser works by first fully parenthesizing (with {}) the string
// and then removing the parentheses one by one in make_boxes,
// adding a box each time.  Not especially efficient or versatile
// but speed is not critical and this was easy to write.

void KFormula::parse(QString text, QArray<charinfo> *info)
{
  int i, j;

  if(info) { //initialize info

    info->resize(text.length() + 1);

    for(i = 0; i <= (int)text.length(); i++) {
      charinfo inf;
      
      inf.where = NULL;
      inf.posinbox = 0;
      inf.posinstr = i;
      inf.left = 0;

      info->at(i) = inf;
    }

    //make empty boxes and put cursor into them if necessary:
    for(i = 0; i < (int)text.length(); i++) {
      if(!special().contains(text[i]) || text[i] == L_GROUP ||
	 text[i] == R_GROUP || intext().contains(text[i])) continue;

      if(i == 0 || (text[i - 1] != R_GROUP &&
		    special().contains(text[i - 1]))) {
	text.insert(i, L_GROUP);
	INSERTED(i);
	text.insert(i + 1, R_GROUP);
	INSERTED(i + 2); //note! we don't move the current cursor over!
	continue;
      }

      if(i == (int)text.length() - 1 ||
	 (text[i + 1] != L_GROUP &&
	  special().contains(text[i + 1]))) {
	text.insert(i + 1, L_GROUP);
	INSERTED(i + 1);
	text.insert(i + 2, R_GROUP);
	INSERTED(i + 3); //note! we don't move the current cursor over!
	continue;
      }
    }

    for(i = 0; i < (int)info->size(); i++) {
      //decide whether to atach the cursor to the left or right character:

      if((*info)[i].posinstr &&
	 (!special().contains(text[(*info)[i].posinstr - 1]) ||
	 intext().contains(text[(*info)[i].posinstr - 1])))
	(*info)[i].posinstr--; //stick to a letter or an intext

      else if((*info)[i].posinstr < (int)text.length() &&
	      (!special().contains(text[(*info)[i].posinstr]) ||
	      intext().contains(text[(*info)[i].posinstr])))
	(*info)[i].left = 1; //stick to letter on the right

      else if((*info)[i].posinstr &&
	      text[(*info)[i].posinstr - 1] == R_GROUP)
	(*info)[i].posinstr--; //stick to an rgroup if possible

      else (*info)[i].left = 1;
    }
  }

  //delete the existing boxes
  while(boxes.size() > 0) {
    delete boxes[boxes.size() - 1];
    boxes.resize(boxes.size() - 1);
  }

  //make "unseen" braces into regular ones:
  for(i = 0; i < (int)text.length(); i++) {
    if(text[i] == QChar(L_BRACE_UNSEEN)) text[i] = L_GROUP;
    if(text[i] == QChar(R_BRACE_UNSEEN)) text[i] = R_GROUP;
  }

  if(restricted) {
    //isolate numbers from letters for evaluation:
    //and insert cats after spaces
    for(i = 0; i < (int)text.length() - 1; i++)
      {
	if(text[i].isDigit() &&
	   text[i + 1].isLetter()) {
	  text.insert(i + 1, QChar(CAT));
	  INSERTED(i + 1);
	  i++;
	}
	else if(text[i] == ' ' && text[i + 1] != ' ') {
	  text.insert(i + 1, QChar(CAT));
	  INSERTED(i + 1);
	  i++;
	}
      }
  }

  //isolate all symbols from text:
  for(i = 0; i < (int)text.length(); i++)
    {
      if(text[i].unicode() >= SYMBOL_ABOVE) {
	if(i > 0 && !special().contains(text[i - 1])) {
	  text.insert(i, QChar(CAT));
	  INSERTED(i);
	  i++;
	}
	if(i < (int)text.length() - 1 && !special().contains(text[i + 1])) {
	  i++;
	  text.insert(i, QChar(CAT));
	  INSERTED(i);
	}
      }
    }


  //search for implicit concatenation:
  //insert a CAT (#) symbol at every concatenation:
  //"a{b}/{c}d" -> "a#{b}/{c}#d".

  for(i = 0; i < ((int)text.length() - 1); i++)
    {
      j = i + 1;
      if(j > (int)text.length() - 1) continue;
      if(text[j] != L_GROUP && text[i] != R_GROUP) continue;
      if(text[i] == R_GROUP && text[j] == L_GROUP) {
	text.insert(j, QChar(CAT));
	INSERTED(j);
      }

      if(text[i].unicode() && text[j].unicode() &&
          special().contains(text[i]) && special().contains(text[j])) continue;
      text.insert(j, QChar(CAT));
      INSERTED(j);
    }

  //now parenthesize everything in reverse order (think about it).
  //After this, "x+y*z^{2}" becomes "{x}+{{y}*{{z}^{{2}}}}"

  //equal and gt lt signs have lowest priority
  for(i = (int)text.length() - 1; i >= 0; i--) {
    if(text[i] != QChar(LESS) && text[i] != QChar(MORE) &&
       text[i] != QChar(EQUAL)) continue;
    parenthesize(text, i, info);
  }

  //addition and subtraction
  for(i = (int)text.length() - 1; i >= 0; i--) {
    if(text[i] != QChar(PLUS) && text[i] != QChar(MINUS)) continue;
    parenthesize(text, i, info);
  }

  //slash and multiplication
  for(i = (int)text.length() - 1; i >= 0; i--) {
    if(text[i] != QChar(SLASH) && text[i] != QChar(TIMES)) continue;
    parenthesize(text, i, info);
  }

  //concatenation--backwards for evaluation
  for(i = 0; i < (int)text.length(); i++) {
    if(text[i] != QChar(CAT)) continue;
    parenthesize(text, i, info);
  }

  //locational things following operand:
  //should not be reversed despite order of ops.
  for(i = (int)text.length() - 1; i >= 0; i--) {
    if(text[i] != QChar(POWER) && text[i] != QChar(SUB) &&
	  text[i] != QChar(ABOVE) && text[i] != QChar(BELOW)) continue;
    parenthesize(text, i, info);
  }

  //locational things preceding operand:
  for(i = 0; i < (int)text.length(); i++) {
    if(text[i] != QChar(LSUB) && text[i] != QChar(LSUP)) continue;
    parenthesize(text, i, info);
  }

  //this is not in usual order of operations but it works:
  //we want "@@x" to become "{}@{{}@{x}}" not "{{}@{}}@{x}" or anything
  //like that

  //roots and division and parentheses:
  for(i = 0; i < (int)text.length(); i++) {
    if(!delim().contains(text[i]) && text[i] != QChar(SQRT) &&
       text[i] != QChar(DIVIDE)) continue;
    parenthesize(text, i, info);
  }

  if ( info )
    for(i = 0; i < (int)info->size(); i++) {
      if(!(*info)[i].left) (*info)[i].posinstr++;
    }

  //Now just make the boxes.
  makeBoxes(text, 0, (int)text.length(), info);

}

//---------------------------PARENTHESIZE--------------------------
//given a string and an operator in position i, this puts
//curly braces around the two groups near the operator.  e.g.:
// Before: "{x}+{y*3}".  Then parenthesize is called with
// i = 6.  After: "{x}+{{y}*{3}}"
void KFormula::parenthesize(QString &temp, int &i, QArray<charinfo> *info)
{
  int j;
  int level = 0;
  int need_paren = 0;

  if(i == 0 || temp[i - 1] != R_GROUP) need_paren = 1;

  //search for the left end of the left group
  for(j = i; j >= 0; j--) {
    if(temp[j] == R_GROUP) level++;
    if(temp[j] == L_GROUP) level--;
    if(level < 0) {
      j++;
      break;
    }

    if(j == 0) break;

    if(j < i && level == 0 && temp[j - 1] != L_GROUP) need_paren = 1;
  }

  //insert left brace of left group if necessary
  if(need_paren) {
    temp.insert(j, L_GROUP);
    INSERTED(j);
    i++;
  //insert the right brace of the left group
    temp.insert(i, R_GROUP);
    INSERTED(i); i++;
  }

  level = 0;

  if(i + 1 < (int)temp.length() && temp[i + 1] == L_GROUP)
    need_paren = 0;
  else need_paren = 1;

  //now search for the right end of the right group
  for(j = i + 1; j <= (int)temp.length(); j++) {
    if(j < (int)temp.length() && temp[j] == L_GROUP) level++;
    if(j < (int)temp.length() && temp[j] == R_GROUP) level--;

    if(level < 0 || j == (int)temp.length()) break;

    if(level == 0 && j > i + 1 && (j < (int)temp.length() - 1 &&
       temp[j + 1] != R_GROUP)) need_paren = 1;
  }

  if(need_paren) {
    //insert right brace of left group:
    temp.insert(j, R_GROUP);
    INSERTED(j);

    //insert the left brace of the right group:
    temp.insert(i + 1, L_GROUP);
    INSERTED(i + 1);
  }
}

//-----------------------------MAKE BOXES-----------------------------
//creates the boxes from a fully parenthesized str.
//calls itself recursively.
box * KFormula::makeBoxes(QString str, int offset,
			  int maxlen, QArray<charinfo> *info)
{
  int toplevel = -1; //the location of the toplevel operator
  int level = 0;
  int i;

  //when the posinstr bug is fixed, change the "maxlen + 1" in the info
  //comparison to "maxlen"

  if(maxlen <= 0) { // make empty box:
    boxes.resize(boxes.size() + 1);
    boxes[boxes.size() - 1] = new box("");
    if(info) { //update the charinfo for all characters in this box
      for(i = 0; i < (int)info->size(); i++) {
	if((*info)[i].posinstr >= offset &&
	   (*info)[i].posinstr - offset <= maxlen) {
	  (*info)[i].where = boxes[boxes.size() - 1];
	  (*info)[i].posinbox = 0;
	}
      }
    }
    
    return boxes[boxes.size() - 1];
  }

  if(str[0] != L_GROUP) { //we have a literal--make a
                                 //TEXT or SYMBOL box:
    boxes.resize(boxes.size() + 1);
    if(str[0].unicode() < SYMBOL_ABOVE) {
      boxes[boxes.size() - 1] = new box(str.left(maxlen));
    }
    else {
      boxes[boxes.size() - 1] = new box(SYMBOL, NULL, NULL);
      boxes[boxes.size() - 1]->text[0] = str[0];
    }

    if(info) {
      for(i = 0; i < (int)info->size(); i++) {
	if((*info)[i].posinstr >= offset &&
	   (*info)[i].posinstr - offset <= maxlen) {
	  (*info)[i].where = boxes[boxes.size() - 1];
	  (*info)[i].posinbox = (*info)[i].posinstr - offset;
	    //	    (1 - (*info)[i].left);
	}
      }
    }
    
    return boxes[boxes.size() - 1];
  }

  //find toplevel:  in "{{x}*{y}}+{3}" toplevel = 9, the +
  for(i = 0; i < maxlen; i++) {
    if(str[i] == L_GROUP) level++;
    if(str[i] == R_GROUP) level--;

    if(level == 0 && i < maxlen - 1) {
      toplevel = i + 1;
      break;
    }
  }

  //if there is no toplevel operand, strip the outside curly braces.  e.g.:
  //"{{x}+{y}}" -> "{x}+{y}".  Done by recursively calling makeBoxes
  //on the appropriate substring.
  if(toplevel == -1) return makeBoxes(str.mid(1), offset + 1,
				      maxlen - 2, info);

  if(str[toplevel] == QChar(MATRIX)) { // we have a matrix!
    int w, h, level, startpos = 0;

    //it looks like this: {w&h}M{{a}&{b}&{x}&...&{y}}
    w = str[toplevel - 4].unicode();
    h = str[toplevel - 2].unicode();

    matrixbox *tmpbox;

    tmpbox = new matrixbox(w, h);

    level = 0;

    for(i = toplevel + 2; i < maxlen - 1; i++) { // now add the elements
      if(str[i] == L_GROUP) {
	if(level == 0) startpos = i; // we start a new element
	level++;
	continue;
      }
      if(str[i] == R_GROUP) {
	level--;
      }

      if(level == 0 && str[i] == QChar(SEPARATOR)) {
	// we add the element
	tmpbox->addElem(makeBoxes(str.mid(startpos + 1),
				  offset + startpos + 1, i - startpos - 2,
				  info));
      }
    }

    boxes.resize(boxes.size() + 1);
    boxes[boxes.size() - 1] = tmpbox;
  }
  else { //we don't have a matrix
    //this stores the returned pointer so we don't add it to the
    //boxes array until all the children have been added.
    box *tmpbox;

    tmpbox = new box((BoxType)(str[toplevel].unicode()), //that's the operator.
		     makeBoxes(str, offset, toplevel, info),
		     makeBoxes(str.mid(toplevel + 1), offset + toplevel + 1,
			       maxlen - toplevel - 1, info));

    boxes.resize(boxes.size() + 1);
    boxes[boxes.size() - 1] = tmpbox;
  }

  if(info) {
    for(i = 0; i < (int)info->size(); i++) {
      //if the character has been assigned nowhere else, assign it to
      //this box.
      if((*info)[i].where == NULL && (*info)[i].posinstr >= offset &&
	 (*info)[i].posinstr - offset <= maxlen) {
	(*info)[i].where = boxes[boxes.size() - 1];
	//for non-TEXT boxes, posinbox is 0 if the character is
	//to the left of the box and 1 if it is to the right.
	if((*info)[i].posinstr - offset > toplevel) (*info)[i].posinbox = 1;
	else (*info)[i].posinbox = 0;
      }
    }
  }

  return boxes[boxes.size() - 1];
}






