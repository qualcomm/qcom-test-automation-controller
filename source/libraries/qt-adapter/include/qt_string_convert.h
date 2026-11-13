// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Conversion utilities between qtac:: string types and Qt string types.

#pragma once

#include <qtac/ByteArray.h>
#include <qtac/String.h>

#include <QByteArray>
#include <QString>

namespace QtAdapter {

inline QString toQString(const qtac::String& s)
{
    return QString::fromStdString(s.toStdString());
}

inline QString toQString(const qtac::ByteArray& b)
{
    return QString::fromLatin1(b.toStdString());
}

inline QByteArray toQByteArray(const qtac::ByteArray& b)
{
    const std::string s = b.toStdString();
    return QByteArray(s.data(), static_cast<qsizetype>(s.size()));
}

inline QByteArray toQByteArray(const qtac::String& s)
{
    return toQByteArray(qtac::ByteArray(s.toStdString()));
}

inline qtac::String toQtacString(const QString& s)
{
    return qtac::String(s.toStdString());
}

inline qtac::ByteArray toQtacByteArray(const QByteArray& b)
{
    return qtac::ByteArray(b.toStdString().c_str());
}

inline qtac::ByteArray toQtacByteArray(const QString& s)
{
    return qtac::ByteArray(s.toLatin1().toStdString().c_str());
}

} // namespace QtAdapter
