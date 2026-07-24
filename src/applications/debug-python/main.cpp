// Copyright (c) 2022 Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QT
#include <QFile>
#include <QTextStream>

// C++
#include <iostream>

void pressQToQuit(QTextStream& writeToMe)
{
    writeToMe << "Press \"q\" and \"<enter>\" to quit" << Qt::endl;
    writeToMe << "> ";
    writeToMe.flush();
}

int main
(
    int argc,
    char* argv[]
)
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    QFile standardIn;
    if (standardIn.open(stdin, QIODevice::ReadOnly))
    {
        QFile standardOut;
        QTextStream inStream(&standardIn);

        if (standardOut.open(stdout, QIODevice::WriteOnly))
        {
            QTextStream outStream(&standardOut);
            bool running{true};

            outStream << "This process is detected by Alpaca Python scripts to turn on debugging." << Qt::endl;
            outStream << "All for the want of a #define in Python." << Qt::endl << Qt::endl;

            pressQToQuit(outStream);

            while (running)
            {
                QString line;

                line = inStream.readLine(10);
                line = line.trimmed().toLower();
                if (line.isEmpty() == false)
                {
                    if (line == "q")
                    {
                        running = false;
                    }
                    else
                    {
                        pressQToQuit(outStream);
                    }
                }
                else
                {
                    pressQToQuit(outStream);
                }
            }
        }
        else
        {
            std::cout << "Unabled to open stdout" << std::endl;
        }
    }
    else
    {
        std::cout << "Unabled to open stdout" << std::endl;
    }
}