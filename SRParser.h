//! @author Marcel Petrick; mail@marcelpetrick.it; copyright: currently all rights reserved
//! @version    0.01 unreleased
//! @date   20150408
//!
//! @file   "SRParser.h"
//! @brief  Simple Shift-reduce-parser to recognize the structure of the input and create combined tokens out of it.

#pragma once

//own classes
#include "ItemContainer.h"

//Qt-crap
#include <QObject>

class SRParser : public QObject
{
    Q_OBJECT

public:
//    explicit SRParser(QObject* parent = NULL);
    explicit SRParser(ItemContainer* container, QObject* parent = NULL);
    ~SRParser();

    //this is the main method!
    bool process();

private:
    ItemContainer*  mInputContainer;
    ItemContainer*  mStack;

    bool    shift();
    bool    reduce();

//! @todo both functions do almost the same ... combine them; container itself should take care of this
    //for now the only real reduce-functions
    bool    reduceGrid();
    bool    reduceItem();
};
