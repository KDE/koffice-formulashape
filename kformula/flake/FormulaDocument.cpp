/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2010 Inge Wallin <inge@lysator.liu.se>

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

// Own
#include "FormulaDocument.h"

// Qt
#include <QWidget>
#include <QIODevice>
#include <QDebug>
#include <QPainter>

// KOffice
#include <KoDocument.h>
#include <KXmlWriter.h>
#include <KOdfStoreReader.h>
#include <KOdfWriteStore.h>
#include <KOdfLoadingContext.h>
#include <KShapeLoadingContext.h>
#include <KShapeSavingContext.h>
#include <KOdfXmlNS.h>
#include <KOdfStylesReader.h>
#include <KOdfGenericStyles.h>
#include <KOdfEmbeddedDocumentSaver.h>
#include <KoView.h>
#include <KComponentData>
#include <KDebug>

// KFormula
#include "KoFormulaShape.h"


class FormulaDocument::Private
{
public:
    Private();
    ~Private();

    KoFormulaShape *parent;
};

FormulaDocument::Private::Private()
{
}

FormulaDocument::Private::~Private()
{
}

FormulaDocument::FormulaDocument( KoFormulaShape *parent )
    : KoDocument( 0, 0 )
    , d ( new Private )
{
    d->parent = parent;
    // Needed by KoDocument::nativeOasisMimeType().
    // KOdfEmbeddedDocumentSaver uses that method to
    // get the mimetype of the embedded document.
    setComponentData( KComponentData( "math" ) );
}

FormulaDocument::~FormulaDocument()
{
    delete d;
}


bool FormulaDocument::loadOdf( KOdfStoreReader &odfStore )
{
    KXmlDocument doc = odfStore.contentDoc();
    KXmlElement  bodyElement = doc.documentElement();

    kDebug(31000) << bodyElement.nodeName();

    if (bodyElement.nodeName() != "math:math") {
        kError(35001) << "No <math:math> element found.";
        return false;
    }

    // When the formula is stored in an embedded document, it seems to
    // always have a <math:semantics> element that surrounds the
    // actual formula.  I have to check with the MathML spec what this
    // actually means and if it is obligatory.  /iw
    KXmlNode semanticsNode = bodyElement.namedItemNS( KOdfXmlNS::math, "semantics" );
    if ( !semanticsNode.isNull() ) {
        bodyElement = semanticsNode.toElement();
    }

    KOdfLoadingContext   odfLoadingContext( odfStore.styles(), odfStore.store() );
    KShapeLoadingContext context(odfLoadingContext, d->parent->resourceManager());

    return d->parent->loadOdfEmbedded( bodyElement, context );
}

bool FormulaDocument::loadXML( const KXmlDocument &doc, KOdfStore *)
{
    Q_UNUSED( doc );

    // We don't support the old XML format any more.
    return false;
}

bool FormulaDocument::saveOdf( SavingContext &context )
{
    // FIXME: This code is copied from ChartDocument, so it needs to
    // be adapted to the needs of the KoFormulaShape.

    KOdfWriteStore &odfStore = context.odfStore;
    KOdfStore *store = odfStore.store();
    KXmlWriter *manifestWriter = odfStore.manifestWriter();
    KXmlWriter *contentWriter  = odfStore.contentWriter();
    if ( !contentWriter )
        return false;

    KOdfGenericStyles mainStyles;
    KXmlWriter *bodyWriter = odfStore.bodyWriter();
    if ( !bodyWriter )
        return false;

    KOdfEmbeddedDocumentSaver& embeddedSaver = context.embeddedSaver;

    KShapeSavingContext savingContext( *bodyWriter, mainStyles, embeddedSaver );

    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:formula" );

    d->parent->saveOdf( savingContext );

    bodyWriter->endElement(); // office:formula
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfStyles( KOdfGenericStyles::DocumentAutomaticStyles, contentWriter );
    odfStore.closeContentWriter();

    // Add manifest line for content.xml and styles.xml
    manifestWriter->addManifestEntry( url().path() + "/content.xml", "text/xml" );
    manifestWriter->addManifestEntry( url().path() + "/styles.xml", "text/xml" );

    // save the styles.xml
    if ( !mainStyles.saveOdfStylesDotXml( store, manifestWriter ) )
        return false;

    if ( !savingContext.saveDataCenter( store, manifestWriter ) ) {
        return false;
    }

    return true;
}

KoView *FormulaDocument::createViewInstance( QWidget *parent )
{
    Q_UNUSED( parent );

    return 0;
}

void FormulaDocument::paintContent( QPainter &painter, const QRect &rect )
{
    Q_UNUSED( painter );
    Q_UNUSED( rect );
}


