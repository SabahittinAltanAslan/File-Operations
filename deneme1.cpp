#include <iostream>
#include <fstream>
#include <locale.h>
using namespace std;

int main()
{
    setlocale(LC_ALL, "Turkish");
    string dosyaYolu;
    cout << "L�tfen Bir Dosya Yolu Giriniz:";
    cin >> dosyaYolu;
    ifstream file(dosyaYolu.c_str(), ios::binary);
    if (file.is_open()) {
        char x[2];
        file.read(x, 2);
        cout << "Yolunu verdi�iniz dosyan�n ilk iki bayt�: " << x[0] << x[1] << endl;
    }
    else {
        cout << "Dosya yolu yanl�� veya dosya bulunamad�" << endl;
    }
    return 0;
}

