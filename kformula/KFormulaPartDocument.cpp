/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "KFormulaPartDocument.h"
#include "KFormulaPartView.h"
#include "KFormulaPartFactory.h"
#include "FormulaElement.h"

#include <QPainter>
#include <QString>
#include <QUndoCommand>
#include <QUndoStack>

#include <KoGlobal.h>
#include <KXmlWriter.h>
#include <KoMainWindow.h>
#include <KOdfStorageDevice.h>
#include <KOdfStoreReader.h>
#include <KOdfWriteStore.h>
#include <ktemporaryfile.h>
#include <klocale.h>


KFormulaPartDocument::KFormulaPartDocument( QWidget *parentWidget, QObject* parent,
                                              bool singleViewMode )
                     : KoDocument( parentWidget, parent, singleViewMode )
{
    setComponentData( KFormulaPartFactory::global(), false );

    m_commandHistory = new QUndoStack(this);
    m_formulaElement = new FormulaElement;
/*    connect( m_commandHistory, SIGNAL( commandExecuted() ),
		         this, SLOT( slotExecuted() ) );
    connect( m_commandHistory, SIGNAL( documentRestored() ),
		         this, SLOT( documentRestored() ) );*/
}


KFormulaPartDocument::~KFormulaPartDocument()
{
}


void KFormulaPartDocument::addShape (KShape *shape)
{
    Q_UNUSED( shape );
}

void KFormulaPartDocument::removeShape (KShape *shape)
{
    Q_UNUSED( shape );
}

QMap<QString, KDataCenterBase *> KFormulaPartDocument::dataCenterMap() const
{
    Q_ASSERT(false);
    // if the formula app ever supports more than a single shape this should be implemented
    // along with several other methods - see any other app or ask boemann
    return QMap<QString, KDataCenterBase *>() ;
}

bool KFormulaPartDocument::saveOdf( SavingContext &documentContext )
{
    Q_UNUSED( documentContext );
/*  KOdfStorageDevice dev( store );
 
    KXmlWriter* contentWriter = KOdfWriteStore::createOasisXmlWriter( &dev, "math:math" );
    formulaShape->saveOasis( contentWriter )
    delete contentWriter;

    if( !store->close() )
        return false;
*/
    return true;
}

bool KFormulaPartDocument::loadOdf( KOdfStoreReader & odfStore )
{
    m_formulaElement->readMathML( odfStore.contentDoc().documentElement() );
/*    if ( document->loadOasis( doc ) )
    {
        m_commandHistory->clear();
        m_commendHistory->documentSaved();
        return true;
    }*/
    
    return true;
}

bool KFormulaPartDocument::loadXML(const KXmlDocument& doc, KOdfStore*)
{
    // TODO: Check whether it's really MathML or old KFO format
    m_formulaElement->readMathML( doc.documentElement() );
    return true;
}

KoView* KFormulaPartDocument::createViewInstance( QWidget* parent )
{
    return new KFormulaPartView( this, parent );
}

void KFormulaPartDocument::commandExecuted()
{
    setModified( true );
}

void KFormulaPartDocument::documentRestored()
{
    setModified( false );
}

void KFormulaPartDocument::showStartUpWidget( KoMainWindow* parent, bool )
{
    parent->setRootDocument( this );
}

bool KFormulaPartDocument::showEmbedInitDialog(QWidget* /*parent*/)
{
    return true;
}

void KFormulaPartDocument::paintContent( QPainter& painter, const QRect& rect)
{
    //bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;

    painter.fillRect( rect, Qt::white );
    
//    m_formulaShape->drawFormula( painter, rect );
}

QString KFormulaPartDocument::configFile() const
{
    return QString();
}

