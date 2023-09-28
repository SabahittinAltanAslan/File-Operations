#include <iostream>
#include <fstream>
#include <locale.h>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

int main() {
    setlocale(LC_ALL, "Turkish");
    string dosya_yolu;
    cout << "Dosya yolunu girin: ";
    cin >> dosya_yolu;

        ifstream file(dosya_yolu.c_str());
        if (file.is_open()) {
            string metin((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            string arananKelime = "MALWARE";
            size_t index = metin.find(arananKelime);
            if (index != string::npos) {
                cout << '"' << arananKelime << "\" ifadesi " << index << " adresinde bulundu." << endl;
            } else {
                cout << '"' <<  arananKelime << "\" ifadesi herhangi bir konumda bulunamadý." << endl;
            }
            file.close();
        } else {
            cout << "Dosya açýlamadý." << endl;
        }
     

    return 0;
}

