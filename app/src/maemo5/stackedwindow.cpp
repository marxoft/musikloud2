/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stackedwindow.h"

QList<StackedWindow*> StackedWindow::stack;

StackedWindow::StackedWindow(StackedWindow *parent) :
    QMainWindow(parent),
    m_progressCount(0)
{
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    setAttribute(Qt::WA_DeleteOnClose, stack.size() > 0);
    stack.append(this);
}

StackedWindow::~StackedWindow() {
    stack.removeOne(this);
}

StackedWindow* StackedWindow::currentWindow() {
    return stack.isEmpty() ? 0 : stack.last();
}

void StackedWindow::popWindow() {
    if (StackedWindow *w = currentWindow()) {
        w->close();
    }
}

void StackedWindow::clearWindows() {
    for (int i = stack.size() - 1; i > 0; i--) {
        if (StackedWindow * w = stack.at(i)) {
            w->close();
        }
    }
}

bool StackedWindow::isBusy() const {
    return testAttribute(Qt::WA_Maemo5ShowProgressIndicator);
}

void StackedWindow::showProgressIndicator() {
    m_progressCount++;

    if (isVisible()) {
        setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
    }
}

void StackedWindow::hideProgressIndicator() {
    if (m_progressCount > 0) {
        m_progressCount--;
        
        if (m_progressCount == 0) {
            setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
        }
    }
}

void StackedWindow::showEvent(QShowEvent *e) {
    QMainWindow::showEvent(e);

    if (m_progressCount > 0) {
        setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
    }
}
