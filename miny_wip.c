#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int riadky;
    int stlpce;
    int pocetBomb;
    int wincon;
} nastaveniehp;

typedef struct {
    int jeBomba;
    int susediSBombou;
    int jeZakryte;
} policko;

nastaveniehp nastavithp(){
    nastaveniehp nast;

    printf("Zadajte pocet riadkov herneho pola: ");
    scanf("%d", &nast.riadky);
    printf("Zadajte pocet stlpcov herneho pola: ");
    scanf("%d", &nast.stlpce);
    printf("Zadajte pocet pocet bomb v hernom poli: ");
    scanf("%d", &nast.pocetBomb);

    while (nast.pocetBomb > nast.riadky * nast.stlpce) {
        printf("Zadany pocet pocet bomb presahuje pocet policok herneho pola. Zadajte platnu hodnotu:\n");
        scanf("%d", &nast.pocetBomb);
    }
    return nast;
}
void generaciaSuradnicBomb(int riadky, int stlpce, int pocetBomb, policko **hernepole,int in_x,int in_y) {
    srand(time(NULL));
    int pocetUmiestnenychBomb = 0;

    while (pocetUmiestnenychBomb < pocetBomb) {
        int x = rand() % riadky;
        int y = rand() % stlpce;

        if (hernepole[x][y].jeBomba == 0) {
            if (!(x >= in_x - 1 && x <= in_x + 1 && y >= in_y - 1 && y <= in_y + 1)) {
                hernepole[x][y].jeBomba = 1;
                pocetUmiestnenychBomb++;
            }
        }
    }
}

//debug funkcia
void vypisanieSuradnicBomb(policko **hernepole, int riadky, int stlpce) {
    printf("Suradnice bomb:\n");
    for (int i = 0; i < riadky; i++) {
        for (int j = 0; j < stlpce; j++) {
            if (hernepole[i][j].jeBomba) {
                printf("[%d,%d],\n", i+1, j+1);
            }
        }
    }
}

void vypocetSusediSBombou(int riadky, int stlpce, policko **hernepole) {
    for (int x = 0; x < riadky; x++) {
        for (int y = 0; y < stlpce; y++) {
            if (hernepole[x][y].jeBomba) {
                continue; // Skip pocetBomb cells
            }

            int pocetSusediSBombou = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < riadky && ny >= 0 && ny < stlpce && hernepole[nx][ny].jeBomba) {
                        pocetSusediSBombou++;
                    }
                }
            }

            hernepole[x][y].susediSBombou = pocetSusediSBombou;
        }
    }
}


void vypisanieHernehoPola(int riadky, int stlpce, policko **hernepole) {
    for (int i = 0; i < riadky; i++) {
        for (int j = 0; j < stlpce; j++) {
            if (hernepole[i][j].jeZakryte) {
                printf("? ");
            } else if (hernepole[i][j].jeBomba) {
                printf("* ");
            } else {
                printf("%d ", hernepole[i][j].susediSBombou);
            }
        }
        printf("\n");
    }
}

void odhaleniePolicok(policko **hernepole, int x, int y, int riadky, int stlpce, int *wincon) {
    if (x < 0 || x >= riadky || y < 0 || y >= stlpce) {
        //printf("Error: Out of bounds. Try again.\n");
        return;
    }
    else if (!hernepole[x][y].jeZakryte) {
        //printf("Error: Cell already uncovered. Try again.\n");
        return;
    }
    else if (hernepole[x][y].jeBomba) {
        *wincon=-1;
        for (int i = 0; i < riadky; i++) {
            for (int j = 0; j < stlpce; j++) {
                if (hernepole[i][j].jeBomba) {
                    hernepole[i][j].jeZakryte = 0;
                }
            }
        }
        return;
    }
    else {
        hernepole[x][y].jeZakryte = 0;
        (*wincon)--;
        if (hernepole[x][y].susediSBombou == 0) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;

                    int newX = x + dx;
                    int newY = y + dy;
                    odhaleniePolicok(hernepole, newX, newY, riadky, stlpce, wincon);

                }
            }
        }
    }
}

int uzivatelskeMenu() {
    int vyber;
    printf("Zadajte vas vyber z nasledujucich moznosti:\n");
    printf("1. Zahajit novu hru\n");
    printf("2. Nastavit herne pole a zahajit novu hru\n");
    printf("3. Zobrazit najlepsie casy pre nastavene hracie pole\n");
    printf("4. Ukoncit program\n");
    scanf("%d", &vyber);
    return vyber;
}

double vyhra(double t, int riadky, int stlpce, policko **hernepole) {
    double dosiahnutyCas = ((double)t)/CLOCKS_PER_SEC;
    vypisanieHernehoPola(riadky, stlpce, hernepole);
    printf("Vyhrali ste s casom %f s. Gratulujem!\n",dosiahnutyCas);
    return dosiahnutyCas;
}

double prehra(double t, int riadky, int stlpce, policko **hernepole) {
    vypisanieHernehoPola(riadky, stlpce, hernepole);
    printf("Prehrali ste. To sa stava aj najlepsim!\n");
}

void vypisanierebricku(int riadky, int stlpce, int pocetBomb) {
    char filename[10];
    sprintf(filename,"%dx%dx%d.txt",riadky,stlpce,pocetBomb);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening times file");
        return;
    }

    float times[100];
    int count = 0;

    while (fscanf(file, "%f", &times[count]) != EOF) {
        count++;
        if (count >= 100) {
            break;
        }
    }

    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (times[j] > times[j + 1]) {
                float temp = times[j];
                times[j] = times[j + 1];
                times[j + 1] = temp;
            }
        }
    }
    printf("Najlepsie casy pre nastavene hracie pole:\n");
    for (int i = 0; i < count; i++) {
        printf("%f\n", times[i]);
    }
    return;
}

int main() {
    nastaveniehp nast = nastavithp();
    int vyber;
    do{
        nast.wincon = nast.riadky * nast.stlpce - nast.pocetBomb;
        policko **hernepole = malloc(nast.riadky * sizeof(policko*));
        for (int i = 0; i < nast.riadky; i++) {
            hernepole[i] = malloc(nast.stlpce * sizeof(policko));
            for (int j = 0; j <nast.stlpce; j++) {
                hernepole[i][j].jeBomba = 0;
                hernepole[i][j].susediSBombou = 0;
                hernepole[i][j].jeZakryte = 1;
            }
        }

        vypisanieHernehoPola(nast.riadky, nast.stlpce, hernepole);

        clock_t t;
        t=clock();
        printf("Casomiera spustena.\n");

        int in_x, in_y;
        printf("Zadajte suradnice policka, ktore si zelate odhalit (riadok stlpec):\n");
        scanf("%d %d", &in_x, &in_y);
        in_x--,in_y--;

        generaciaSuradnicBomb(nast.riadky, nast.stlpce, nast.pocetBomb, hernepole, in_x, in_y);
        vypisanieSuradnicBomb(hernepole, nast.riadky, nast.stlpce);
        vypocetSusediSBombou(nast.riadky, nast.stlpce, hernepole);
        odhaleniePolicok(hernepole, in_x, in_y, nast.riadky, nast.stlpce,&nast.wincon);

        printf("Do vyhry vam este zostava odhalit %d policok.\n", nast.wincon);

        if (nast.wincon==0){
            double dosiahnutyCas=vyhra((clock()-t), nast.riadky, nast.stlpce, hernepole);
        
             char filename[10];
             sprintf(filename,"%dx%dx%d.txt",nast.riadky,nast.stlpce,nast.pocetBomb);
             printf(filename);
             FILE *file;
             file=fopen(filename,"a");
             fprintf(file, "%f\n",dosiahnutyCas);
             fclose(file);

            for (int i = 0; i < nast.riadky; i++) {
                free(hernepole[i]);
            }
            free(hernepole);

            vyber = uzivatelskeMenu();
            switch (vyber) {
                case 1:
                    continue;
                case 2:
                    nast=nastavithp();
                    continue;
                case 3:
                    vypisanierebricku(nast.riadky,nast.stlpce,nast.pocetBomb);
                    vyber = uzivatelskeMenu();
                case 4:
                    printf("Exiting game.\n");
                    break;
                default:
                    printf("Invalid vyber. Please try again.\n");
                    break;
                }

        } else {
            while (1) {
                vypisanieHernehoPola(nast.riadky, nast.stlpce, hernepole);

                int x, y;
                printf("Zadajte suradnice policka, ktore si zelate odhalit (riadok stlpec):\n");
                scanf("%d %d", &x, &y);
                x--; y--;
                
                if (x >= 0 && x < nast.riadky && y >= 0 && y < nast.stlpce) {
                    odhaleniePolicok(hernepole, x, y, nast.riadky, nast.stlpce,&nast.wincon);
                } else {
                    printf("Neplatne suradnice! Skuste znova.\n");
                }

                printf("Do vyhry vam este zostava odhalit %d policok.\n", nast.wincon);

                if (nast.wincon==0){
                    double dosiahnutyCas=vyhra((clock()-t), nast.riadky, nast.stlpce, hernepole);

                    char filename[10];
                    sprintf(filename,"%dx%dx%d.txt",nast.riadky,nast.stlpce,nast.pocetBomb);
                    printf(filename);
                    FILE *file;
                    file=fopen(filename,"a");
                    fprintf(file, "%f\n",dosiahnutyCas);
                    fclose(file);
                    
                    break;
                }else if(nast.wincon==-1){
                    double dosiahnutyCas=prehra((clock()-t), nast.riadky, nast.stlpce, hernepole);
                
                    break;
                }
            }
        }

        for (int i = 0; i < nast.riadky; i++) {
            free(hernepole[i]);
        }
        free(hernepole);
        
        vyber = uzivatelskeMenu();
        switch (vyber) {
            case 1:
                break;
            case 2:
                nast=nastavithp();
                break;
            case 3:
                vypisanierebricku(nast.riadky,nast.stlpce,nast.pocetBomb);
                vyber = uzivatelskeMenu();
            case 4: // Exit game
                printf("Exiting game.\n");
                break;
            default:
                printf("Invalid vyber. Please try again.\n");
                break;
            }
        }
        while (vyber != 4);
        return 0;
}
