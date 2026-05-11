#ifndef BAGITEM_H
#define BAGITEM_H

#include <QString>

struct BagItem
{
    QString name;
    QString description;
    int slotCount;
    double maxWeight;
    bool isValid;
    QString errorField;

    BagItem()
        : slotCount(0)
        , maxWeight(0.0)
        , isValid(true)
    {
    }

    bool validate()
    {
        isValid = true;
        errorField.clear();

        if (name.isEmpty())
        {
            isValid = false;
            errorField = QString::fromUtf8("название");
            return false;
        }

        if (description.isEmpty())
        {
            isValid = false;
            errorField = QString::fromUtf8("описание");
            return false;
        }

        if (slotCount <= 0)
        {
            isValid = false;
            errorField = QString::fromUtf8("кол-во слотов");
            return false;
        }

        if (maxWeight <= 0.0)
        {
            isValid = false;
            errorField = QString::fromUtf8("максимальный вес");
            return false;
        }

        return true;
    }
};

#endif // BAGITEM_H
