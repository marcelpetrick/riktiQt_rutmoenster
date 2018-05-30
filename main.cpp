//! @author Marcel Petrick; mail@marcelpetrick.it; copyright: currently all rights reserved
//! @version    0.01 unreleased
//! @date   20150408
//!
//! @file   "SRParser.cpp"
//! @brief  main-function to get all the fun started .... todo: write something clever anyone wants to read ... not

//own classes
#include "ItemContainer.h"
#include "SRParser.h"

//Qt-crap
#include <QTextStream>
#include <QCoreApplication>
#include <QStringList>
#include <QFileInfo> //for the file-existance-check

#include <QDebug>

void readFile(QString fileName, ItemContainer* container)
{
//    qDebug() << "readFile()" << endl;

    QFile inputFile(fileName);
    if(inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inputStream(&inputFile);
        while (!inputStream.atEnd())
        {
            static int lineNumber(1); //because pluma numbers starting with 1
            QString const line = inputStream.readLine().trimmed(); //maybe also check simplified()
//            qDebug() << "line " << QString::number(lineNumber) << ": " << line;

            //insert the string into the container
            bool insertSuccess = container->addItem(line);
            Q_UNUSED(insertSuccess);
//            qDebug() << "insertSuccess: " << QString(insertSuccess ? "TRUE" : "FALSE") << endl;

            lineNumber++;
        }

        inputFile.close();
    }
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream cerr(stderr);
    QStringList argList(app.arguments());

//    cout << "argc = " << argc << endl;

//    // print the current argument list
//    for(int i = 0; i < argList.size(); ++i)
//    {
//        cout << QString("argv #%1 is %2").arg(i).arg(argList[i]) << endl;
//    }

    if(/*!argList.isEmpty() &&*/ argList.size() >= 2) //check for existance of at least one "real" parameter
    {
//        cout << "enough args!" << endl;
        QString const path(argList[1]); //get the string: first argument is the binary-name itself
        if(path.endsWith(".ui"))
        {
            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
//                cout << "the fucking file exists! path was \"" << path << "\"" << endl;

                //! insert every single string into the container which also does some determination of the item-type
                ItemContainer* container = new ItemContainer();
                readFile(path, container); //call now the print-all-function

                //! print out everything for debugging
//                qDebug() << container->getCurrentContent() << endl;

                //! hand over the stuff to the SR-parser: takes also ownership of the container
                SRParser parser(container);
                parser.process(); //trigger now the processing

                //! @ todo print the output
                //parser.printResult();
            }
            else
            {
                cerr << "the fucking file DOES NOT exist! path was \"" << path << "\"" << endl;
            }
        }
        else
        {
            cerr << "the fucking file is not of type Qt-form based on the given suffix! path was \"" << path << "\"" << endl;
        }
    }

    return 0; //for success
}
