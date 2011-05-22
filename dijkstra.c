//      dijkstra_v2.c
//  	Powered by jack_r
//		Program veriyi dijkstra.in dosyasından alır ve dijkstra.out
//		dosyasına çıktıyı yazar.
//		Verilen verilere gore sorulan 2 sehir arasi en kisa yolu bulur

#include <stdio.h>
#include <stdlib.h>

#define sonsuz 999;

struct harita {
	int 				komsu_ad;						// Baglantısı olan şehirlerin ismi
	int 				uzaklik;						// Baglantısı olan şehirlere uzaklığı
	int 				kullan;							// dijkstra için gerekli
	struct harita 		*sehir;							// Haritadaki sehirler
	struct harita 		*baglanti;						// sehrin komsuları
};

struct harita *yollar; // Tüm sehirler
int sinir = 0; // Şehir miktarı
int dizi[2];   // Sorulan sehirler burada tutulacak

//~ "kullan"a atama(dijkstra için gerekli)
void
ata(int atanacak) {
	struct harita *tara;
	for (tara = yollar; tara; tara = tara->sehir)
		if(tara->komsu_ad == atanacak)
			break;
	tara->kullan = 1;
}

//~ "kullan"ı öğrenme(dijkstra için gerekli)
int
ver(int verilecek) {
	struct harita *tara;
	for (tara = yollar; tara; tara = tara->sehir)
		if (tara->komsu_ad == verilecek)
			break;
	return tara->kullan;
}

//	Olusturma ve Ilklendirme
struct harita *
harita(int komsu, int uzak) {
	struct harita *yeni;

	if ((yeni = malloc(sizeof(struct harita))) == NULL) {
		fprintf(stderr, "bellek tukendi\n");
		exit(EXIT_FAILURE);
	}
	yeni->komsu_ad = komsu;
	yeni->uzaklik = uzak;
	return yeni;
}

void
ekle(int sehir, int mesafe) {
	struct harita *yeni;
	yeni = harita(sehir, mesafe);
	yeni->sehir = yollar;
	yollar = yeni;
	yollar->baglanti = NULL;
	yollar->kullan = 0;
}

//~ İlk defa eklenecek mi sorgulama
int 
isVar(int sehir) {
	struct harita *ara;
	for (ara = yollar; ara; ara = ara->sehir)
		if(ara->komsu_ad == sehir)
			return 1;
	return 0;
}

//~ İki şehir arası mesafeyi bulma
int
mesafe(int ev, int komsu) {
	struct harita *ara;
	struct harita *ara2;
	for (ara = yollar; ara; ara = ara->sehir)
		if (ara->komsu_ad == ev)
			for (ara2 = ara->baglanti; ara2; ara2 = ara2->baglanti)
				if (ara2->komsu_ad == komsu)
					return ara2->uzaklik;
	return sonsuz;
}

//~ Haritayı oluşturma
void 
yerlestir(int sehir1, int sehir2, int uzak) {
	struct harita *yeni, *yeni2;
	if (!isVar(sehir1)) {
		ekle(sehir1, 0);
	}
	
	if (!isVar(sehir2)) {
		ekle(sehir2, 0);
	}
	
	for (yeni = yollar; yeni; yeni = yeni->sehir)
		if(yeni->komsu_ad == sehir1)
			break;
			
	yeni2 = harita(sehir2, uzak);
	yeni2->baglanti = yeni->baglanti;
	yeni->baglanti = yeni2;
	
	for (yeni = yollar; yeni; yeni = yeni->sehir)
		if(yeni->komsu_ad == sehir2)
			break;
			
	yeni2 = harita(sehir1, uzak);
	yeni2->baglanti = yeni->baglanti;
	yeni->baglanti = yeni2;
	
	if (sinir < sehir1)
		sinir = sehir1;
	if (sinir < sehir2)
		sinir = sehir2;
}

//~ Başlama
void
dosya_ac(void) {
    FILE *girdi;
    int sehir1, sehir2, uzak, bag;
    if (!(girdi = fopen("dijkstra.in","r"))) {
         printf("dijkstra.in isimli dosya acilamadi.\n");
         exit(EXIT_FAILURE);
    }
    fscanf(girdi,"%d\n", &bag);
	while (bag) {
        fscanf(girdi,"%d %d %d\n", &sehir1, &sehir2, &uzak);
        yerlestir(sehir1, sehir2, uzak);
        bag--;
	}
    fscanf(girdi,"%d %d\n", &sehir1, &sehir2);
    dizi[0] = sehir1;
    dizi[1] = sehir2;
    fclose(girdi);
}

//~ En kısa yolu bulma
int
dijkstra(int bas, int son) {
    int min_yol, min_uzak, i, kucuk, yedek;
    int *uzakliklar;
    
    sinir++;
    
    uzakliklar = malloc(sizeof(int) * sinir);
    
    for (i = 0; i < sinir; i++)
          *(uzakliklar + i) = sonsuz;
    min_yol = bas;
    min_uzak = 0;
    ata(min_yol);
    while (min_yol != son) {
        for (i = 1; i < sinir; i++) {
              if ((ver(i) == 1) || (min_yol == i))
                  continue;
              if ((min_uzak + mesafe(min_yol, i)) < *(uzakliklar + i)) {
                  *(uzakliklar + i) = min_uzak + mesafe(min_yol, i);
              }
        }
        kucuk = 99;
        for (i = 1; i < sinir; i++) {
              if ((ver(i) == 1) || (i == min_yol))
                  continue;
              if (kucuk > *(uzakliklar + i)) {
                  kucuk = *(uzakliklar + i);
                  yedek = i;
              }
        }
        min_uzak = *(uzakliklar + yedek);
        min_yol = yedek;
        ata(min_yol);
    }
    free(uzakliklar);
    return kucuk;
}

//~ Belleği boşaltma
void
yoket(void) {
	struct harita *sonra, *yan, *alt, *sonra2;
	
	yan = yollar;
	while (yan) {
		sonra2 = yan->sehir;
		alt = yan->baglanti;
		while (alt) {
			sonra = alt->baglanti;
			free(alt);
			alt = sonra;
		}
		free(yan);
		yan = sonra2;
	}
	free(yollar);
}

//~  Bitiriş
void yaz(int sayi) {
     FILE *cikti;
     if ((cikti = fopen("dijkstra.out","w")) == NULL) {
         fprintf(stderr,"dijkstra.out isimli dosya olusturulamiyor\n");
         exit(EXIT_FAILURE);
     }
     fprintf(cikti,"en kisa uzaklik = %d\n",sayi);
     fclose(cikti);
}

int main() {
	int i;
	yollar = malloc(sizeof(struct harita));
	yollar = NULL;
	dosya_ac();
	i = dijkstra(dizi[0], dizi[1]);
	printf("en kısa mesafe: %d", i);
	yaz(i);
	yoket();
	getchar();
	return 0;
}
