#include <qstring.h>
#include <bitset>
#include <string.h>
bool GetBit(short n, int i)
  {
   return ((n >> i) & 1);
  }
  short ExtractBits(short n, unsigned int i, unsigned int j)
  {   
      unsigned short shifted = n >> i;
      unsigned short mask = (1 << j) - 1;
      return shifted & mask;
  }
  QString CharToBinaryString(char c)
  {
      // std::bitset<8> automatically formats the character into 8 binary digits
      std::string stdStr = std::bitset<8>(c).to_string();
      return QString::fromUtf8(stdStr.c_str());
  }
  //quint8 myByte = 0x2A; // Binary: 00101010

  // Convert to binary string and pad to 8 characters with '0'
  //QString binaryString = QString::number(myByte, 2).rightJustified(8, '0');

  // Result: "00101010"
  
