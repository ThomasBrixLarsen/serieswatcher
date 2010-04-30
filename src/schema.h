/*
 * Copyright (C) 2010 Corentin Chary <corentin.chary@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef SCHEMA_H
# define SCHEMA_H

static const char *schema =
"CREATE TABLE  IF NOT EXISTS episodes ("
"    id INTEGER PRIMARY KEY ON CONFLICT REPLACE,"
"    showId INTEGER,"
"    seasonId INTEGER,"
"    name TEXT,"
"    overview TEXT,"
"    season INTEGER,"
"    episode INTEGER,"
"    director TEXT,"
"    guestStars TEXT,"
"    language TEXT,"
"    productionCode TEXT,"
"    rating TEXT,"
"    writer TEXT,"
"    firstAired INTEGER,"
"    dvdChapter TEXT,"
"    dvdDiscId TEXT,"
"    dvdEpisodeNumber TEXT,"
"    dvdSeason TEXT,"
"    image TEXT,"
"    airsAfterSeason TEXT,"
"    airsBeforeSeason TEXT,"
"    airsBeforeEpisode TEXT,"
"    combinedEpisode INTEGER,"
"    combinedSeason INTEGER,"
"    absoluteNumber INTEGER,"
"    epImgFlag TEXT,"
"    imdbId TEXT,"
"    lastUpdated INTEGER"
");"
"CREATE INDEX IF NOT EXISTS showSeasonId on episodes ("
"     showId ASC,"
"     seasonId ASC);"
"CREATE TABLE  IF NOT EXISTS shows ("
"    id INTEGER PRIMARY KEY ON CONFLICT REPLACE,"
"    name TEXT,"
"    overview TEXT,"
"    genre TEXT,"
"    actors TEXT,"
"    network TEXT,"
"    contentRating TEXT,"
"    rating TEXT,"
"    runtime TEXT,"
"    status TEXT,"
"    language TEXT,"
"    firstAired INTEGER,"
"    airsDay TEXT,"
"    airsTime TEXT,"
"    banner TEXT,"
"    poster TEXT,"
"    fanArt TEXT,"
"    imdbId TEXT,"
"    seriesId TEXT,"
"    zap2itId TEXT,"
"    lastUpdated INTEGER"
");"
"CREATE TABLE IF NOT EXISTS episodes_extra ("
"    id INTEGER PRIMARY KEY ON CONFLICT REPLACE,"
"    watched INTEGER DEFAULT (0)"
");"
"CREATE TABLE IF NOT EXISTS banners ("
"    id INTEGER PRIMARY KEY ON CONFLICT REPLACE,"
"    showId INTEGER,"
"    path TEXT,"
"    type TEXT,"
"    type2 TEXT,"
"    language TEXT,"
"     season TEXT );"
"CREATE INDEX IF NOT EXISTS showId on banners (showId ASC)";

#endif
