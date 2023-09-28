#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <windows.h>
//dosyan�n b�l�m ad�n� ,offsetini ,boyutunu saklar
struct SectionInfo {
    std::string name;
    size_t offset;
    size_t size;
};

//Verdi�imiz dizindeki klas�r�n i�i bo� mu diye kontrol eder bo�sa hata mesaj� verir.
void checkFolder(const std::filesystem::path& folderPath) {
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
}


void searchFiles(const std::string& directory) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path(), std::ios::binary);//dosyay� ikili(binary)modda a�ar
            if (file.is_open()) {
                char x[2];
                file.read(x, 2);
                if (x[0] != 'M' || x[1] != 'Z') {//ilk iki bayt� okur koontrool eder
                    std::cout << entry.path() << "Gecersiz PE dosyasi"   << std::endl;
                    file.close();
                    continue;
                }
                file.seekg(0, std::ios::end);//dosya konumunu sona al�r
                std::streampos fileSize = file.tellg();//sona kadar gitti�i yerlerin(dosyan�n tamam�)boyutlar�n� kaydeder
                file.seekg(0, std::ios::beg);//tekrardan dosya konumunu ba�a getirir.
                std::vector<char> buffer(fileSize);//buffer olu�turulur dosya boyutu kadar bellekte yer ayr�l�r.
                file.read(buffer.data(), fileSize);//dosyan�n i�eri�i buffera vector arac�l���yla okunur.
                file.close();//dosya kapat�l�r.
                std::string content(buffer.data(), fileSize);//kal�c� string de�i�ken(content)'e bilgiler kaydedilir.

                PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(buffer.data());//kontrol kodlar� i�in haz�r kodlar kendim entegre ettim kodlar�ma
                PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(&buffer[dosHeader->e_lfanew]);

                // Signature alan�n�n do�rulanmas�
                if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
                    std::cout << entry.path() << "Gecersiz PE dosyasi"  << std::endl;
                    continue;
                }
                //Cihaz�n destek kontrol�
                WORD machineType = ntHeader->FileHeader.Machine;
                if (machineType != IMAGE_FILE_MACHINE_I386 && machineType != IMAGE_FILE_MACHINE_AMD64) {
                     
					std::cout << entry.path() << ": Desteklenmeyen makine t�r�" << std::endl;
                    continue;
        }
                
                //sectionlar� dola��r ve t�m bilgileri SectionInfo'da tutar. 
                PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeader);
                std::vector<SectionInfo> sections;

                for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i) {
                    SectionInfo section;
                    section.name = reinterpret_cast<const char*>(sectionHeader->Name);
                    section.offset = sectionHeader->VirtualAddress;
                    section.size = sectionHeader->Misc.VirtualSize;
                    sections.push_back(section);
                    ++sectionHeader;
                }
                //Malware kelimesini arar
                size_t offset = content.find("MALWARE");
    
                if (offset != std::string::npos) {
                    for (const auto& section : sections) {
                    	//bulunan malwarenin t�m bilgileri ekrana yazd�r�l�r.(yol,sectionu)
                        if (offset >= section.offset && offset < (section.offset + section.size)) {
                            std::cout << entry.path() << ": MALWARE BULUNDU (" << section.offset << " adresinde ve \"" << section.name << "\" section'inda)" << std::endl;
                            break;
                        }
                    }
                } else {
                	//bulunamad�ysa bu bilgiyi kullan�c�yla payla��r konum bilgisi ile birlikte
                    std::cout << entry.path() << ": MALWARE BULUNAMADI" << std::endl;
                }
            } else {//dosya a��lamad�ysa dosya bulunamad� hatas�n� ekrana yazd�r�r.
                std::cout << entry.path() << ": Dosya bulunamadi." << std::endl;
            }
        }
    }
}

int main() {
	//kullan�c�dan bir dosya yolu al�nr ve fonksiyonlar�m�z s�ras�yla kontrollere ba�lar.
    std::cout << "Lutfen dizin yolu giriniz: ";
    std::string dizin_yolu;
    std::cin >> dizin_yolu;
    checkFolder(dizin_yolu);
    searchFiles(dizin_yolu);
   

    return 0;
}

