#include <stdio.h>      // Standart girdi/çıktı işlemleri için gerekli kütüphane (printf, scanf gibi)
#include <string.h>     // String (dizi) işlemleri için gerekli kütüphane (strcpy, strcmp gibi)
#include <stdbool.h>    // Boolean (true/false) değerler için gerekli kütüphane
#include <stdarg.h>     // Değişken sayıda argüman alan fonksiyonlar için gerekli kütüphane

#define MAX_CALISAN 100  // En fazla 100 çalışan kaydedilebilir.
#define MAX_SATIR 100    // Bir satırda en fazla 100 karakter olabilir.

// Çalışan verilerini tutmak için kullanılan struct yapısı
typedef struct {
    char sicil_no[20];  // Çalışanın sicil numarası (max 20 karakter)
    int saat;           // Giriş/Çıkış saati
    int dakika;         // Giriş/Çıkış dakikası
    char durum;         // Çalışanın giriş ('<') veya çıkış ('>') durumu
} Calisan;

// Fonksiyonların prototipleri (yani isimleri ve ne alıp ne döndürdükleri)
void veri_al_ve_kaydet();               // Kullanıcıdan çalışan verilerini alıp dosyaya kaydeder
void gec_ve_erken_bul();               // Dosyadaki verilere göre en geç ve en erken gelenleri bulur
void tum_calisanlari_goruntule();      // Tüm çalışan verilerini ekrana yazdırır
int saat_dakika_fark(int saat1, int dakika1, int saat2, int dakika2); // İki saat arasındaki farkı dakika cinsinden hesaplar
void dosyaya_yaz(const char *dosya_adi, const char *format, ...); // Verileri belirtilen formatta dosyaya yazar
void gec_ve_erken_dosyaya_yaz(const char *dosya_adi, const char *metin, bool bulundu, const char *sicil, int saat, int dakika, int fark); // Geç/erken verilerini dosyaya yazar
void calisan_kaydini_sil(const char *silinecek_sicil); // Belirtilen sicil numarasına sahip çalışanın kaydını siler
void silinecek_sicil_al_ve_sil();      // Kullanıcıdan silinecek sicil numarasını alıp silme işlemini yapar

// Ana fonksiyon (programın başladığı yer)
int main() {
    int secim; // Kullanıcının menüden seçtiği seçenek
    while (1) { // Sonsuz döngü (kullanıcı çıkış yapana kadar devam eder)
        printf("\n*** Fabrika Giris Sistemi ***\n");
        printf("1. Calisan Giris ve Cikis\n");
        printf("2. Gec ve Erken Gelenler\n");
        printf("3. Tum Calisanlari Goruntule\n");
        printf("4. Calisan Kaydini Sil\n");
        printf("5. Cikis\n");
        printf("Seciminizi yapin: ");
        scanf("%d", &secim); // Kullanıcının seçimi okunur
        while (getchar() != '\n'); // Buffer temizleme (scanf'ten sonraki gereksiz karakterleri temizlemek için)
        switch (secim) { // Kullanıcının seçimine göre işlem yapılır
            case 1:
                veri_al_ve_kaydet(); // Çalışan verilerini alıp kaydetme fonksiyonu çağrılır
                break;
            case 2:
                gec_ve_erken_bul();   // Geç ve erken gelenleri bulma fonksiyonu çağrılır
                break;
            case 3:
                tum_calisanlari_goruntule(); // Tüm çalışanları görüntüleme fonksiyonu çağrılır
                break;
            case 4:
                silinecek_sicil_al_ve_sil();  // Çalışan kaydını silme fonksiyonu çağrılır
                break;
            case 5:
                printf("Programdan cikiliyor...\n");
                return 0; // Programdan çıkış
            default:
                printf("Gecersiz secim! Lutfen tekrar deneyin.\n"); // Geçersiz seçim yapıldığında hata mesajı
        }
    }
}

// Kullanıcıdan çalışan verilerini alıp "gunluk.txt" dosyasına kaydeder
void veri_al_ve_kaydet() {
    int n;
    printf("Kac calisanin verisini girmek istiyorsunuz?: ");
    scanf("%d", &n); // Kaç çalışan verisi girileceği kullanıcıdan alınır
    while (getchar() != '\n'); // Buffer temizleme
    Calisan calisanlar[MAX_CALISAN]; // Çalışan bilgilerini tutacak dizi
    for (int i = 0; i < n; i++) { // Belirtilen sayıda çalışan için döngü
        char sicil_input[MAX_SATIR];
        printf("%d. Calisanin Sicil No: ", i + 1);
        fgets(sicil_input, sizeof(sicil_input), stdin); // Sicil numarası kullanıcıdan alınır
        if(sscanf(sicil_input, "%s", calisanlar[i].sicil_no) != 1){ // Sicil numarası düzgün alınmazsa hata mesajı
            printf("Hatali sicil numarasi girisi!\n");
            i--; // Döngüde bir geri giderek tekrar giriş yapmasını sağlar
            continue;
        }

        printf("Saat (SS DD): ");
        if (scanf("%d %d", &calisanlar[i].saat, &calisanlar[i].dakika) != 2) { // Saat ve dakika bilgileri alınır
            printf("Hatali saat girisi!\n");
            while (getchar() != '\n'); // Hatalı giriş durumunda buffer temizlenir
            i--;
            continue;
        }
        while (getchar() != '\n'); // Buffer temizleme

        printf("Durum (Giris icin '<', Cikis icin '>'): ");
        if(scanf(" %c", &calisanlar[i].durum) != 1){ // Durum bilgisi alınır
            printf("Hatali durum girisi!\n");
            while (getchar() != '\n'); // Hatalı giriş durumunda buffer temizlenir
            i--;
            continue;
        }
        while (getchar() != '\n'); // Buffer temizleme

        // Çalışan bilgileri "gunluk.txt" dosyasına yazılır
        dosyaya_yaz("gunluk.txt", "%s %02d:%02d %c\n", calisanlar[i].sicil_no, calisanlar[i].saat, calisanlar[i].dakika, calisanlar[i].durum);
    }

    printf("Veriler gunluk.txt dosyasina kaydedildi.\n");
}

// "gunluk.txt" dosyasındaki verileri okur ve en geç/en erken gelenleri bulur
void gec_ve_erken_bul() {
    FILE *dosya = fopen("gunluk.txt", "r"); // Dosya okuma modunda açılır
    if (dosya == NULL) {
        printf("gunluk.txt dosyasi bulunamadi!\n"); // Dosya açılamazsa hata mesajı
        return;
    }

    char satir[MAX_SATIR]; // Dosyadan okunan satırlar için dizi
    char en_gec_sicil[20], en_erken_sicil[20]; // En geç ve en erken gelenlerin sicil numaraları
    int en_gec_saat = 9, en_gec_dakika = 0, gecikme = 0; // En geç gelme saati ve dakika
    int en_erken_saat = 17, en_erken_dakika = 0, erken_cikis = 0; // En erken çıkış saati ve dakika
    bool gecikme_durumu = false, erken_bulundu = false; // Geç kalma durumu ve erken çıkış durumu

    while (fgets(satir, sizeof(satir), dosya) != NULL) { // Dosyanın sonuna kadar satır satır okunur
        char sicil_no[20];
        int saat, dakika;
        char durum;
        satir[strcspn(satir, "\n")] = 0; // Satır sonundaki newline karakteri temizlenir
        if (sscanf(satir, "%s %d:%d %c", sicil_no, &saat, &dakika, &durum) == 4) { // Satırdaki bilgiler okunur
            if (durum == '<') { // Giriş durumu kontrol edilir
                int fark = saat_dakika_fark(9, 0, saat, dakika); // Giriş saati ile 9:00 arasındaki fark bulunur
                if (fark > gecikme) { // En geç geleni belirler
                    gecikme = fark;
                    strcpy(en_gec_sicil, sicil_no);
                    en_gec_saat = saat;
                    en_gec_dakika = dakika;
                    gecikme_durumu = true; // Geç kalma durumu true yapılır
                }
            } else if (durum == '>') { // Çıkış durumu kontrol edilir
                int fark = saat_dakika_fark(saat, dakika, 17, 0); // Çıkış saati ile 17:00 arasındaki fark bulunur
                if (fark > erken_cikis) { // En erken çıkanı belirler
                    erken_cikis = fark;
                    strcpy(en_erken_sicil, sicil_no);
                    en_erken_saat = saat;
                    en_erken_dakika = dakika;
                    erken_bulundu = true; // Erken çıkış durumu true yapılır
                }
            }
        } else {
            printf("Hatali satir formati: %s\n", satir); // Hatalı satır formatı varsa yazdırır
        }
    }

    fclose(dosya); // Dosya kapatılır

    // Geç kalan bilgileri "gec.txt" dosyasına yazılır
    gec_ve_erken_dosyaya_yaz("gec.txt", "\nEn gec gelen: %s, Geldigi saat: %02d:%02d, Gecikme: %d dakika\n", gecikme_durumu, en_gec_sicil, en_gec_saat, en_gec_dakika, gecikme);

    // Erken çıkan bilgileri "erken.txt" dosyasına yazılır
    gec_ve_erken_dosyaya_yaz("erken.txt", "\nEn erken giden: %s, Saat: %02d:%02d, Erken cikis: %d dakika\n", erken_bulundu, en_erken_sicil, en_erken_saat, en_erken_dakika, erken_cikis);

    printf("\nGec ve erken bilgileri dosyalara kaydedildi.\n");
}

// İki saat arasındaki farkı dakika cinsinden hesaplar
int saat_dakika_fark(int saat1, int dakika1, int saat2, int dakika2) {
    return (saat2 * 60 + dakika2) - (saat1 * 60 + dakika1); // Saatleri ve dakikaları dakika cinsine çevirerek farkı bulur
}

// Tüm çalışanların verilerini "gunluk.txt" dosyasından okuyup ekrana yazdırır
void tum_calisanlari_goruntule() {
    FILE *dosya = fopen("gunluk.txt", "r"); // Dosya okuma modunda açılır
    if (dosya == NULL) {
        printf("gunluk.txt dosyasi bulunamadi!\n"); // Dosya açılamazsa hata mesajı
        return;
    }

    printf("\n--- Tum Calisanlarin Verileri ---\n");

    char sicil_no[20];
    int saat, dakika;
    char durum;

    // Dosyadaki her satırı okuyup ekrana yazdırır
    while (fscanf(dosya, "%s %d:%d %c", sicil_no, &saat, &dakika, &durum) != EOF) {
        printf("| Sicil No: %s | Saat: %02d:%02d | Durum: %c |\n", sicil_no, saat, dakika, durum);
    }

    fclose(dosya); // Dosya kapatılır
    printf("\n");
}

// Değişken sayıda argüman alarak belirtilen dosyaya yazar
void dosyaya_yaz(const char *dosya_adi, const char *format, ...) {
    FILE *dosya = fopen(dosya_adi, "a"); // Dosya ekleme modunda açılır
    if (dosya == NULL) {
        printf("%s dosyasi olusturulamadi!\n", dosya_adi); // Dosya açılamazsa hata mesajı
        return;
    }

    va_list args;
    va_start(args, format); // Değişken argümanları al
    vfprintf(dosya, format, args); // Dosyaya yaz
    va_end(args); // Değişken argüman listesini temizle

    fclose(dosya); // Dosya kapatılır
}

// Geç ve erken bilgileri ilgili dosyaya yazar ve ekrana yazdırır
void gec_ve_erken_dosyaya_yaz(const char *dosya_adi, const char *metin, bool bulundu, const char *sicil, int saat, int dakika, int fark) {
    FILE *dosya = fopen(dosya_adi, "w"); // Dosya yazma modunda açılır
    if (dosya == NULL) {
        printf("%s dosyasi olusturulamadi!\n", dosya_adi); // Dosya açılamazsa hata mesajı
        return;
    }
    if (bulundu) { // Geç veya erken gelen bulunduysa
        fprintf(dosya, metin, sicil, saat, dakika, fark); // Dosyaya yaz
        printf(metin, sicil, saat, dakika, fark); // Ekrana yaz
    } else { // Geç veya erken gelen bulunamadıysa
        fprintf(dosya, "%s bulunamadi\n", dosya_adi == "gec.txt" ? "Gec kalan" : "Erken cikan"); // Dosyaya yaz
        printf("%s bulunamadi\n", dosya_adi == "gec.txt" ? "Gec kalan" : "Erken cikan"); // Ekrana yaz
    }
    fclose(dosya); // Dosya kapatılır
}

// Kullanıcıdan silinecek sicil numarasını alıp silme işlemini başlatır
void silinecek_sicil_al_ve_sil() {
    char silinecek_sicil[20];
    printf("Silinecek Calisanin Sicil Numarasi: ");
    fgets(silinecek_sicil, sizeof(silinecek_sicil), stdin); // Sicil numarası kullanıcıdan alınır
    silinecek_sicil[strcspn(silinecek_sicil, "\n")] = 0; // Satır sonundaki newline karakteri temizlenir
    calisan_kaydini_sil(silinecek_sicil); // Silme fonksiyonuna sicil numarası gönderilir
}

// Belirtilen sicil numarasına sahip çalışanın kaydını "gunluk.txt" dosyasından siler
void calisan_kaydini_sil(const char *silinecek_sicil) {
    FILE *dosya = fopen("gunluk.txt", "r"); // Dosya okuma modunda açılır
    if (dosya == NULL) {
        printf("gunluk.txt dosyasi bulunamadi!\n"); // Dosya açılamazsa hata mesajı
        return;
    }

    FILE *gecici_dosya = fopen("gecici.txt", "w"); // Geçici dosya yazma modunda açılır
    if (gecici_dosya == NULL) {
        printf("gecici.txt dosyasi olusturulamadi!\n"); // Geçici dosya açılamazsa hata mesajı
        fclose(dosya);
        return;
    }

    char sicil_no[20];
    int saat, dakika;
    char durum;
    bool kayit_silindi = false; // Kayıt silindi mi bilgisini tutar

    // Dosyanın sonuna kadar satır satır okunur
    while (fscanf(dosya, "%s %d:%d %c", sicil_no, &saat, &dakika, &durum) != EOF) {
        if (strcmp(sicil_no, silinecek_sicil) != 0) { // Okunan sicil numarası silinecek numaraya eşit değilse
            fprintf(gecici_dosya, "%s %02d:%02d %c\n", sicil_no, saat, dakika, durum); // Geçici dosyaya yaz
        } else { // Silinecek sicil numarası bulunduysa
            kayit_silindi = true; // Kayıt silindi olarak işaretlenir
        }
    }

    fclose(dosya); // Dosya kapatılır
    fclose(gecici_dosya); // Geçici dosya kapatılır

    if (remove("gunluk.txt") != 0) { // Orijinal dosya silinmeye çalışılır
        printf("gunluk.txt dosyasi silinemedi!\n"); // Dosya silinemezse hata mesajı
        remove("gecici.txt"); // Geçici dosya silinir
        return;
    }

    if (rename("gecici.txt", "gunluk.txt") != 0) { // Geçici dosya orijinal isimle yeniden adlandırılmaya çalışılır
        printf("gecici.txt dosyasi yeniden adlandirilamadi!\n"); // Dosya yeniden adlandırılamazsa hata mesajı
        remove("gecici.txt"); // Geçici dosya silinir
        return;
    }

    if(kayit_silindi){
        printf("Sicil numarasi %s olan calisanin kaydi silindi.\n",silinecek_sicil); // Kayıt silindi mesajı
    }
    else{
        printf("Sicil numarasi %s olan calisan kaydi bulunamadi.\n",silinecek_sicil); // Kayıt bulunamadı mesajı
    }
}
