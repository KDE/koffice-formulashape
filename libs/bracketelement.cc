/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#include <qptrlist.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpointarray.h>

#include <kdebug.h>

#include "artwork.h"
#include "bracketelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "sequenceelement.h"

KFORMULA_NAMESPACE_BEGIN

SingleContentElement::SingleContentElement(BasicElement* parent )
    : BasicElement( parent )
{
    content = new SequenceElement( this );
}


SingleContentElement::SingleContentElement( const SingleContentElement& other )
    : BasicElement( other )
{
    content = new SequenceElement( other.content );
    content->setParent( this );
}


SingleContentElement::~SingleContentElement()
{
    delete content;
}


QChar SingleContentElement::getCharacter() const
{
    return content->getCharacter();
}

BasicElement* SingleContentElement::goToPos( FormulaCursor* cursor, bool& handled,
                                             const LuPixelPoint& point, const LuPixelPoint& parentOrigin )
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        LuPixelPoint myPos(parentOrigin.x() + getX(),
                           parentOrigin.y() + getY());

        e = content->goToPos(cursor, handled, point, myPos);
        if (e != 0) {
            return e;
        }
        return this;
    }
    return 0;
}

void SingleContentElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        //bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            content->moveLeft(cursor, this);
        }
        else {
            getParent()->moveLeft(cursor, this);
        }
    }
}

void SingleContentElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        //bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            content->moveRight(cursor, this);
        }
        else {
            getParent()->moveRight(cursor, this);
        }
    }
}

void SingleContentElement::moveUp(FormulaCursor* cursor, BasicElement* /*from*/)
{
    getParent()->moveUp(cursor, this);
}

void SingleContentElement::moveDown(FormulaCursor* cursor, BasicElement* /*from*/)
{
    getParent()->moveDown(cursor, this);
}

void SingleContentElement::remove( FormulaCursor* cursor,
                                   QPtrList<BasicElement>& removedChildren,
                                   Direction direction )
{
    switch (cursor->getPos()) {
    case contentPos:
        BasicElement* parent = getParent();
        parent->selectChild(cursor, this);
        parent->remove(cursor, removedChildren, direction);
    }
}

void SingleContentElement::normalize( FormulaCursor* cursor, Direction direction )
{
    if (direction == beforeCursor) {
        content->moveLeft(cursor, this);
    }
    else {
        content->moveRight(cursor, this);
    }
}

SequenceElement* SingleContentElement::getMainChild()
{
    return content;
}

void SingleContentElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    if (child == content) {
        cursor->setTo(this, contentPos);
    }
}

void SingleContentElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);

    QDomDocument doc = element.ownerDocument();

    QDomElement con = doc.createElement("CONTENT");
    con.appendChild(content->getElementDom(doc));
    element.appendChild(con);
}

bool SingleContentElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }

    if ( !buildChild( content, node, "CONTENT" ) ) {
        kdWarning( DEBUGID ) << "Empty content in " << getTagName() << endl;
        return false;
    }
    node = node.nextSibling();

    return true;
}

void SingleContentElement::writeMathML( QDomDocument doc, QDomNode parent )
{
    content->writeMathML( doc, parent );
}



BracketElement::BracketElement(SymbolType l, SymbolType r, BasicElement* parent)
    : SingleContentElement(parent)
{
    right = createBracket(r);
    left = createBracket(l);
}


BracketElement::~BracketElement()
{
    delete left;
    delete right;
}


BracketElement::BracketElement( const BracketElement& other )
    : SingleContentElement( other )
{
    right = createBracket( other.right->getType() );
    left = createBracket( other.left->getType() );
}


BasicElement* BracketElement::goToPos( FormulaCursor* cursor, bool& handled,
                                       const LuPixelPoint& point, const LuPixelPoint& parentOrigin )
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        LuPixelPoint myPos(parentOrigin.x() + getX(),
                           parentOrigin.y() + getY());
        e = getContent()->goToPos(cursor, handled, point, myPos);
        if (e != 0) {
            return e;
        }

        // We are in one of those gaps.
        luPixel dx = point.x() - myPos.x();
        luPixel dy = point.y() - myPos.y();

        if ((dx > getContent()->getX()+getContent()->getWidth()) ||
            (dy > getContent()->getY()+getContent()->getHeight())) {
            getContent()->moveEnd(cursor);
            handled = true;
            return getContent();
        }
        return this;
    }
    return 0;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void BracketElement::calcSizes(const ContextStyle& style, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle istyle)
{
    SequenceElement* content = getContent();
    content->calcSizes(style, tstyle, istyle);

    if (content->isTextOnly()) {
        left->calcSizes(style, tstyle);
        right->calcSizes(style, tstyle);

        setBaseline(QMAX(content->getBaseline(),
                         QMAX(left->getBaseline(), right->getBaseline())));

        content->setY(getBaseline() - content->getBaseline());
        left   ->setY(getBaseline() - left   ->getBaseline());
        right  ->setY(getBaseline() - right  ->getBaseline());

        //setMidline(content->getY() + content->getMidline());
        setHeight(QMAX(content->getY() + content->getHeight(),
                       QMAX(left ->getY() + left ->getHeight(),
                            right->getY() + right->getHeight())));
    }
    else {
        //kdDebug( DEBUGID ) << "BracketElement::calcSizes " << content->axis( style, tstyle ) << " " << content->getHeight() << endl;
        luPixel contentHeight = 2 * QMAX( content->axis( style, tstyle ),
                                          content->getHeight() - content->axis( style, tstyle ) );
        left->calcSizes( style, tstyle, contentHeight );
        right->calcSizes( style, tstyle, contentHeight );

        // height
        setHeight(QMAX(contentHeight,
                       QMAX(left->getHeight(), right->getHeight())));
        //setMidline(getHeight() / 2);

        content->setY(getHeight() / 2 - content->axis( style, tstyle ));
        setBaseline(content->getBaseline() + content->getY());

        if ( left->getBaseline() != -1 ) {
            left->setY(getBaseline() - left->getBaseline());
        }
        else {
            left->setY((getHeight() - left->getHeight())/2);
        }
        if ( right->getBaseline() != -1 ) {
            right->setY(getBaseline() - right->getBaseline());
        }
        else {
            right->setY((getHeight() - right->getHeight())/2);
        }

//         kdDebug() << "BracketElement::calcSizes" << endl
//                   << "getHeight(): " << getHeight() << endl
//                   << "left->getHeight():  " << left->getHeight() << endl
//                   << "right->getHeight(): " << right->getHeight() << endl
//                   << "left->getY():  " << left->getY() << endl
//                   << "right->getY(): " << right->getY() << endl
//                   << endl;
    }

    // width
    setWidth(left->getWidth() + content->getWidth() + right->getWidth());
    content->setX(left->getWidth());
    right  ->setX(left->getWidth()+content->getWidth());
}


/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void BracketElement::draw( QPainter& painter, const LuPixelRect& r,
                           const ContextStyle& style,
                           ContextStyle::TextStyle tstyle,
                           ContextStyle::IndexStyle istyle,
                           const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
        return;

    SequenceElement* content = getContent();
    content->draw(painter, r, style, tstyle, istyle, myPos);

    if (content->isTextOnly()) {
        left->draw(painter, r, style, tstyle, myPos);
        right->draw(painter, r, style, tstyle, myPos);
    }
    else {
        luPixel contentHeight = 2 * QMAX(content->axis( style, tstyle ),
                                         content->getHeight() - content->axis( style, tstyle ));
        left->draw(painter, r, style, tstyle, contentHeight, myPos);
        right->draw(painter, r, style, tstyle, contentHeight, myPos);
    }

    // Debug
//     painter.setBrush( Qt::NoBrush );
//     painter.setPen( Qt::red );
//     painter.drawRect( style.layoutUnitToPixelX( myPos.x()+left->getX() ),
//                       style.layoutUnitToPixelY( myPos.y()+left->getY() ),
//                       style.layoutUnitToPixelX( left->getWidth() ),
//                       style.layoutUnitToPixelY( left->getHeight() ) );
//     painter.drawRect( style.layoutUnitToPixelX( myPos.x()+right->getX() ),
//                       style.layoutUnitToPixelY( myPos.y()+right->getY() ),
//                       style.layoutUnitToPixelX( right->getWidth() ),
//                       style.layoutUnitToPixelY( right->getHeight() ) );
}


/**
 * Creates a new bracket object that matches the char.
 */
Artwork* BracketElement::createBracket(SymbolType bracket)
{
    Artwork* aw = new Artwork();
    aw->setType( bracket );
    return aw;
}


/**
 * Appends our attributes to the dom element.
 */
void BracketElement::writeDom(QDomElement& element)
{
    SingleContentElement::writeDom(element);
    element.setAttribute("LEFT", left->getType());
    element.setAttribute("RIGHT", right->getType());
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool BracketElement::readAttributesFromDom(QDomElement& element)
{
    if (!BasicElement::readAttributesFromDom(element)) {
        return false;
    }
    QString leftStr = element.attribute("LEFT");
    if(!leftStr.isNull()) {
        left->setType(static_cast<SymbolType>(leftStr.toInt()));
    }
    QString rightStr = element.attribute("RIGHT");
    if(!rightStr.isNull()) {
        right->setType(static_cast<SymbolType>(rightStr.toInt()));
    }
    return true;
}

QString BracketElement::toLatex()
{
    QString ls,rs,cs;
    cs=getContent()->toLatex();
    ls="\\left"+latexString(left->getType());
    rs="\\right"+latexString(right->getType());

    return ls+cs+rs;
}

QString BracketElement::latexString(char type)
{
    switch (type) {
	case ']':
	    return "]";
	case '[':
	    return "[";
	case '{':
	    return "\\{";
	case '}':
	    return "\\}";
	case '(':
	    return "(";
	case ')':
	    return ")";
	case '|':
	    return "|";
        case '<':
            return "\\langle";
        case '>':
            return "\\rangle";
        case '/':
            return "/";
        case '\\':
            return "\\backslash";
    }
    return ".";
}

QString BracketElement::formulaString()
{
    return "(" + getContent()->formulaString() + ")";
}

void BracketElement::writeMathML( QDomDocument doc, QDomNode parent )
{
    QDomElement de = doc.createElement( "mfenced" );
    if ( left->getType() != LeftRoundBracket ||
         right->getType() != RightRoundBracket )
    {
        de.setAttribute( "open",  QString( QChar( left->getType() ) ) );
        de.setAttribute( "close", QString( QChar( right->getType() ) ) );
    }
    SingleContentElement::writeMathML( doc, de );
    parent.appendChild( de );
}


OverlineElement::OverlineElement( BasicElement* parent )
    : SingleContentElement( parent )
{
}

OverlineElement::~OverlineElement()
{
}

OverlineElement::OverlineElement( const OverlineElement& other )
    : SingleContentElement( other )
{
}


void OverlineElement::calcSizes(const ContextStyle& style, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle istyle)
{
    SequenceElement* content = getContent();
    content->calcSizes(style, tstyle,
		       style.convertIndexStyleLower(istyle));

    //luPixel distX = style.ptToPixelX( style.getThinSpace( tstyle ) );
    luPixel distY = style.ptToPixelY( style.getThinSpace( tstyle ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    setWidth( content->getWidth() );
    setHeight( content->getHeight() + distY*2 );

    content->setX( 0 );
    content->setY( distY );
    setBaseline(content->getBaseline() + content->getY());
}

void OverlineElement::draw( QPainter& painter, const LuPixelRect& r,
                            const ContextStyle& style,
                            ContextStyle::TextStyle tstyle,
                            ContextStyle::IndexStyle istyle,
                            const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
        return;

    SequenceElement* content = getContent();
    content->draw( painter, r, style, tstyle,
                   style.convertIndexStyleLower( istyle ), myPos );

    luPixel x = myPos.x();
    luPixel y = myPos.y();
    //int distX = style.getDistanceX(tstyle);
    luPixel distY = style.ptToPixelY( style.getThinSpace( tstyle ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    painter.setPen( QPen( style.getDefaultColor(),
                          style.layoutUnitToPixelY( style.getLineWidth() ) ) );

    painter.drawLine( style.layoutUnitToPixelX( x ),
                      style.layoutUnitToPixelY( y+distY/3 ),
                      style.layoutUnitToPixelX( x+content->getWidth() ),
                      style.layoutUnitToPixelY( y+distY/3 ) );
}


QString OverlineElement::toLatex()
{
    return "\\overline{" + getContent()->toLatex() + "}";
}

QString OverlineElement::formulaString()
{
    return getContent()->formulaString();
}

void OverlineElement::writeMathML( QDomDocument doc, QDomNode parent )
{
    QDomElement de = doc.createElement( "mover" );
    SingleContentElement::writeMathML( doc, de );
    QDomElement op = doc.createElement( "mo" );
    // is this the right entity? Mozilla renders it correctly.
    op.appendChild( doc.createEntityReference( "OverBar" ) );
    de.appendChild( op );
    parent.appendChild( de );
}


UnderlineElement::UnderlineElement( BasicElement* parent )
    : SingleContentElement( parent )
{
}

UnderlineElement::~UnderlineElement()
{
}


UnderlineElement::UnderlineElement( const UnderlineElement& other )
    : SingleContentElement( other )
{
}


void UnderlineElement::calcSizes(const ContextStyle& style,
                                 ContextStyle::TextStyle tstyle,
                                 ContextStyle::IndexStyle istyle)
{
    SequenceElement* content = getContent();
    content->calcSizes(style, tstyle,
		       style.convertIndexStyleLower(istyle));

    //luPixel distX = style.ptToPixelX( style.getThinSpace( tstyle ) );
    luPixel distY = style.ptToPixelY( style.getThinSpace( tstyle ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    setWidth( content->getWidth() );
    setHeight( content->getHeight() + distY*2 );

    content->setX( 0 );
    content->setY( 0 );
    setBaseline(content->getBaseline() + content->getY());
}

void UnderlineElement::draw( QPainter& painter, const LuPixelRect& r,
                             const ContextStyle& style,
                             ContextStyle::TextStyle tstyle,
                             ContextStyle::IndexStyle istyle,
                             const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
        return;

    SequenceElement* content = getContent();
    content->draw( painter, r, style, tstyle,
                   style.convertIndexStyleLower( istyle ), myPos );

    luPixel x = myPos.x();
    luPixel y = myPos.y();
    //int distX = style.getDistanceX(tstyle);
    luPixel distY = style.ptToPixelY( style.getThinSpace( tstyle ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    painter.setPen( QPen( style.getDefaultColor(),
                          style.layoutUnitToPixelY( style.getLineWidth() ) ) );

    painter.drawLine( style.layoutUnitToPixelX( x ),
                      style.layoutUnitToPixelY( y+getHeight()-distY ),
                      style.layoutUnitToPixelX( x+content->getWidth() ),
                      style.layoutUnitToPixelY( y+getHeight()-distY ) );
}


QString UnderlineElement::toLatex()
{
    return "\\underline{" + getContent()->toLatex() + "}";
}

QString UnderlineElement::formulaString()
{
    return getContent()->formulaString();
}

void UnderlineElement::writeMathML( QDomDocument doc, QDomNode parent )
{
    QDomElement de = doc.createElement( "munder" );
    SingleContentElement::writeMathML( doc, de );
    QDomElement op = doc.createElement( "mo" );
    // is this the right entity? Mozilla renders it correctly.
    op.appendChild( doc.createEntityReference( "UnderBar" ) );
    de.appendChild( op );
    parent.appendChild( de );
}

KFORMULA_NAMESPACE_END
