#include <locale.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
/*void checkFolder(const std::filesystem::path& folderPath) {
    bool hasFiles = false;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            hasFiles = true;
            break;
        }
    }

    if (!hasFiles) {
        std::cout << "Uyari: " << folderPath.string() << " klasoru bos!" << std::endl;
    }
}*/

void dosyayi_tara(const std::string& dosya_yolu) {
    std::ifstream file(dosya_yolu.c_str());
    if (file.is_open()) {
        std::string metin((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string arananKelime = "MALWARE";
        size_t index = metin.find(arananKelime);
        if (index == std::string::npos) {
            std::cout << '"' << arananKelime << "\" ifadesi " << index << " adresinde bulundu." << std::endl;
        } else {
            std::cout << '"' <<  arananKelime << "\" ifadesi herhangi bir konumda bulunamadi." << std::endl;
        }
        file.close();
    } else {
        std::cout << "Belirtilen dosya bulunamadi." << std::endl;
    }
}

void dizini_tara(const std::string& dizin_yolu) {
    for (const auto& entry : std::filesystem::directory_iterator(dizin_yolu)) {
        if (entry.is_regular_file()) {
            dosyayi_tara(entry.path().string());
        }
    }
}

int main() {
    setlocale(LC_ALL, "Turkish");
    std::string dizin_yolu;
    std::cout << "Dizin yolunu girin: ";
    std::cin >> dizin_yolu;
    //checkFolder(dizin_yolu);
    dizini_tara(dizin_yolu);

    return 0;
}    
