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

#ifndef MATRIXDIALOG_H_INCLUDED
#define MATRIXDIALOG_H_INCLUDED

#include <qwidget.h>
#include <qdialog.h>
#include <qvalidator.h>

//  Here's a dialog for entering matrix sizes:
class MatrixDialog : public QDialog
{
  Q_OBJECT
public:
  int w, h;
  bool zeroFill;

  MatrixDialog(QWidget *parent);

  ~MatrixDialog() {}

protected slots:
  void setWidth(int value);
  void setHeight(int value);
  void setFill(bool value);

};

#endif // MATRIXDIALOG_H_INCLUDED