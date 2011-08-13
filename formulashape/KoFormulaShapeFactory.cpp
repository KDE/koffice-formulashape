/* This file is part of the KDE project
 * Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "KoFormulaShapeFactory.h"

#include <klocale.h>
#include <kdebug.h>

#include <KShapeFactoryBase.h>
#include <KShapeLoadingContext.h>
#include <KOdfXmlNS.h>

#include "KoFormulaShape.h"


KoFormulaShapeFactory::KoFormulaShapeFactory( QObject *parent )
    : KShapeFactoryBase( parent, KoFormulaShapeId, i18n( "Formula" ) )
{
    setToolTip(i18n( "A formula"));
    setIcon( "x-shape-formula" );

    // The following lines let the formula shape load both embedded and
    // inline formulas.
    //
    // The line below tells the shape registry which XML elements that
    // the shape supports.
    //
    // FIXME: Find out if inline formulas are supported by ODF.

    QList<QPair<QString, QStringList> > elementNamesList;
    elementNamesList.append(qMakePair(QString(KOdfXmlNS::draw), QStringList("object")));
    elementNamesList.append(qMakePair(QString(KOdfXmlNS::math), QStringList("math")));
    setOdfElements(elementNamesList);

    setLoadingPriority( 1 );
/*    KoShapeTemplate t;
    t.id = KoFormulaShapeId;
    t.name = i18n("Formula");
    t.toolTip = i18n("A formula");
    t.icon = ""; //TODO add it
    props = new KProperties();
    t.properties = props;
    addTemplate( t );*/
}

KoFormulaShapeFactory::~KoFormulaShapeFactory()
{}

KShape *KoFormulaShapeFactory::createDefaultShape(KResourceManager *resourceManager) const
{
    KoFormulaShape* formula = new KoFormulaShape(resourceManager);
    formula->setShapeId( KoFormulaShapeId );
    return formula;
}

bool KoFormulaShapeFactory::supports(const KXmlElement& e, KShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    bool retval = ((e.localName() == "math"
                    && e.namespaceURI() == KOdfXmlNS::math)
                   || (e.localName() == "object"
                       && e.namespaceURI() == KOdfXmlNS::draw));

    // Should be 39001 (kformula) instead of 31000 (koffice)
    //kDebug(31000) << e.nodeName() << " - "<< e.namespaceURI(); 
    //kDebug(31000) << "Return value = " << retval;

    return retval;
}


#include "KoFormulaShapeFactory.moc"
