#include "tvdb.h"


QtTvDB::Mirrors *TvDB::mirrors_ = NULL;

QtTvDB::Mirrors *TvDB::mirrors()
{
  if (!mirrors_)
    mirrors_ = new QtTvDB::Mirrors();
  return mirrors_;
}
