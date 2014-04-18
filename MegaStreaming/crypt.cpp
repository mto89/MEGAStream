#include "crypt.h"

Crypt::Crypt()
{
}

 // Vigenere's alghoritm
 QString Crypt::crypt(QString text, QString key, QString alphabet)
 {
    QString newKey = key;
    while (newKey.length()<text.length()) {
        newKey = newKey + key;
    }

    QString newText = "";
    int n, u, m, h;

    for (int i = 0; i < text.length(); ++i) {
        for(h = 0; h < alphabet.length(); ++h) {if (newKey[i]==alphabet[h]) n=h;}
        for(m = 0; m < alphabet.length(); ++m) {if (text[i]==alphabet[h]) u=m;}

        newText = newText + alphabet[(u+n)%alphabet.length()];
    }

    return newText;
 }

 QString Crypt::decrypt(QString text, QString key, QString alphabet)
 {
     QString newkey = key;
     while (newkey.length() < text.length()) {
         newkey = newkey + key;
     }
     QString newtext = "";
     int h;
     int m;
     int n;
     int u;
     for (int i=0; i<text.length(); i++){
         for (h = 0; h<alphabet.length()+1;h++) if (newkey[i]==alphabet[h]) n=h;
         for (m = 0; m<alphabet.length()+1;m++) if (text[i]==alphabet[m]) u=m;
         newtext = newtext + alphabet[(alphabet.length()-n+u)%(alphabet.length())];
     }
     return newtext;
 }
