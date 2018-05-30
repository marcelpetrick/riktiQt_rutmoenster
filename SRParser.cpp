//! @author Marcel Petrick; mail@marcelpetrick.it; copyright: currently all rights reserved
//! @version    0.01 unreleased
//! @date   20150408
//!
//! @file   "SRParser.cpp"
//! @brief  Simple Shift-reduce-parser to recognize the structure of the input and create combined tokens out of it.

#include "SRParser.h"
#include "Item.h"

#include <QDebug>

//SRParser::SRParser(QObject* parent) :
//    QObject(parent),
//    mInputContainer(NULL)
//{
//    qDebug() << "ctor SRParser!" << endl;
//}

SRParser::SRParser(ItemContainer* container, QObject* parent) :
       QObject(parent),
       mInputContainer(container),
       mStack(new ItemContainer())
{
//    qDebug() << "ctor SRParser with param!" << endl;
}

SRParser::~SRParser()
{
//    qDebug() << "dtor SRParser: clean up the members!" << endl;

    //clean up the members
    if(this->mInputContainer)
    {
//        qDebug()<< ".. deleted mContainer!" << endl;
        delete this->mInputContainer;
    }

    if(this->mStack)
    {
//        qDebug()<< ".. deleted mStack!" << endl;
        delete this->mStack;
    }
}

//! @brief  Do the shift-reduce-steps until the input-container is empty.
bool SRParser::process()
{
    bool returnValue(false);

    //! @todo remove this check, because also an empty file would be a valid example for sorting!
    //check first if there is any input
    if(this->mInputContainer->isEmpty())
    {
        qDebug() << "SRParser::process(): empty container!";
        return returnValue;
    }

    //while not empty
    //get the item from the input: if one of the end-tokens, then reduce - maybe also push before doing the reduce: push in inverse order?
    //else push back to the stack

    bool isEmpty(this->mInputContainer->isEmpty());
    bool shiftSucceeded(true);
    bool reduceSucceeded(true);

    //check first emptyness - if failed, then break
    //shift then - if failed, break
    //try to reduce - if failed because of some reduce-error, then break; if just not applicable then do nothing
    while(!isEmpty && shiftSucceeded && reduceSucceeded)
    {
        isEmpty = this->mInputContainer->isEmpty();
        if(isEmpty)
        {
            qDebug() << "SRParser::process(): stop because of emptyness:" << endl;
            //! print out everything for debugging: this is the same like what should be done if everything succeeded
            qDebug() << "###########################################################";
            QString const toPrint(this->mStack->getCurrentContent());            
            qDebug() << toPrint << endl; //print to stderr
            QTextStream cout(stdout);
            cout << toPrint; //print to stdout

            returnValue = false;
            break;
        }

        shiftSucceeded = this->shift();
        if(!shiftSucceeded)
        {
            qDebug() << "SRParser::process(): stop because of shiftFailed:" << endl;
            returnValue = false;
            break;
        }

        //! @todo reduce until not reducible anymore
        reduceSucceeded = this->reduce();
        if(!reduceSucceeded)
        {
            qDebug() << "SRParser::process(): stop because of reduceFailed:" << endl;
            returnValue = false;
            break;
        }

//        qDebug() << "reached end of the while-loop! # mList: " << QString::number(this->mInputContainer->size())
//                 << " # stack: " << QString::number(this->mStack->size());
    }

    return returnValue;
}

//! @brief  Insert the first item (head) of the inputContainer to the stack.
bool SRParser::shift()
{
    bool returnValue(false);

    Item newItem(this->mInputContainer->pop()); //create new item by getting the first from the input-container

    if(newItem.type != Item::ItemType_unknown) //success
    {
        this->mStack->push(newItem); //add it to the stack
        returnValue = true;
    }
    else
    {
        qDebug() << "pop somehow failed! ERROR";
    }

    return returnValue;
}

//! @brief  Check if current content (read backwards) of the stack is reduceable.
//!         This is the case if the last item is the end of a item- or an gridlayout-rule.
//!         In general this would be more complex (determined by the backward-read production rule (right hand side)).
bool SRParser::reduce()
{
    bool returnValue(false);

    Item::ItemType lastEntryType(this->mStack->getTypeLastItem());

    switch(lastEntryType)
    {
    case Item::ItemType_GridEnd:
        this->reduceGrid();
        break;
    case Item::ItemType_ItemEnd:
        this->reduceItem();
    default:
        ; //do nothing
    }

    //! @todo fake for now
    returnValue = true;

    return returnValue;
}

//! @brief  todo
bool SRParser::reduceGrid()
{
    qDebug() << "SRParser::reduceGrid()";

    bool returnValue(true);

//find the next itembegin
//combine everything "from" to "end" into a single item
    int const firstPos(this->mStack->posClosestFromEnd(Item::ItemType_GridBegin));

    if(firstPos != -1)
    {
        unsigned int const lastPos(this->mStack->size() - 1);
        qDebug() << "reduceGrid: reduce now from " << QString::number(firstPos) << " to " << QString::number(lastPos);
        this->mStack->reduce(firstPos, lastPos, Item::ItemType_GridCombined);
    }
    else
    {
        qDebug() << "reduceGrid: did not find any valid entry: big mistake!";
    }

    return returnValue;
}

//! @brief  todo
bool SRParser::reduceItem()
{
    qDebug() << "SRParser::reduceItem()";

    bool returnValue(true);

//find the next itembegin
//combine everything "from" to "end" into a single item
    int const firstPos(this->mStack->posClosestFromEnd(Item::ItemType_ItemBegin));

    if(firstPos != -1)
    {
        unsigned int const lastPos(this->mStack->size() - 1);
        qDebug() << "reduceItem: reduce now from " << QString::number(firstPos) << " to " << QString::number(lastPos);
        this->mStack->reduce(firstPos, lastPos, Item::ItemType_ItemCombined);
    }
    else
    {
        qDebug() << "reduceItem: did not find any valid entry: big mistake!";
    }

    return returnValue;
}
