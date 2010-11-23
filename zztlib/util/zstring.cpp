
#include <cstdlib>
#include <cerrno>
#include <algorithm>
#include <string>
#include "zstring.h"

void ZString::toUpper()
{
  std::transform(begin(), end(), begin(), ::toupper);
}

ZString ZString::upper() const
{
  ZString next;
  std::transform(begin(), end(), std::back_inserter( next ), ::toupper);
  return next;
}

void ZString::toLower()
{
  std::transform(begin(), end(), begin(), ::tolower);
}

ZString ZString::lower() const
{
  ZString next;
  std::transform(begin(), end(), std::back_inserter( next ), ::tolower);
  return next;
}

unsigned int ZString::uint( bool *error, int base ) const
{
  unsigned long int x = strtoul( c_str(), 0, base );
  if ( error ) {
    *error = ( errno == ERANGE ) ? true : false;
  }
  return x;
}

int ZString::sint( bool *error, int base ) const
{
  long int x = strtol( c_str(), 0, base );
  if ( error ) {
    *error = ( errno == ERANGE ) ? true : false;
  }
  return x;
}

signed short ZString::word( bool *error, int base ) const
{
  return sint( error, base );
}

unsigned char ZString::byte( bool *error, int base ) const
{
  return uint( error, base );
}

