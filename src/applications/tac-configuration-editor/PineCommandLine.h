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
