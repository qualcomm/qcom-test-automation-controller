// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

#ifndef PINECOMMANDLINE_H
#define PINECOMMANDLINE_H

// Qt
#include <QProcess>

class PineCommandLine
{
public:
    PineCommandLine();
    void invokeCli();
private:
    QProcess                    _process;
};

#endif // PINECOMMANDLINE_H
