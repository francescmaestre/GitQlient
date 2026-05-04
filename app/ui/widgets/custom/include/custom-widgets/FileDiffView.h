#pragma once

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2020 Francesc Maestre
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <custom-widgets/DiffInfo.h>

#include <QPlainTextEdit>

class FileDiffHighlighter;

class LineNumberArea;

class FileDiffView : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void signalScrollChanged(int value);

public:
    explicit FileDiffView(QColor additionColor, QColor removalColor, QColor commentColor, QWidget* parent = nullptr);
    ~FileDiffView();

    void addNumberArea(LineNumberArea* numberArea);
    void loadDiff(
        const QString& text,
        const QVector<ChunkDiffInfo::ChunkInfo>& fileDiffInfo = QVector<ChunkDiffInfo::ChunkInfo>());
    void moveScrollBarToPos(int value);
    void setStartingLine(int lineNumber) { mStartingLine = lineNumber; }
    int getHeight() const;
    int getLineHeigth() const;
    QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* target, QEvent* event) override;

private:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect& rect, int dy);
    int lineNumberAreaWidth();

    QVector<ChunkDiffInfo::ChunkInfo> mFileDiffInfo;
    LineNumberArea* mLineNumberArea = nullptr;
    FileDiffHighlighter* mDiffHighlighter = nullptr;
    int mStartingLine = 0;
    int mRow = -1;

    friend class LineNumberArea;
};
