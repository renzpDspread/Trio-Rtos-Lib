#ifndef BASE_H
#define BASE_H

#include <QApplication>

extern void* std_malloc(int len);
extern void  std_free(void* ptr);
extern char std_s_asc_bcd(char c, int t);
extern int std_bcd_2_asc(char* bcd, int bcd_len, char* asc);
extern int std_asc_2_bcd(char* asc, int asc_len, char* bcd);
extern void std_char_opposite_endian(char* data, int len);
extern void trace_value(char* title, char* hex, int len);
extern QString fileMd5(const QString &path, qint64 offset);


#endif // BASE_H
