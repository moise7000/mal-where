//
// Zip Bomb functions
//
#ifndef MAL_WHERE_ZIPBOMB_H
#define MAL_WHERE_ZIPBOMB_H

void create_zipbomb(const char* filename, unsigned long size_mb, int levels);
void detonate_zipbomb(const char* filename, int levels, unsigned long expected_size);

#endif //MAL_WHERE_ZIPBOMB_H
