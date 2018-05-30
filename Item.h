//! @author     Marcel Petrick; mail@marcelpetrick.it
//! @license    GNU GENERAL PUBLIC LICENSE Version 3
//! @version    0.02
//! @date       20180530
//!
//! @file   "Item.h"
//! @brief   Expanded struct for keeping the type and the original full string of the parsed line (string).

#pragma once

//Qt-includes
#include <QDebug>

class Item
{
public:

    //the type - also needed for outside checks
    enum ItemType
    {
        ItemType_unknown = 0,   //default value - if everything fails, then check against this
        ItemType_regular,   //all other types
        ItemType_GridBegin, //grid start
        ItemType_GridEnd,   //grid end
        ItemType_ItemBegin, //item start
        ItemType_ItemEnd,   //item end
        ItemType_GridCombined, //used to save the combined grid-block: after reduction
        ItemType_ItemCombined, //used to save the combined item-block: after reduction
        ItemType_count //! @attention Should be always the last entry!
    };

//members
    ItemType     type;
    QString      workingString;

//functions
    //default ctor for init
    Item() :
        type(ItemType_unknown),
        workingString(QString())
    {
//        qDebug() << "Item used default ctor!" << endl;
    }

    //default ctor for init
    Item(ItemType inType, QString inQString) :
        type(inType),
        workingString(inQString)
    {
//        qDebug() << "Item used param-ctor!" << endl;
    }
};
