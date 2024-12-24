#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define MAX_CALISAN 100
#define MAX_SATIR 100

// Calisan verileri icin struct tanimlama
typedef struct {
    char sicil_no[20];
    int saat;
    int dakika;
    char durum; // Giris icin '<', cikis icin '>'
} Calisan;

// Fonksiyon prototipleri
void veri_al_ve_kaydet();
void gec_ve_erken_bul();
void tum_calisanlari_goruntule();
int saat_dakika_fark(int saat1, int dakika1, int saat2, int dakika2);
void dosyaya_yaz(const char *dosya_adi, const char *format, ...);
void gec_ve_erken_dosyaya_yaz(const char *dosya_adi, const char *metin, bool bulundu, const char *sicil, int saat, int dakika, int fark);
void calisan_kaydini_sil(const char *silinecek_sicil);
void silinecek_sicil_al_ve_sil();


int main() {
    int secim;
    while (1) {
        printf("\n*** Fabrika Giris Sistemi ***\n");
        printf("1. Calisan Giris ve Cikis\n");
        printf("2. Gec ve Erken Gelenler\n");
        printf("3. Tum Calisanlari Goruntule\n");
        printf("4. Calisan Kaydini Sil\n");
        printf("5. Cikis\n");
        printf("Seciminizi yapin: ");
        scanf("%d", &secim);
        while (getchar() != '\n'); // Buffer temizleme
        switch (secim) {
            case 1:
                veri_al_ve_kaydet();
                break;
            case 2:
                gec_ve_erken_bul();
                break;
            case 3:
                tum_calisanlari_goruntule();
                break;
            case 4:
                silinecek_sicil_al_ve_sil();
                break;
            case 5:
                printf("Programdan cikiliyor...\n");
                return 0;
            default:
                printf("Gecersiz secim! Lutfen tekrar deneyin.\n");
        }
    }
    return 0;
}

// Kullanici verilerini alir ve dosyaya kaydeder
void veri_al_ve_kaydet() {
    int n;
    printf("Kac calisanin verisini girmek istiyorsunuz?: ");
    scanf("%d", &n);
    while (getchar() != '\n'); // Buffer temizleme
    Calisan calisanlar[MAX_CALISAN];
    for (int i = 0; i < n; i++) {
        char sicil_input[MAX_SATIR];
        printf("%d. Calisanin Sicil No: ", i + 1);
        fgets(sicil_input, sizeof(sicil_input), stdin);
        if(sscanf(sicil_input, "%s", calisanlar[i].sicil_no) != 1){
            printf("Hatali sicil numarasi girisi!\n");
            i--;
            continue;
        }

        printf("Saat (SS DD): ");
        if (scanf("%d %d", &calisanlar[i].saat, &calisanlar[i].dakika) != 2) {
            printf("Hatali saat girisi!\n");
            while (getchar() != '\n');
            i--;
            continue;
        }
        while (getchar() != '\n'); // Buffer temizleme

        printf("Durum (Giris icin '<', Cikis icin '>'): ");
        if(scanf(" %c", &calisanlar[i].durum) != 1){
            printf("Hatali durum girisi!\n");
            while (getchar() != '\n');
            i--;
            continue;
        }
        while (getchar() != '\n'); // Buffer temizleme

        dosyaya_yaz("gunluk.txt", "%s %02d:%02d %c\n", calisanlar[i].sicil_no, calisanlar[i].saat, calisanlar[i].dakika, calisanlar[i].durum);
    }

    printf("Veriler gunluk.txt dosyasina kaydedildi.\n");
}

// Dosyadaki verileri okur ve gec/erken bilgilerini bulur
void gec_ve_erken_bul() {
    FILE *dosya = fopen("gunluk.txt", "r");
    if (dosya == NULL) {
        printf("gunluk.txt dosyasi bulunamadi!\n");
        return;
    }

    char satir[MAX_SATIR];
    char en_gec_sicil[20], en_erken_sicil[20];
    int en_gec_saat = 9, en_gec_dakika = 0, gecikme = 0;
    int en_erken_saat = 17, en_erken_dakika = 0, erken_cikis = 0;
    bool gecikme_durumu = false, erken_bulundu = false;

    while (fgets(satir, sizeof(satir), dosya) != NULL) {
        char sicil_no[20];
        int saat, dakika;
        char durum;
        satir[strcspn(satir, "\n")] = 0; // Remove newline character
        if (sscanf(satir, "%s %d:%d %c", sicil_no, &saat, &dakika, &durum) == 4) {
            if (durum == '<') {
                int fark = saat_dakika_fark(9, 0, saat, dakika);
                if (fark > gecikme) {
                    gecikme = fark;
                    strcpy(en_gec_sicil, sicil_no);
                    en_gec_saat = saat;
                    en_gec_dakika = dakika;
                    gecikme_durumu = true;
                }
            } else if (durum == '>') {
                int fark = saat_dakika_fark(saat, dakika, 17, 0);
                if (fark > erken_cikis) {
                    erken_cikis = fark;
                    strcpy(en_erken_sicil, sicil_no);
                    en_erken_saat = saat;
                    en_erken_dakika = dakika;
                    erken_bulundu = true;
                }
            }
        } else {
            printf("Hatali satir formati: %s\n", satir); // Hatali satirlari goster
        }
    }

    fclose(dosya);

    // Gec.txt dosyasina yazma
    gec_ve_erken_dosyaya_yaz("gec.txt", "\nEn gec gelen: %s, Geldigi saat: %02d:%02d, Gecikme: %d dakika\n", gecikme_durumu, en_gec_sicil, en_gec_saat, en_gec_dakika, gecikme);

    // Erken.txt dosyasina yazma
    gec_ve_erken_dosyaya_yaz("erken.txt", "\nEn erken giden: %s, Saat: %02d:%02d, Erken cikis: %d dakika\n", erken_bulundu, en_erken_sicil, en_erken_saat, en_erken_dakika, erken_cikis);

    printf("\nGec ve erken bilgileri dosyalara kaydedildi.\n");
}

// Iki zaman arasindaki farki dakika cinsinden hesaplar
int saat_dakika_fark(int saat1, int dakika1, int saat2, int dakika2) {
    return (saat2 * 60 + dakika2) - (saat1 * 60 + dakika1);
}

void tum_calisanlari_goruntule() {
    FILE *dosya = fopen("gunluk.txt", "r");
    if (dosya == NULL) {
        printf("gunluk.txt dosyasi bulunamadi!\n");
        return;
    }

    printf("\n--- Tum Calisanlarin Verileri ---\n");

    char sicil_no[20];
    int saat, dakika;
    char durum;

    // Dosyadaki her satırı okuyup ekrana yazdırıyoruz
    while (fscanf(dosya, "%s %d:%d %c", sicil_no, &saat, &dakika, &durum) != EOF) {
        printf("| Sicil No: %s | Saat: %02d:%02d | Durum: %c |\n", sicil_no, saat, dakika, durum);
    }

    fclose(dosya);
    printf("\n");
}

void dosyaya_yaz(const char *dosya_adi, const char *format, ...) {
    FILE *dosya = fopen(dosya_adi, "a");
    if (dosya == NULL) {
        printf("%s dosyasi olusturulamadi!\n", dosya_adi);
        return;
    }

    va_list args;
    va_start(args, format);
    vfprintf(dosya, format, args);
    va_end(args);

    fclose(dosya);
}

void gec_ve_erken_dosyaya_yaz(const char *dosya_adi, const char *metin, bool bulundu, const char *sicil, int saat, int dakika, int fark) {
    FILE *dosya = fopen(dosya_adi, "w");
    if (dosya == NULL) {
        printf("%s dosyasi olusturulamadi!\n", dosya_adi);
        return;
    }
    if (bulundu) {
        fprintf(dosya, metin, sicil, saat, dakika, fark);
        printf(metin, sicil, saat, dakika, fark);
    } else {
        fprintf(dosya, "%s bulunamadi\n", dosya_adi == "gec.txt" ? "Gec kalan" : "Erken cikan");
        printf("%s bulunamadi\n", dosya_adi == "gec.txt" ? "Gec kalan" : "Erken cikan");
    }
    fclose(dosya);
}

void silinecek_sicil_al_ve_sil() {
    char silinecek_sicil[20];
    printf("Silinecek Calisanin Sicil Numarasi: ");
    fgets(silinecek_sicil, sizeof(silinecek_sicil), stdin);
    silinecek_sicil[strcspn(silinecek_sicil, "\n")] = 0; // Newline karakterini temizle
    calisan_kaydini_sil(silinecek_sicil);
}

void calisan_kaydini_sil(const char *silinecek_sicil) {
    FILE *dosya = fopen("gunluk.txt", "r");
    if (dosya == NULL) {
        printf("gunluk.txt dosyasi bulunamadi!\n");
        return;
    }

    FILE *gecici_dosya = fopen("gecici.txt", "w");
    if (gecici_dosya == NULL) {
        printf("gecici.txt dosyasi olusturulamadi!\n");
        fclose(dosya);
        return;
    }

    char sicil_no[20];
    int saat, dakika;
    char durum;
    bool kayit_silindi = false;

    while (fscanf(dosya, "%s %d:%d %c", sicil_no, &saat, &dakika, &durum) != EOF) {
        if (strcmp(sicil_no, silinecek_sicil) != 0) {
            fprintf(gecici_dosya, "%s %02d:%02d %c\n", sicil_no, saat, dakika, durum);
        } else {
            kayit_silindi = true;
        }
    }

    fclose(dosya);
    fclose(gecici_dosya);

    if (remove("gunluk.txt") != 0) {
        printf("gunluk.txt dosyasi silinemedi!\n");
        remove("gecici.txt");
        return;
    }

    if (rename("gecici.txt", "gunluk.txt") != 0) {
        printf("gecici.txt dosyasi yeniden adlandirilamadi!\n");
        remove("gecici.txt");
        return;
    }

    if(kayit_silindi){
        printf("Sicil numarasi %s olan calisanin kaydi silindi.\n",silinecek_sicil);
    }
    else{
        printf("Sicil numarasi %s olan calisan kaydi bulunamadi.\n",silinecek_sicil);
    }
}