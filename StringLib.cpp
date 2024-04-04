#include "StringLib.h"

IStringBuilder::IStringBuilder(char *stringBuffer, uint16_t bufferSize)
{
    stringBuf = stringBuffer;
    bufSize = bufferSize;
    clear();
}

// Clears the string buffer.
void IStringBuilder::clear()
{
    *stringBuf = 0;
    endIdx = 0;
}

// Appends a char to the string buffer.
bool IStringBuilder::append(char chr)
{
    if (endIdx < bufSize - 1)
    {
        if (chr != 0)
        {
            stringBuf [endIdx++] = chr;
            stringBuf [endIdx] = 0;
        }
        return true;
    }
    return false;
}

// Appends a string to the string buffer.
bool IStringBuilder::append (const char *str)
{
    bool writeCompleted = true;

    if (str) while (*str)
    {
        if (!append(*str))
        {
            writeCompleted = false;
            break;
        }
        str++;
    }
    return writeCompleted;
}

PROGMEM const char _HexStr[] = "0123456789ABCDEF";

// Appends the byte as a hex number.
bool IStringBuilder::appendHex(uint8_t hexByte)
{  
  char chrHi = pgm_read_byte(&_HexStr[(hexByte >> 4) & 15]);
  char chrLo = pgm_read_byte(&_HexStr[hexByte & 15]);
  return append(chrHi) && append(chrLo);
}

bool IStringBuilder::appendInt (int32_t num)
{
    char buf[11];

    ltoa(num, buf, 10);
    return append(buf);
}


// Appends a string in program memory to the string buffer.
bool IStringBuilder::append_P (const char *str)
{
    bool writeCompleted = true;

    if (!str) return true;

    char chr = pgm_read_byte(str++);

    while (chr)
    {
        if (!append(chr))
        {
            writeCompleted = false;
            break;
        }
        chr = pgm_read_byte(str++);
    }
    return writeCompleted;
}

// Trims the end of the string where any of the ending chars are in the trim array.
void IStringBuilder::trimEnd(const char *trimCharArray)
{
    while (endIdx > 0)
    {
        if (strchr(trimCharArray, stringBuf [endIdx -1 ]))
        {
            endIdx--;
            stringBuf [endIdx] = 0;
            continue;
        }
        break;
    }
}

// Returns true if string ends with suffix, false otherwise.
bool IStringBuilder::endsWith(const char *suffix) const
{
    if(suffix == NULL)
    {
        return false;
    }

    uint16_t suffix_len = strlen(suffix);

    if(suffix_len > length())
    {
        return false;
    }

    return (strncmp( stringBuf + length() - suffix_len, suffix, suffix_len ) == 0);
}

// Returns true if string starts with prexif, false otherwise.
bool IStringBuilder::startsWith(const char *prefix) const
{
    return (strncmp(stringBuf, prefix, strlen(prefix)) == 0);
}

// Returns true if there is capacity for wrting more chars to the string buffer, false otherwise.
bool IStringBuilder::hasCapacity() const
{
    return endIdx < bufSize;
}

// Returns the length of the string in the buffer.
uint16_t IStringBuilder::length() const
{
    return endIdx;
}

// Re-calculates string length where the internal string buffer has been 
// populated externally.
void IStringBuilder::reCalcLength()
{
    stringBuf[bufSize-1] = 0;
    endIdx = strlen(stringBuf);
}

// Returns pointer to internal string buffer. If contents
// of internal buffer are modified externally, must call reCalcLength().
char *IStringBuilder::toString() const
{
    return stringBuf;
}

/* Inserts a string at a point in the string buffer.*/
void IStringBuilder::insertAt(uint16_t pos, const char *str)
{
    if (pos > length())             // inserting after end of string not possible.
    {
        return;
    }
    if (pos == length())            // inserting at end of string, so just write as normal.
    {
        append (str);
        return;
    }

    uint16_t len = strlen(str);

    if (pos + len >= bufSize )      // existing string will be displaced completely, so reset
    {                               // current end index and write the new string.
        endIdx = pos;
        stringBuf [endIdx] = 0;
        append (str);
        return;
    }

    uint16_t moveLen = length() - pos + 1;
    
    if (len + length() >= bufSize)  // resulting string will exceed buffer size,
    {                               // so reduce move length to fit withing buffer.
        moveLen -= (len + length() - bufSize +1);
        endIdx = bufSize -1;
    }
    else
    {
        endIdx += len;              // must be careful changing endIdx, because length()
    }                               // depends on it to calculate the string length.
    
    memmove(stringBuf + pos + len, stringBuf + pos, moveLen);
    strncpy(stringBuf + pos, str, len);

    stringBuf [endIdx] = 0;
}


uint16_t IStringBuilder::maxCapacity() const
{
    return bufSize;
}

IStringBuilder::operator const char*() const
{
    return toString();
}

//-----------------------------------------------------------------------------

// Initialise the string reader with a string.
StringReader::StringReader(char *stringBuffer)
{
    if (stringBuffer == NULL)
    {
    endPtr = (char *)0;
    stringPtr = (char *)1;
    }
    else
    {
    reset(stringBuffer);
    }
}

// Re-sets the string reader buffer with a new string.
void StringReader::reset(char *stringBuffer)
{
    stringPtr = stringBuffer;
    endPtr = stringPtr + strlen(stringBuffer);
}

// Returns the next char in the buffer without advancing the reader
// Will also return the string terminating char \0. 
char StringReader::peekChar () const
{
    if (stringPtr <= endPtr)
    {
        return *stringPtr;
    }
    else
    {
        return 0;
    }
}

// Returns the remaining string without advancing the reader.
char *StringReader::peekString() const
{
    if (stringPtr <= endPtr)
    {
        return stringPtr;
    }
    else
    {
        return 0;
    }
}

// Returns the next char and advances the reader.
// Will also return the string terminating char \0. 
char StringReader::readChar()
{
    char chr = *stringPtr;

    if (stringPtr <= endPtr)
    {
        stringPtr++;
        return chr;
    }
    return 0;
}

// Returns the remaining portion of the string, and advances
// the reader to the end of the string.
char *StringReader::readToEnd()
{
    if (stringPtr <= endPtr)
    {
        char *str = stringPtr;

        stringPtr = endPtr + 1;
        return str;
    }
    return NULL;
}

// Returns a portion of the string up to the delimiter, and advances
// the reader to a point immediately after the delimeter.
char *StringReader::readUntil(const char *delim)
{
    if (stringPtr <= endPtr)
    {
        char *str = stringPtr;
        char *delimPtr = strstr(str, delim);

        if (delimPtr)
        {
            *delimPtr = 0;
            uint16_t fieldLen = delimPtr != NULL ? delimPtr - str : endPtr - str;

            stringPtr += fieldLen + (delimPtr != NULL ? strlen(delim) : 0);
        }
        else
        {
            stringPtr = endPtr + 1;
        }
        return str;
    }
    return NULL;
}

// Returns true if there are more chars to read.
bool StringReader::available() const
{
    return stringPtr <= endPtr;
}
