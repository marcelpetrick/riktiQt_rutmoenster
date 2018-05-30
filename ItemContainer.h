//! @author     Marcel Petrick; mail@marcelpetrick.it
//! @license    GNU GENERAL PUBLIC LICENSE Version 3
//! @version    0.02
//! @date       20180530
//!
//! @file   "ItemContainer.h"
//! @brief   Keeps the "list" of all line-entries: with type and cleaned "full" string

#pragma once

//own classes
#include "Item.h"

//Qt-includes
#include <QObject>
#include <QList>
#include <QDebug>

class ItemContainer : public QObject
{
    Q_OBJECT

private:
    QList<Item>     mList;
    Item::ItemType determineType(QString input);

public:
    explicit        ItemContainer(QObject* parent = nullptr);
    bool            addItem(QString input);
    QString         getCurrentContent();
    QString         getItemTypeString(Item::ItemType input);
    unsigned int    size();
    bool            isEmpty();

    //! @todo add function for "get head" - and remove it. something like pop!
    Item            pop();
    void            push(Item input);

    //reduce-step
    Item::ItemType  getTypeLastItem(); //for checking if a reduce-step would be applicable
    int             posClosestFromEnd(Item::ItemType type);
    bool            reduce(unsigned int begin, unsigned int end, Item::ItemType replacementType);
    ItemContainer*  cutOff(unsigned int begin, unsigned int end);
    QString         reduceSorted();

    //check the row&col-data of the given index; use the returnValue to determine success or not
    bool            getPositionData(const QString input, int &row, int &col);
};
