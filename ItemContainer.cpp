//! @author Marcel Petrick; mail@marcelpetrick.it; copyright: currently all rights reserved
//! @version    0.01 unreleased
//! @date   20150408
//!
//! @file   "ItemContainer.cpp"
//! @brief   Keeps the "list" of all line-entries: with type and cleaned "full" string

#include "ItemContainer.h"

ItemContainer::ItemContainer(QObject* parent) :
    QObject(parent),
    mList(QList<Item>())
{
//    qDebug() << "ctor Itemcontainer! created empty list!" << endl;
}

bool ItemContainer::addItem(QString input)
{
//    qDebug() << "addItem: " << input << endl;
    bool returnValue(false);

    //! determine type!
    Item newItem(this->determineType(input), input); //create new item

    mList.append(newItem); //insert
    returnValue = true;

    return returnValue;
}

//! @brief  Return the current content as newline-separated string.
QString ItemContainer::getCurrentContent()
{
    QString returnValue;

    unsigned int lineNumber(1);
    foreach(const Item item, this->mList)
    {
        //! @todo add a 000-prefix based on the size of the list for the linenumber!
//        returnValue.append(QString::number(lineNumber) + " # " + this->getItemTypeString(item.type) + " #  " + item.workingString + "\n");

        //print just for debugging the content
        returnValue.append(item.workingString + "\n");
        lineNumber++;
    }

    return returnValue;
}

//! @brief  Helper to determine the type of the token.
//!         Currently only needed types are defined. All else will return ItemType_regular.
Item::ItemType ItemContainer::determineType(QString input)
{
    //define different classes which are recognized based on the given prefix of the string
    QString const itemBegin("<item"); //can be <item> and <item row=
    QString const itemEnd("</item>");
    QString const layoutBegin("<layout class=\""); //layoutBegin("<layout class=\"QGridLayout\"");
    QString const layoutEnd("</layout>");

    //determine the type
    Item::ItemType returnValue(Item::ItemType_unknown);

    if(input.left(itemBegin.size()) == itemBegin)
    {
//        qDebug() << "found: " << "itemBegin";
        returnValue = Item::ItemType_ItemBegin;
    }
    else if(input.left(itemEnd.size()) == itemEnd)
    {
//        qDebug() << "found: " << "itemEnd";
        returnValue = Item::ItemType_ItemEnd;
    }
    else if(input.left(layoutBegin.size()) == layoutBegin)
    {
//        qDebug() << "found: " << "layoutBegin";
        returnValue = Item::ItemType_GridBegin;
    }
    else if(input.left(layoutEnd.size()) == layoutEnd)
    {
//        qDebug() << "found: " << "layoutEnd";
        returnValue = Item::ItemType_GridEnd;
    }
    else
    {
//        qDebug() << "found: " << ".. regular, nothing special";
        returnValue = Item::ItemType_regular;
    }

    return returnValue;
}

//! @brief  Just convert the enum to a QString.
QString ItemContainer::getItemTypeString(Item::ItemType input)
{
    QString returnValue;

    switch(input)
    {
    case Item::ItemType_unknown:
        returnValue = "ItemType_unknown"; break;
    case Item::ItemType_regular:
        returnValue = "ItemType_regular"; break;
    case Item::ItemType_GridBegin:
        returnValue = "ItemType_GridBegin"; break;
    case Item::ItemType_GridEnd:
        returnValue = "ItemType_GridEnd"; break;
    case Item::ItemType_ItemBegin:
        returnValue = "ItemType_ItemBegin"; break;
    case Item::ItemType_ItemEnd:
        returnValue = "ItemType_ItemEnd"; break;
    case Item::ItemType_ItemCombined:
        returnValue = "ItemType_ItemCombined"; break;
    case Item::ItemType_GridCombined:
        returnValue = "ItemType_GridCombined"; break;
    case Item::ItemType_count:
        returnValue = "ItemType_count"; break;
        //! @todo add every fucking new item-type!
    default:
        returnValue = "BIG FUCKING ERROR! CHECK ItemContainer::getItemTypeString! ADD new conversions!"; break;
    }

    return returnValue;
}

//! @brief  Return the current size of the content (list).
unsigned int ItemContainer::size()
{
    return (this->mList.size());
}

//! @brief  Return if the container is currently filled with some entries.
bool ItemContainer::isEmpty()
{
    return (this->mList.isEmpty());
}

//! @brief  Return the currently first item (head) of the container. Delete it afterwards.
//!         Error-case is determined by returning a default "Item" with type "unknown".
Item ItemContainer::pop()
{
    Item returnValue;

    if(this->isEmpty())
    {
        qDebug() << "ItemContainer::pop(): stopped because the list was already empty!";
    }
    else
    {
        returnValue = this->mList.first(); //get
        this->mList.removeFirst(); //delete
    }

    return returnValue;
}

//! @brief  Not a real stack: therefore pushed to the end.
//!         This easifies using the "un-mirrored" production-rules.
void ItemContainer::push(Item input)
{
    this->mList.append(input);
}

//! @brief  Return the type of the last item; if empty list, then return typeUnknown.
//!for checking if a reduce-step would be applicable
Item::ItemType ItemContainer::getTypeLastItem()
{
    if(!this->isEmpty())
    {
        return (this->mList.at(this->mList.size() - 1).type);
    }

    return Item::ItemType_unknown;
}

//! @brief  For getting the position of the very next item with the given type. Search starts from "end".
//!         Used for the reduction.
//! @attention  return of negative values == ERROR!
int ItemContainer::posClosestFromEnd(Item::ItemType type)
{
    unsigned int returnValue(-1);

    //! @todo maybe use lastIndexOf() ..
    int pos(this->mList.size() - 1);
    while(pos >= 0)
    {
        if(this->mList.at(pos).type == type)
        {
            break;
        }

        pos--;
    }

    if(pos >= 0)
    {
        returnValue = pos;
    }
    else
    {
        qDebug() << "ItemContainer::posClosestFromEnd: did not find search-value of " << this->getItemTypeString(type);
    }

    return returnValue;
}

//! @brief   todo: grab the content of begin to end and combine it into a single string. assign the given type. no checks so far ..
bool ItemContainer::reduce(unsigned int begin, unsigned int end, Item::ItemType replacementType)
{
    bool returnValue(false);
    QString combinedString;

    if(begin <= end)
    {
        //! @attention for special cases like the grid-layout: check if certain replacement-type; then sort the crap; then combine
        //! @attention for regular cases: like <item> - just combine
        //exclude also those layouts which are not of type grid
        if((replacementType == Item::ItemType_GridCombined) && (this->mList.at(begin).workingString.contains("QGridLayout")))
        {
            qDebug() << "ItemContainer::reduce: ItemType_GridCombined - do the magic (with sorting!)";

            //idea: since the input should be assumed "sorted and uniq", then every item exists just once
            //so search for the current iter: add it to the string. Then find the next biggest value; rinse & repeat

//            { //! just for debugging: give current content from here to end. Result is exactly like foreseen: so lets cut of the part of the container and reduce it!
//                QString debugCrap;
//                for(int iter(begin); iter < (this->mList.size()); iter++)
//                {
//                    debugCrap.append(this->mList.at(iter).workingString).append("##");
//                }
//                qDebug() << debugCrap;

//                //check now if cutOff(..) works
//                qDebug() << "#########################";
//                qDebug() << "old size: " << QString::number(this->size());
//                ItemContainer* test(this->cutOff(begin, end));
//                qDebug() << "new size: " << QString::number(this->size())  << " ## temp size: " << QString::number(test->size());
//                qDebug() << "#########################";
//                qDebug() << test->getCurrentContent();
//            } //end of debugging: output should show that all the stuff which is needed for reduction is contained ...

            //! idea is the following: cut of the part; then reduce it sorted ascendingly; then insert the returned string as new item with the given type
            ItemContainer* cutPart(this->cutOff(begin, end)); //create new container
            combinedString = cutPart->reduceSorted(); //get the stuff
            delete cutPart; //get rid of the temporary container!

            qDebug() << "combinedString: " << combinedString;
        }
        else
        {
            //! collect: from end to begin
            //! Usable for all types of layouts and item-types
            for(unsigned int iter(end); iter >= begin; iter--)
            {
                combinedString.prepend(this->mList.at(iter).workingString);
                this->mList.removeAt(iter);
            }
        }

        //! check and break! in case something went wrong
        if(combinedString.isEmpty())
        {
            qDebug() << "ItemContainer::reduce: big issue: combined string is empty! ERROR!";
        }

        //! insert a new one at the end
        Item newItem(replacementType, combinedString);
        this->mList.append(newItem);

        returnValue = true;
    }
    else
    {
        // else: simplest check already failed!
        combinedString.append("big fucking ERROR! check ItemContainer::reduce!");
    }

    return returnValue;
}

//! @brief  Move all then content inside the given markers to the returned new container and remove them from the original container (which is 'us').
ItemContainer* ItemContainer::cutOff(unsigned int begin, unsigned int end)
{
    qDebug() << "ItemContainer::cutOff: called with " << begin << " to " << end; //! @todo remove
    ItemContainer* returnValue(new ItemContainer());

    //check first if both positions are valid ones
    if(this->size() <= begin || this->size() <= end || begin > end)
    {
        qDebug() << "ItemContainer::cutOff: something is wrong with the indices! ERROR!";
        return returnValue; //early return; maybe break
    }

    //first collect all of them ..
    for(unsigned int iter(begin); iter <= end; iter++) //! smaller or equal to the end?
    {
        Item const temp(this->mList.at(iter)); //get
        returnValue->push(temp); //insert
    }

    //.. then discard the old stuff. Doing it directly would lead to a need for an adjustment of the indices (because of removed elements)
    // do this from end to begin, because else it would delete just (e. g.) 4 of 6 elements
    for(unsigned int iter(end); iter >= begin; iter--)
    {
        this->mList.removeAt(iter); //remove original
    }


    return returnValue;
}

//! @brief  Reduce in a sorted way: should be just called for the gridlayout-type!
//!  @returns   QString all items combined a single string.
QString ItemContainer::reduceSorted()
{
    //! maybe add some checks if first and last item fit ... maybe not ;)
    QString combinedString;

    //to simplify the handling: cut of head and tail first
    unsigned int startPos(0); //first
    QString const head(this->mList.at(startPos).workingString);
    this->mList.removeAt(startPos);
    startPos = this->size() - 1; //last
    QString const tail(this->mList.at(startPos).workingString);
    this->mList.removeAt(startPos);
    qDebug() << "head: " << head; //! @todo remove
    qDebug() << "tail: " << tail;

    while(!this->mList.isEmpty()) //repeat over and over until it runs out of usable elements
    {
        //find the smallest number bigger than these temporarily MAX-initialised values
        int pos(-1);
        int tempRow(1234567890); //init with biggest values
        int tempCol(1234567890);
        bool atLeastOneCandidateWasFound(false);

        for(int iter(0); iter < (this->mList.size()); iter++) //for ALL
        {
            int currRow(-1);
            int currCol(-1);
            //check for the current row/col-data
            bool const success =  this->getPositionData(this->mList.at(iter).workingString, currRow, currCol); //use the modified +1-start
            qDebug() << QString(success ? "SUCCESS!" : "FAILURE! pos = ") << pos << " # found row : col = " << currRow << " : " << currCol;

            if(success) //conversion succeeded
            {
                if( (currRow < tempRow) || //direct hit: it row is smaller, then pair is smaller
                    ((currRow == tempRow) && (currCol < tempCol))) //rows can be equal: but then col has to decide
                {
                    qDebug() << " exchanged : " << tempRow << "," << tempCol << " with " << currRow << "," << currCol; //! @todo remove
                    pos = iter;
                    tempRow = currRow;
                    tempCol = currCol;
                    atLeastOneCandidateWasFound |= true; //once true, always true!
                }
            }
//            else
//            {
//                qDebug() << "else-case: just inserted for debugger";
//            }

//            qDebug() << "pos is now " << pos;
        }

        qDebug() << QString(atLeastOneCandidateWasFound ? "atLeastOneCandidateWasFound SUCCESS!" : "atLeastOneCandidateWasFound FAILURE! pos = ") << pos << " # found row : col = " << tempRow << " : " << tempCol;

        if(atLeastOneCandidateWasFound)
        {
            qDebug() << "pos with lowest col/row is: " << pos << " at "<< tempRow << " : " << tempCol << " # " << this->mList.at(pos).workingString;

            //add it to the current string; remove it from the original list
            combinedString.append(this->mList.at(pos).workingString);
            //somehow the Qt-Designer needs a newline for new items ... else their parsing fails
            //! @todo realize this by using the original strings with attached newline!
            if(this->size() > 1)
            {
                combinedString.append("\n");
            }
            this->mList.removeAt(pos);
        }
        else
        {
            qDebug() << "found nothing: reduce now the reamining stuff into a string and this will lead to skipping the loop; remaining items: " << this->mList.size();

            //! @todo copy-cat: fix this
            //! collect: from end to begin
            //! Usable for all types of layouts and item-types.
            QString gatheredRemains;
            while(!this->mList.isEmpty())
            {
                gatheredRemains.append(this->mList.at(0).workingString);
                this->mList.removeAt(0);
            }
            qDebug() << "gatheredRemains (which will be added): " << gatheredRemains;
            combinedString.append("\n").append(gatheredRemains);
        }

    } //! end of the lopp which should take over the given string from position "pos" and combine them

    //add head and tail
    combinedString = head + "\n" + combinedString + "\n" + tail;

    return combinedString;
}

//! @brief  Check the row&col-data of the given index; use the returnValue to determine success or not.
//!         Strip the info from a string like "       <item row="12" column="0">"
bool ItemContainer::getPositionData(QString const input, int &row, int &col)
{
    //prepare for the worst case
    bool returnValue(false);
    row = -1; //re-init them
    col = -1;

    //check first if the string has type of <item row=".." col=".." ..>

    int tempValue(-1);
    int from(-1);
    int to(-1);
    QString search;
    //! first check "row"
    search = "row=\"";
    from = input.indexOf(search) + search.size();
    to = input.indexOf("\"", from);
    if((from != -1) && (to != -1) && (from < to))
    {
        bool converted(false);
        tempValue = input.mid(from, to - from).toInt(&converted);

        if(!converted)
        {
//            qDebug() << "next fucking error: row: conversion failed!" << "\'" << input << "\'";
            return returnValue;
        }
        else
        {
            row = tempValue;
        }
    }
    else
    {
//        qDebug() << "something failed while recognizing the posData: row:" << "\'" << input << "\'";
        return returnValue;
    }

    //! then "column" based on the given start
    search = "column=\"";
    from = input.indexOf(search, to) + search.size(); //start from the end of "col"
    to = input.indexOf("\"", from);
    if((from != -1) && (to != -1) && (from < to))
    {
        bool converted(false);
        tempValue = input.mid(from, to - from).toInt(&converted);

        if(!converted)
        {
//            qDebug() << "next fucking error: column: conversion failed!" << "\'" << input << "\'";
            return returnValue;
        }
        else
        {
            col = tempValue;
        }
    }
    else
    {
//        qDebug() << "something failed while recognizing the posData: column: " << "\'" << input << "\'";
        return returnValue;
    }

    //if this location is reached then everything works
    returnValue = true;

    //! todo remove: just for debugging:
    qDebug() << "row col: " << row << " # " << col << " ## " << input;
    if(input.contains("colspan"))
    {
        qDebug() << "\n";
//        qDebug() << "row col: " << row << " # " << col << " ## " << input;
    }

    return returnValue;
}
