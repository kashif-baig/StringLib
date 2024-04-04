#ifndef STRINGLIB_H
#define STRINGLIB_H

#include <Arduino.h>

// Abstract class for building a string in a string buffer.
class IStringBuilder
{
public:
    // Clears the string buffer.
    void clear();

    // Appends a char to the string buffer.
    bool append(char chr);

    // Appends the byte as a hex number.
    bool appendHex(uint8_t hexByte);

    // Appends an signed 32 bit integer to the string buffer.
    bool appendInt (int32_t num);

    // Appends a string to the string buffer.
    bool append (const char *str);

    // Appends a string in program memory to the string buffer.
    bool append_P(const char *str);

    // Inserts a string at a point in the string buffer.
    void insertAt(uint16_t pos, const char *str);

    // Trims the end of the string where any of the ending chars are in the trim array.
    void trimEnd(const char *trimCharArray);

    // Returns true if string ends with suffix, false otherwise.
    bool endsWith(const char *suffix) const;

    // Returns true if string starts with prefix, false otherwise.
    bool startsWith(const char *prefix) const;
    
    // Returns true if there is capacity for writing more chars to the string buffer, false otherwise.
    bool hasCapacity() const;

    // Returns the length of the string in the buffer.
    uint16_t length() const;

    // Recalculates string length where the internal string buffer has been 
    // populated externally.
    void reCalcLength();

    // Returns pointer to internal string buffer. If contents
    // of internal buffer are modified externally, must call reCalcLength().
    char *toString() const;

    // Returns the size of the internal string buffer. Maximum string
    // length will be one less due to zero terminating byte.
    uint16_t maxCapacity() const;

    operator const char*() const;

protected:
    // Create instance using a buffer with a specified size.
    IStringBuilder(char *stringBuffer, uint16_t bufferSize);

private: 
    char *stringBuf;
    uint16_t bufSize;
    uint16_t endIdx;
};


// String writer for building strings.
template <uint16_t L=17>
class StringBuilder : public IStringBuilder
{
public:
    // Create instance using a buffer with a specified size.
    StringBuilder(): IStringBuilder(stringBuffer, L) { }

private:
    char stringBuffer[L];    
};

// --------------------------------------------------------------------------------


// String reader for reading strings char by char or up to a delimiter.
// Note, this is a destructive reader which replaces the contents of the
// supplied string buffer.
class StringReader
{
public:
    // Initialise the string reader with a string.
    StringReader(char *stringBuffer);

    // Re-sets the string reader buffer with a new string.
    void reset(char *stringBuffer);

    // Returns the next char in the buffer without advancing the reader
    // Will also return the string terminating char \0. 
    char peekChar () const;

    // Returns the remaining string without advancing the reader.
    char *peekString() const;

    // Returns the next char and advances the reader.
    // Will also return the string terminating char \0. 
    char readChar();

    // Returns the remaining portion of the string, and advances
    // the reader to the end of the string.
    char *readToEnd();

    // Returns a portion of the string up to the delimiter, and advances
    // the reader to a point immediately after the delimeter.
    char *readUntil(const char *delim);

    // Returns true if there are more chars to read.
    bool available() const;

private:
    char *stringPtr;
    char *endPtr;
};

#endif
