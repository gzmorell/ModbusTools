/*
    Modbus Tools

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "core_logview.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QTableView>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QToolBar>
#include <QCoreApplication>
#include <QMap>
#include <QColor>

#include <core.h>
#include <gui/core_ui.h>
#include <gui/dialogs/core_dialogs.h>

//#include "core_logviewmodel.h"

mbCoreLogView::Strings::Strings() :
    prefix(QStringLiteral("Ui.LogView.")),
    maxSize(prefix+QStringLiteral("maxSize")),
    font(prefix+QStringLiteral("font")),
    colors(prefix+QStringLiteral("colors"))
{
}

const mbCoreLogView::Strings &mbCoreLogView::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreLogView::Defaults::Defaults() :
    maxSize(1 << 20),
    font(QFont("Courier New", 8).toString()),
    colors({
        {mb::Log_Error,      QColor(Qt::red)},
        {mb::Log_Warning,    QColor(255, 170, 0)},
        {mb::Log_Info,       QColor(0, 140, 0)},
        {mb::Log_Tx,         QColor(Qt::darkBlue)},
        {mb::Log_Rx,         QColor(50, 0, 50)},
        {mb::Log_Debug,      QColor(Qt::gray)},
        {mb::Log_QtFatal,    QColor(180, 0, 0)},
        {mb::Log_QtCritical, QColor(Qt::red)},
        {mb::Log_QtWarning,  QColor(255, 170, 0)},
        {mb::Log_QtDebug,    QColor(Qt::darkGray)},
        {mb::Log_QtInfo,     QColor(0, 100, 0)}
    })
{
}

const mbCoreLogView::Defaults &mbCoreLogView::Defaults::instance()
{
    static const Defaults s;
    return s;
}

mbCoreLogView::mbCoreLogView(QWidget *parent)
    : QWidget{parent}
{
    const auto &d = Defaults::instance();
    m_core = mbCore::globalCore();
    m_toolBar = new QToolBar(this);
    m_toolBar->setIconSize(QSize(16,16));
    m_toolBar->setContentsMargins(0,0,0,0);

    //m_view = new QTableView(this);
    //m_model = new mbCoreLogViewModel(m_view);
    //m_view->setModel(m_model);
    //QHeaderView *header;
    //header = m_view->horizontalHeader();
    //header->setStretchLastSection(true);
    //header->hide();
    //header = m_view->verticalHeader();
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    //header->hide();

    m_offset = 0;
    m_maxSize = d.maxSize;
    m_colorMap = d.colors;

    m_view = new QTextEdit(this);
    m_view->setReadOnly(true);
    setFontString(d.font);

    QAction *actionClear = new QAction(m_toolBar);
    actionClear->setIcon(QIcon(":/core/icons/clear.png"));
    actionClear->setText(QCoreApplication::translate("mbCoreLogView", "Clear", nullptr));
    connect(actionClear, &QAction::triggered, this, &mbCoreLogView::clear);
    m_toolBar->addAction(actionClear);

    QAction *actionExportLog = new QAction(m_toolBar);
    actionExportLog->setIcon(QIcon(":/core/icons/logexport.png"));
    actionExportLog->setText(QCoreApplication::translate("mbCoreLogView", "Export", nullptr));
    connect(actionExportLog, &QAction::triggered, this, &mbCoreLogView::exportLog);
    m_toolBar->addAction(actionExportLog);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_view);

}

void mbCoreLogView::setMaxSize(int sz)
{
    if (sz < MBLOGVIEW_MAXSIZE_MIN)
        sz = MBLOGVIEW_MAXSIZE_MIN;
    if (m_maxSize != sz)
    {
        m_maxSize = sz;
        //m_offset = m_view->document()->characterCount();
    }
}

QString mbCoreLogView::fontString() const
{
    return m_view->font().toString();
}

void mbCoreLogView::setFontString(const QString &font)
{
    QFont f = m_view->font();
    if (f.fromString(font))
        m_view->setFont(f);
}

QVariant mbCoreLogView::colorMap() const
{
    return mb::toVariant(m_colorMap);
}

void mbCoreLogView::setColorMap(const QVariant &v)
{
    auto map = mb::toColorMap(v);
    // Unite map and m_colorMap
    for (auto it = map.constBegin(); it != map.constEnd(); ++it)
    {
        m_colorMap[it.key()] = it.value();
    }
}

MBSETTINGS mbCoreLogView::cachedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r;
    r[s.maxSize] = this->maxSize   ();
    r[s.font   ] = this->fontString();
    r[s.colors ] = this->colorMap  ();
    return r;
}

void mbCoreLogView::setCachedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    //bool ok;

    it = settings.find(s.maxSize); if (it != end) this->setMaxSize   (it.value().toInt());
    it = settings.find(s.font   ); if (it != end) this->setFontString(it.value().toString());
    it = settings.find(s.colors ); if (it != end) this->setColorMap  (it.value());
}

void mbCoreLogView::clear()
{
    //m_model->clear();
    m_view->clear();
    m_offset = 0;
}

void mbCoreLogView::exportLog()
{
    mbCoreUi *ui = mbCore::globalCore()->coreUi();
    QString fileName = ui->dialogsCore()->getSaveFileName(ui, QStringLiteral("Export Log"), QString(), QStringLiteral("Text files (*.txt);;All files (*)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return;
    file.write(m_view->toPlainText().toUtf8());
    file.close();
}

void mbCoreLogView::logMessage(mb::LogFlag flag, const QString &source, const QString &text)
{
    //m_model->logMessage(flag, source, text);
    QString s;
    if (m_core->useTimestamp())
    {
        s = QString("%1 '%2' [%3]: %4").arg(QDateTime::currentDateTime().toString(m_core->formatDateTime()),
                                          source,
                                          mb::toString(flag),
                                          text);
    }
    else
    {
        s = QString("'%1' [%2]: %3").arg(source,
                                       mb::toString(flag),
                                       text);
    }
    int sz = m_view->document()->characterCount();
    int linesz = s.size();
    if ((sz + linesz) > m_maxSize)
    {
        if (m_offset && ((sz > m_offset)))
        {
            QTextCursor cursor = m_view->textCursor();
            cursor.setPosition(0); // start of document
            auto i = m_offset - 1;
            // Pass possible new line characters
            while (i < sz)
            {
                auto c = m_view->document()->characterAt(i);
                // Unicode 0x2029 represents the Paragraph Separator (PS) character,
                if ((c == '\n') || (c == '\r') || (c.unicode() == 0x2029))
                {
                    ++i; // skip the end of line character
                    continue;
                }
                break;
            }
            cursor.setPosition(i, QTextCursor::KeepAnchor); // select until offset
            cursor.removeSelectedText(); // remove the selection
            m_offset = sz-m_offset;
        }
        else
        {
            m_view->clear();
            m_offset = 0;
        }
    }
    else if ((sz >= m_maxSize/2) && (m_offset == 0))
    {
        m_offset = sz; // remember the point for the future deletion
    }
    QTextCursor cursor = m_view->textCursor();
    cursor.movePosition(QTextCursor::End);

    QTextCharFormat format;
    format.setForeground(logColor(flag));

    cursor.insertText(s, format);
    cursor.insertBlock();
    m_view->setTextCursor(cursor);
}

QColor mbCoreLogView::logColor(mb::LogFlag flag) const
{
    return m_colorMap.value(flag, palette().color(QPalette::Text));
}
