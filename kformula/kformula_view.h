/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kformula_view_h__
#define __kformula_view_h__

#include <KoView.h>
#include <QStringList>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QPaintEvent>

class KAction;
class KFormulaDoc;
class KFormulaPartView;
class KFormulaWidget;
class KSelectAction;
class QPaintEvent;
class QScrollArea;
class KformulaViewAdaptor;

namespace KFormula {
    class View;
}


/**
 * @short The parts view
 * @version
 */
class KFormulaPartView : public KoView
{
    Q_OBJECT

  public:
    /**
     * The constructor
     * @param _doc a pointer to the associated KFormulaDoc
     * @param _parent a pointer to the view's parent
     * @param _name the view's name
     */
    KFormulaPartView( KFormulaDoc* _doc, QWidget* _parent=0, const char* _name=0 );

    /**
     * A virtual destructor
     */
    virtual ~KFormulaPartView();

    /**
     * Use this function to obtain the DCOPObject
     * @retur na pointer to the DCOPObject
     */
    virtual KformulaViewAdaptor* dbusObject();

    /**
     * Setup the printer to use
     * @param printer a reference to the KPrinter to use
     */
    virtual void setupPrinter( KPrinter &printer );

    /**
     * Prints the view using the @p printer
     * @param printer the printer to use
     */
    virtual void print( KPrinter &printer );

    /**
     * Use this function to obtain the associated KFormulaDoc
     * @return a pointer to the KFormulaDoc
     */
    KFormulaDoc* document() const { return m_pDoc; }

    
    const KFormula::View* formulaView() const;
    KFormula::View* formulaView();

    /**
     * Parse the @p text into the DOM tree
     * @param text the text to parse
     * @result a QStringList with the errors
     */
    QStringList readFormulaString( QString text );

  protected:

    virtual void resizeEvent( QResizeEvent* _ev );
    virtual void focusInEvent( QFocusEvent* );

    virtual void updateReadWrite( bool );

  protected slots:

    void configure();
    void cursorChanged(bool visible, bool selecting);

    void formulaString();

    void sizeSelected( int );

    void slotShowTipOnStart();
    void slotShowTip();

private:

    /**
     * Enables the actions that modify the formula.
     */
    void setEnabled(bool enabled);

    KFormulaDoc *m_pDoc;
    KFormulaWidget* formulaWidget;
    QScrollArea* m_scrollArea;

    KAction* cutAction;
    KAction* copyAction;
    KAction* pasteAction;

    KAction* addBracketAction;
    KAction* addFractionAction;
    KAction* addRootAction;
    KAction* addSumAction;
    KAction* addProductAction;
    KAction* addIntegralAction;
    KAction* addMatrixAction;
    KAction* addUpperLeftAction;
    KAction* addLowerLeftAction;
    KAction* addUpperRightAction;
    KAction* addLowerRightAction;
    KAction* addGenericUpperAction;
    KAction* addGenericLowerAction;
    KAction* removeEnclosingAction;

    KAction* formulaStringAction;

    static bool first_window;
    KformulaViewAdaptor* m_dbus;
};

#endif
