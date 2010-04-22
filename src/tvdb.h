#ifndef TVDB_H
# define TVDB_H

#include <QtTvDB>

class TvDB {
public:
  static QtTvDB::Mirrors *mirrors();
private:
  static QtTvDB::Mirrors *mirrors_;
};

#endif
