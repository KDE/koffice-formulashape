/*
  TextElement.cc 
  Project KOffice/KFormula
  
  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT
#include <qrect.h> 
#include "BasicElement.h" 
#include "RootElement.h" 
#include "formuladef.h"

RootElement::RootElement(FormulaClass *Formula,BasicElement *Prev=0L,int Relation=-1,BasicElement *Next=0L,
			 QString Content="") : BasicElement(Formula,Prev,Relation,Next,Content)
{
  /*
    Stuff to load pixmap (if need)
  */
  usePixmap=FALSE;
  childrenNumber=1;
}

RootElement::~RootElement()
{
}

void RootElement::draw(QPoint drawPoint,int resolution=72)
{
  
  QPainter *pen;
  pen=formula->painter();
  //QRect globalArea;
  int x,y,unit; //unit is familySize.height/4,used to draw proportional roots
  x=drawPoint.x();
  y=drawPoint.y();
  if( beActive )
    pen->setPen(red);
  unit=familySize.height()/4;
  
  /*  
      Draw root!!
  */
  int ofs=(numericFont/32); 
  /*
   I think I'll change this part of code using a Fill function
   Because painting on a printer I think I may have problems.
  */
  /*
//  int ofs;
  for(ofs=0;ofs<rootFont+1;ofs++){
  pen->drawLine(x+familySize.x()+ofs,y+familySize.y()+unit+1,
		x+familySize.x()+unit+ofs,y+familySize.bottom()+3+ofs);//familySize.y is < 0
  pen->drawLine(x+familySize.x()+unit+ofs,y+familySize.y()+3+ofs,
		x+familySize.x()+unit+ofs,y+familySize.bottom()+ofs);
  pen->drawLine(x+familySize.x()+unit+ofs,y+familySize.y()+3+ofs,
		x+familySize.right()+rootFont+1,y+familySize.y()+3+ofs);
  }*/
  QPointArray points(9);
  points.setPoint(1,x+familySize.x(),y+familySize.y()+unit+1);
  points.setPoint(2,x+familySize.x()+unit,y+familySize.bottom()+3);
  points.setPoint(3,x+familySize.x()+unit+ofs,y+familySize.bottom()+3+ofs);
  points.setPoint(4,x+familySize.x()+unit+ofs,y+familySize.y()+3+ofs);
  points.setPoint(5,x+familySize.right()+unit+ofs,y+familySize.y()+3+ofs);
  points.setPoint(6,x+familySize.right()+unit+ofs,y+familySize.y()+3);
  points.setPoint(7,x+familySize.x()+unit,y+familySize.y()+3);  
  points.setPoint(8,x+familySize.x()+unit,y+familySize.bottom()+3-2*ofs);  
 // points.setPoint(9,x+familySize.x()+unit,y+familySize.bottom()+3);  
  points.setPoint(9,x+familySize.x()+ofs,y+familySize.y()+unit+1);
  pen->setBrush(pen->pen().color());
  pen->drawPolygon(points,FALSE);
  /*
    Draw child[0], it must exist
  */    
  if( beActive )
    pen->setPen(blue);
  child[0]->draw(QPoint(x+familySize.x()+unit+2+ofs+1,y),resolution);
  myArea=globalSize;;
  myArea.moveBy(x,y);
  // globalArea=
  // globalArea.moveBy(x,y);
#ifdef RECT
  pen->drawRect(myArea);   
  // pen->drawRect(globalArea); 
#endif
  if(index[0]!=0L)
    index[0]->draw(drawPoint+
		   familySize.topLeft()-
		   index[0]->getSize().bottomRight()
		   +QPoint(unit/2,unit),
		   resolution);  
  
  drawIndexes(pen,resolution);
  if( beActive )
    pen->setPen(black);
  if(next!=0L) next->draw(drawPoint+QPoint(aSize.width(),0),resolution);
  
  
}

void RootElement::checkSize()
{
  QRect nextDimension; 
  
  if (next!=0L)
    {
      next->checkSize();
      nextDimension=next->getSize();
    }
  
  child[0]->checkSize();
  familySize=child[0]->getSize();
  familySize.setTop(familySize.top()-8-(numericFont/32));
  familySize.setLeft(familySize.left()-8-(numericFont/30)-(familySize.height()/4));  
  
  /*
    aSize is
    child[0]+lines or pixmap
    here put the code to calc it
    unit=familySize.height()/4;
  */
  
  aSize=familySize;
  checkIndexesSize();  //This will change aSize adding Indexes Size
  familySize.moveBy(-aSize.left(),0);
  aSize.moveBy(-aSize.left(),0);
  globalSize=aSize;
  nextDimension.moveBy(aSize.width(),0);
  globalSize=globalSize.unite(nextDimension);
  
}

int RootElement::takeActionFromKeyb(int action)
{
  return -1;
}

void RootElement::setNumericFont(int value)
{
  numericFont=value;
  /*
    We can use differnt pixmap:
    numericFont < 20   tinyRoot.xpm
    20 < numericFont < 50 normalRoot.xpm
    numericFont < 20   bigRoot.xpm
  */
}

void RootElement::drawIndexes(QPainter *pen,int resolution=72)
{
  /*
    Index[0] is in draw() must be lower...
  */
  QPoint dp; //draw point
  dp=myArea.topLeft()-globalSize.topLeft();
  if(index[1]!=0L) index[1]->draw(dp+familySize.bottomLeft()-index[1]->getSize().topRight(),resolution);
  if(index[2]!=0L) index[2]->draw(dp+familySize.topRight()-index[2]->getSize().bottomLeft(),resolution);
  if(index[3]!=0L) index[3]->draw(dp+familySize.bottomRight()-index[3]->getSize().topLeft(),resolution);
}    




