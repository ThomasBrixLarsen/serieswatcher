CREATE TABLE  IF NOT EXISTS "episodes" (
    "id" INTEGER PRIMARY KEY,
    "showId" INTEGER,
    "seasonId" INTEGER,
    "name" TEXT,
    "overview" TEXT,
    "season" INTEGER,
    "episode" INTEGER,
    "director" TEXT,
    "guestStars" TEXT,
    "language" TEXT,
    "productionCode" TEXT,
    "rating" TEXT,
    "writer" TEXT,
    "firstAired" INTEGER,
    "dvdChapter" TEXT,
    "dvdDiscId" TEXT,
    "dvdEpisodeNumber" TEXT,
    "dvdSeason" TEXT,
    "image" TEXT,
    "airsAfterSeason" TEXT,
    "airsBeforeSeason" TEXT,
    "airsBeforeEpisode" TEXT,
    "combinedEpisode" INTEGER,
    "combinedSeason" INTEGER,
    "absoluteNumber" INTEGER,
    "epImgFlag" TEXT,
    "imdbId" TEXT,
    "lastUpdated" INTEGER
);
CREATE INDEX IF NOT EXISTS "showSeasonId" on episodes (showId ASC, seasonId ASC);
CREATE TABLE  IF NOT EXISTS "shows" (
    "id" INTEGER PRIMARY KEY,
    "name" TEXT,
    "overview" TEXT,
    "genre" TEXT,
    "actors" TEXT,
    "network" TEXT,
    "contentRating" TEXT,
    "rating" TEXT,
    "runtime" TEXT,
    "status" TEXT,
    "language" TEXT,
    "firstAired" INTEGER,
    "airsDay" TEXT,
    "airsTime" TEXT,
    "banner" TEXT,
    "poster" TEXT,
    "fanArt" TEXT,
    "imdbId" TEXT,
    "seriesId" TEXT,
    "zap2itId" TEXT,
    "lastUpdated" INTEGER
);
CREATE TABLE IF NOT EXISTS "episodes_extra" (
    "id" INTEGER PRIMARY KEY,
    "watched" INTEGER DEFAULT (0)
);
CREATE TABLE IF NOT EXISTS "banners" (
    "id" INTEGER PRIMARY KEY,
    "showId" INTEGER,
    "path" TEXT,
    "type" TEXT,
    "type2" TEXT,
    "language" TEXT,
    "season" TEXT
);
CREATE INDEX "showId" IF NOT EXISTS on banners (showId ASC);
