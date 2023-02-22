#ifndef KEYBOARDUSBID_H
#define KEYBOARDUSBID_H

struct KeyboardUSBID
{
    unsigned short vid;
    unsigned short pid;

    bool operator==(const KeyboardUSBID& other) const;
};

#endif // KEYBOARDUSBID_H
