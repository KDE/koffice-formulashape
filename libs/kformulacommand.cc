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

#include <klocale.h>  //This is for undo descriptions

#include <q3valuelist.h>

#include "formulacursor.h"
#include "formulaelement.h"
#include "indexelement.h"
#include "kformulacommand.h"
#include "matrixelement.h"
#include "sequenceelement.h"
#include "textelement.h"


KFORMULA_NAMESPACE_BEGIN

int PlainCommand::evilDestructionCount = 0;

PlainCommand::PlainCommand( const QString& name )
    : KNamedCommand(name)
{
    evilDestructionCount++;
}

PlainCommand::~PlainCommand()
{
    evilDestructionCount--;
}

Command::Command(const QString &name, Container* document)
        : PlainCommand(name), cursordata(0), undocursor(0), doc(document)
{
}

Command::~Command()
{
    delete undocursor;
    delete cursordata;
}

FormulaCursor* Command::getExecuteCursor()
{
    FormulaCursor* cursor = getActiveCursor();
    if (cursordata == 0) {
        setExecuteCursor(getActiveCursor());
    }
    else {
        cursor->setCursorData(cursordata);
    }
    return cursor;
}

void Command::setExecuteCursor(FormulaCursor* cursor)
{
    // assert(cursordata == 0);
    cursordata = cursor->getCursorData();
}

FormulaCursor* Command::getUnexecuteCursor()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(undocursor);
    destroyUndoCursor();
    return cursor;
}

void Command::setUnexecuteCursor(FormulaCursor* cursor)
{
    // assert(undocursor == 0);
    undocursor = cursor->getCursorData();
}


// ******  Generic Add command

KFCAdd::KFCAdd(const QString &name, Container *document)
        : Command(name, document)
{
    addList.setAutoDelete( true );
}


void KFCAdd::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    cursor->insert(addList, beforeCursor);
    setUnexecuteCursor(cursor);
    cursor->setSelection(false);
    testDirty();
}


void KFCAdd::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    cursor->remove(addList, beforeCursor);
    //cursor->setSelection(false);
    cursor->normalize();
    testDirty();
}



// ******  Remove selection command

KFCRemoveSelection::KFCRemoveSelection(Container *document,
                                       Qt::Orientation direction)
        : Command(i18n("Remove Selected Text"), document),
          dir(direction)
{
    removedList.setAutoDelete( true );
}

void KFCRemoveSelection::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    cursor->remove(removedList, dir);
    setUnexecuteCursor(cursor);
    testDirty();
}

void KFCRemoveSelection::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    cursor->insert(removedList);
    cursor->setSelection(false);
    testDirty();
}



KFCReplace::KFCReplace(const QString &name, Container* document)
        : KFCAdd(name, document), removeSelection(0)
{
}

KFCReplace::~KFCReplace()
{
    delete removeSelection;
}

void KFCReplace::execute()
{
    if (getActiveCursor()->isSelection() && (removeSelection == 0)) {
        removeSelection = new KFCRemoveSelection(getDocument());
    }
    if (removeSelection != 0) {
        removeSelection->execute();
    }
    KFCAdd::execute();
}

void KFCReplace::unexecute()
{
    KFCAdd::unexecute();
    if (removeSelection != 0) {
        removeSelection->unexecute();
    }
}



KFCRemove::KFCRemove(Container *document,
                     Qt::Orientation direction)
        : Command(i18n("Remove Selected Text"), document),
          element(0), simpleRemoveCursor(0), dir(direction)
{
    removedList.setAutoDelete( true );
}

KFCRemove::~KFCRemove()
{
    delete simpleRemoveCursor;
    delete element;
}

void KFCRemove::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    cursor->remove(removedList, dir);
    if (cursor->elementIsSenseless()) {
        simpleRemoveCursor = cursor->getCursorData();
        element = cursor->replaceByMainChildContent();
    }
    setUnexecuteCursor(cursor);
    cursor->normalize( dir );
    testDirty();
}

void KFCRemove::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    if (element != 0) {
        cursor->replaceSelectionWith(element);
        element = 0;

        cursor->setCursorData(simpleRemoveCursor);
        delete simpleRemoveCursor;
        simpleRemoveCursor = 0;
    }
    cursor->insert(removedList, dir);
    cursor->setSelection(false);
    testDirty();
}


KFCRemoveEnclosing::KFCRemoveEnclosing(Container* document,
                                       Qt::Orientation dir)
        : Command(i18n("Remove Enclosing Element"), document),
          element(0), direction(dir)
{
}

KFCRemoveEnclosing::~KFCRemoveEnclosing()
{
    delete element;
}

void KFCRemoveEnclosing::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    element = cursor->removeEnclosingElement(direction);
    setUnexecuteCursor(cursor);
    //cursor->normalize();
    cursor->setSelection(false);
    testDirty();
}

void KFCRemoveEnclosing::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    cursor->replaceSelectionWith(element);
    cursor->normalize();
    cursor->setSelection(false);
    element = 0;
    testDirty();
}


// ******  Add root, bracket etc command

KFCAddReplacing::KFCAddReplacing(const QString &name, Container* document)
        : Command(name, document), element(0)
{
}

KFCAddReplacing::~KFCAddReplacing()
{
    delete element;
}


void KFCAddReplacing::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    cursor->replaceSelectionWith(element);
    setUnexecuteCursor(cursor);
    cursor->goInsideElement(element);
    element = 0;
    testDirty();
}


void KFCAddReplacing::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    element = cursor->replaceByMainChildContent();
    cursor->normalize();
    testDirty();
}


// ******  Add index command

KFCAddGenericIndex::KFCAddGenericIndex(Container* document, ElementIndexPtr _index)
        : KFCAdd(i18n("Add Index"), document), index(_index)
{
    addElement(new SequenceElement());
}

void KFCAddGenericIndex::execute()
{
    index->setToIndex(getActiveCursor());
    KFCAdd::execute();
}


KFCAddIndex::KFCAddIndex(Container* document,
                         IndexElement* element, ElementIndexPtr index)
        : KFCAddReplacing(i18n("Add Index"), document),
          addIndex(document, index)
{
    setElement(element);
}

KFCAddIndex::~KFCAddIndex()
{
}

void KFCAddIndex::execute()
{
    KFCAddReplacing::execute();
    addIndex.execute();
}

void KFCAddIndex::unexecute()
{
    addIndex.unexecute();
    KFCAddReplacing::unexecute();
}


KFCChangeBaseSize::KFCChangeBaseSize( const QString& name, Container* document,
                                      FormulaElement* formula, int size )
    : PlainCommand( name ), m_document( document ), m_formula( formula ), m_size( size )
{
    m_oldSize = formula->getBaseSize();
}

void KFCChangeBaseSize::execute()
{
    m_formula->setBaseSize( m_size );
    m_document->recalc();
}

void KFCChangeBaseSize::unexecute()
{
    m_formula->setBaseSize( m_oldSize );
    m_document->recalc();
}


FontCommand::FontCommand( const QString& name, Container* document )
    : Command( name, document )
{
    list.setAutoDelete( false );
    elementList.setAutoDelete( false );
}


void FontCommand::collectChildren()
{
    list.clear();
    uint count = elementList.count();
    for ( uint i=0; i<count; ++i ) {
        elementList.at( i )->dispatchFontCommand( this );
    }
}


void FontCommand::parseSequences( const QMap<SequenceElement*, int>& parents )
{
    Q3ValueList<SequenceElement*> sequences = parents.keys();
    for ( Q3ValueList<SequenceElement*>::iterator i = sequences.begin();
          i != sequences.end();
          ++i ) {
        ( *i )->parse();
    }
}


CharStyleCommand::CharStyleCommand( CharStyle cs, const QString& name, Container* document )
    : FontCommand( name, document ), charStyle( cs )
{
}

void CharStyleCommand::execute()
{
    collectChildren();
    QMap<SequenceElement*, int> parentCollector;

    styleList.clear();
    uint count = childrenList().count();
    styleList.reserve( count );
    for ( uint i=0; i<count; ++i ) {
        TextElement* child = childrenList().at( i );
        styleList[i] = child->getCharStyle();
        child->setCharStyle( charStyle );
        parentCollector[static_cast<SequenceElement*>( child->getParent() )] = 1;
    }
    parseSequences( parentCollector );
    testDirty();
}

void CharStyleCommand::unexecute()
{
    QMap<SequenceElement*, int> parentCollector;
    uint count = childrenList().count();
    //styleList.reserve( count );
    for ( uint i=0; i<count; ++i ) {
        TextElement* child = childrenList().at( i );
        child->setCharStyle( styleList[i] );
        parentCollector[static_cast<SequenceElement*>( child->getParent() )] = 1;
    }
    parseSequences( parentCollector );
    testDirty();
}


CharFamilyCommand::CharFamilyCommand( CharFamily cf, const QString& name, Container* document )
    : FontCommand( name, document ), charFamily( cf )
{
}

void CharFamilyCommand::execute()
{
    collectChildren();

    QMap<SequenceElement*, int> parentCollector;

    familyList.clear();
    uint count = childrenList().count();
    familyList.reserve( count );
    for ( uint i=0; i<count; ++i ) {
        TextElement* child = childrenList().at( i );
        familyList[i] = child->getCharFamily();
        child->setCharFamily( charFamily );
        parentCollector[static_cast<SequenceElement*>( child->getParent() )] = 1;
    }
    parseSequences( parentCollector );
    testDirty();
}

void CharFamilyCommand::unexecute()
{
    QMap<SequenceElement*, int> parentCollector;
    uint count = childrenList().count();
    //familyList.reserve( count );
    for ( uint i=0; i<count; ++i ) {
        TextElement* child = childrenList().at( i );
        child->setCharFamily( familyList[i] );
        parentCollector[static_cast<SequenceElement*>( child->getParent() )] = 1;
    }
    parseSequences( parentCollector );
    testDirty();
}


KFORMULA_NAMESPACE_END
