#ifndef KFORMULATOOLBAR_H_INCLUDED
#define KFORMULATOOLBAR_H_INCLUDED

#include "kformulaedit.h"
#include <ktoolbar.h>

/**
 * Toolbar for formula editing functions.
 *
 * Just make one of these and call connectToFormula passing
 * your KFormulaEdit.
 * @author Ilya Baran <ibaran@acs.bu.edu>
 */
class KFormulaToolBar : public KToolBar
{
  Q_OBJECT
public:
  KFormulaToolBar(QWidget *parent=0L, const char *name=0L, int _item_size = -1);

  void connectToFormula(KFormulaEdit *formula);
};


#endif //KFORMULATOOLBAR_H_INCLUDED
