#ifndef KEYBOARDUSBID_H
#define KEYBOARDUSBID_H

#include <QDebug>
struct KeyboardUSBID
{
    unsigned short vid;
    unsigned short pid;

    bool operator==(const KeyboardUSBID& other) const;
};

QDebug& operator<<(QDebug& debug, const KeyboardUSBID& id);

#endif // KEYBOARDUSBID_H
