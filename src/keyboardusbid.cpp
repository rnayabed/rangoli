#include "keyboardusbid.h"

bool KeyboardUSBID::operator==(const KeyboardUSBID& other) const
{
    return vid == other.vid && pid == other.pid;
}
