#ifndef CRYPT_H
#define CRYPT_H
#include <QString>

class Crypt
{
public:
    Crypt();
    QString crypt(QString text, QString key, QString alphabet);   // crypt password on saving
    QString decrypt(QString text, QString key, QString alphabet); // decrypt password on loading
};

#endif // CRYPT_H
