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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <iostream>

#include <qpainter.h>

#include <kapplication.h>
#include <kdebug.h>

#include "basicelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulaview.h"

KFORMULA_NAMESPACE_BEGIN

struct View::View_Impl {

    View_Impl(Container* doc, View* view)
            : smallCursor(false), cursorHasChanged(true),
              document(doc)
    {
        connect(document, SIGNAL(elementWillVanish(BasicElement*)),
                view, SLOT(slotElementWillVanish(BasicElement*)));
        connect(document, SIGNAL(formulaLoaded(FormulaElement*)),
                view, SLOT(slotFormulaLoaded(FormulaElement*)));
        connect(document, SIGNAL(cursorMoved(FormulaCursor*)),
                view, SLOT(slotCursorMoved(FormulaCursor*)));

        cursor = document->createCursor();
    }

    ~View_Impl()
    {
        if ( document->activeCursor() == cursor ) {
            document->setActiveCursor( 0 );
        }
        delete cursor;
    }

    /**
     * If set the cursor will never be bigger that the formula.
     */
    bool smallCursor;

    /**
     * Whether the cursor changed since the last time
     * we emitted a cursorChanged signal.
     */
    bool cursorHasChanged;

    /**
     * The formula we show.
     */
    Container* document;

    /**
     * Out cursor.
     */
    FormulaCursor* cursor;
};


FormulaCursor* View::cursor() const        { return impl->cursor; }
bool& View::cursorHasChanged()             { return impl->cursorHasChanged; }
bool& View::smallCursor()                  { return impl->smallCursor; }
Container* View::container() const { return impl->document; }


View::View(Container* doc)
{
    impl = new View_Impl(doc, this);
    cursor()->calcCursorSize( contextStyle(), smallCursor() );
}

View::~View()
{
    delete impl;
}


QPoint View::getCursorPoint() const
{
    return contextStyle().layoutUnitToPixel( cursor()->getCursorPoint() );
}

void View::setReadOnly(bool ro)
{
    cursor()->setReadOnly(ro);
}


void View::calcCursor()
{
    cursor()->calcCursorSize( contextStyle(), smallCursor() );
}


void View::draw(QPainter& painter, const QRect& rect, const QColorGroup& cg)
{
//     kdDebug( DEBUGID ) << "View::draw: " << rect.x() << " " << rect.y() << " "
//                      << rect.width() << " " << rect.height() << endl;
    container()->draw( painter, rect, cg, true );
    if ( cursorVisible() ) {
        cursor()->draw( painter, contextStyle(), smallCursor() );
    }
}

void View::draw(QPainter& painter, const QRect& rect)
{
    container()->draw( painter, rect, true );
    if ( cursorVisible() ) {
        cursor()->draw( painter, contextStyle(), smallCursor() );
    }
}

void View::keyPressEvent( QKeyEvent* event )
{
    container()->input( event );
}


void View::focusInEvent(QFocusEvent*)
{
    //cursor()->calcCursorSize( contextStyle(), smallCursor() );
    container()->setActiveCursor(cursor());
}

void View::focusOutEvent(QFocusEvent*)
{
    //container()->setActiveCursor(0);
}

void View::mousePressEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mousePressEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mouseReleaseEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mouseReleaseEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mouseDoubleClickEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mouseDoubleClickEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mouseMoveEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mouseMoveEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::wheelEvent( QWheelEvent* event )
{
    const ContextStyle& context = contextStyle();
    wheelEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mousePressEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mousePressEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::mouseReleaseEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mouseReleaseEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::mouseDoubleClickEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mouseDoubleClickEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::mouseMoveEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mouseMoveEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::wheelEvent( QWheelEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    wheelEvent( event, context.ptToLayoutUnitPix( pos ) );
}


void View::mousePressEvent( QMouseEvent* event, const LuPixelPoint& pos )
{
    int flags = movementFlag( event->state() );
    cursor()->mousePress( pos, flags );
    emitCursorChanged();
}

void View::mouseReleaseEvent( QMouseEvent* event, const LuPixelPoint& pos )
{
    int flags = movementFlag( event->state() );
    cursor()->mouseRelease( pos, flags );
    emitCursorChanged();
}

void View::mouseDoubleClickEvent( QMouseEvent*, const LuPixelPoint& )
{
    cursor()->moveRight( WordMovement );
    cursor()->moveLeft( SelectMovement | WordMovement );
    emitCursorChanged();
}

void View::mouseMoveEvent( QMouseEvent* event, const LuPixelPoint& pos )
{
    int flags = movementFlag( event->state() );
    cursor()->mouseMove( pos, flags );
    emitCursorChanged();
}

void View::wheelEvent( QWheelEvent*, const LuPixelPoint& )
{
}


void View::slotCursorMoved(FormulaCursor* c)
{
    if (c == cursor()) {
        cursorHasChanged() = true;
        emitCursorChanged();
    }
}

void View::slotFormulaLoaded(FormulaElement* formula)
{
    cursor()->formulaLoaded(formula);
}

void View::slotElementWillVanish(BasicElement* element)
{
    cursor()->elementWillVanish(element);
    emitCursorChanged();
}

void View::slotSelectAll()
{
    cursor()->moveHome(WordMovement);
    cursor()->moveEnd(SelectMovement | WordMovement);
    emitCursorChanged();
}


void View::moveLeft( int flag )
{
    cursor()->moveLeft( flag );
    emitCursorChanged();
}

void View::moveRight( int flag )
{
    cursor()->moveRight( flag );
    emitCursorChanged();
}

void View::moveUp( int flag )
{
    cursor()->moveUp( flag );
    emitCursorChanged();
}

void View::moveDown( int flag )
{
    cursor()->moveDown( flag );
    emitCursorChanged();
}


void View::moveHome( int flag )
{
    cursor()->moveHome( flag );
    emitCursorChanged();
}

void View::moveEnd( int flag )
{
    cursor()->moveEnd( flag );
    emitCursorChanged();
}


void View::setSmallCursor(bool small)
{
    smallCursor() = small;
}

bool View::isHome() const
{
    return cursor()->isHome();
}

bool View::isEnd() const
{
    return cursor()->isEnd();
}

void View::eraseSelection( Direction direction )
{
    DirectedRemove r( req_remove, direction );
    container()->performRequest( &r );
}

void View::addText( QString str )
{
    TextRequest r( str );
    container()->performRequest( &r );
}

void View::emitCursorChanged()
{
    if (cursor()->hasChanged() || cursorHasChanged()) {
        getDocument()->updateMatrixActions();
        cursor()->clearChangedFlag();
        cursorHasChanged() = false;
        cursor()->calcCursorSize( contextStyle(), smallCursor() );

        emit cursorChanged(cursorVisible(), cursor()->isSelection());
    }
}

const ContextStyle& View::contextStyle() const
{
    return container()->document()->getContextStyle();
}

bool View::cursorVisible()
{
    //return !cursor()->isReadOnly() || cursor()->isSelection();
    return true;
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulaview.moc"
