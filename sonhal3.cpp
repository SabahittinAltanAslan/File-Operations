#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <windows.h>
//dosyanýn bölüm adýný ,offsetini ,boyutunu saklar
struct SectionInfo {
    std::string name;
    size_t offset;
    size_t size;
};

//Verdiðimiz dizindeki klasörün içi boþ mu diye kontrol eder boþsa hata mesajý verir.
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
            std::ifstream file(entry.path(), std::ios::binary);//dosyayý ikili(binary)modda açar
            if (file.is_open()) {
                char x[2];
                file.read(x, 2);
                if (x[0] != 'M' || x[1] != 'Z') {//ilk iki baytý okur koontrool eder
                    std::cout << entry.path() << "Gecersiz PE dosyasi"   << std::endl;
                    file.close();
                    continue;
                }
                file.seekg(0, std::ios::end);//dosya konumunu sona alýr
                std::streampos fileSize = file.tellg();//sona kadar gittiði yerlerin(dosyanýn tamamý)boyutlarýný kaydeder
                file.seekg(0, std::ios::beg);//tekrardan dosya konumunu baþa getirir.
                std::vector<char> buffer(fileSize);//buffer oluþturulur dosya boyutu kadar bellekte yer ayrýlýr.
                file.read(buffer.data(), fileSize);//dosyanýn içeriði buffera vector aracýlýðýyla okunur.
                file.close();//dosya kapatýlýr.
                std::string content(buffer.data(), fileSize);//kalýcý string deðiþken(content)'e bilgiler kaydedilir.

                PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(buffer.data());//kontrol kodlarý için hazýr kodlar kendim entegre ettim kodlarýma
                PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(&buffer[dosHeader->e_lfanew]);

                // Signature alanýnýn doðrulanmasý
                if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
                    std::cout << entry.path() << "Gecersiz PE dosyasi"  << std::endl;
                    continue;
                }
                //Cihazýn destek kontrolü
                WORD machineType = ntHeader->FileHeader.Machine;
                if (machineType != IMAGE_FILE_MACHINE_I386 && machineType != IMAGE_FILE_MACHINE_AMD64) {
                     
					std::cout << entry.path() << ": Desteklenmeyen makine türü" << std::endl;
                    continue;
        }
                
                //sectionlarý dolaþýr ve tüm bilgileri SectionInfo'da tutar. 
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
                    	//bulunan malwarenin tüm bilgileri ekrana yazdýrýlýr.(yol,sectionu)
                        if (offset >= section.offset && offset < (section.offset + section.size)) {
                            std::cout << entry.path() << ": MALWARE BULUNDU (" << section.offset << " adresinde ve \"" << section.name << "\" section'inda)" << std::endl;
                            break;
                        }
                    }
                } else {
                	//bulunamadýysa bu bilgiyi kullanýcýyla paylaþýr konum bilgisi ile birlikte
                    std::cout << entry.path() << ": MALWARE BULUNAMADI" << std::endl;
                }
            } else {//dosya açýlamadýysa dosya bulunamadý hatasýný ekrana yazdýrýr.
                std::cout << entry.path() << ": Dosya bulunamadi." << std::endl;
            }
        }
    }
}

int main() {
	//kullanýcýdan bir dosya yolu alýnr ve fonksiyonlarýmýz sýrasýyla kontrollere baþlar.
    std::cout << "Lutfen dizin yolu giriniz: ";
    std::string dizin_yolu;
    std::cin >> dizin_yolu;
    checkFolder(dizin_yolu);
    searchFiles(dizin_yolu);
   

    return 0;
}

