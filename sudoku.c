//compile with gcc -O sudoku.c -o sudoku
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
const int maxReverse = 150;
const int safety = 15;
const int maxTrials = 8000;

int intpow(int, int);
int box(int[9][9], int, int, int);
int column(int[9][9], int , int);
int row(int[9][9], int , int);
int compare(int[9][9], int[9][9][87], int);
int done(int[9][9]);
int emptyfields(int[9][9]);
void showtask(int[9][9], int[9][9][87], int, int *, int *);
void getcopy(int[9][9], int[9][9][87], int);
void makecopy(int[9][9], int[9][9][87], int);
void printsheet(int[9][9]);
void writesheet2(int[9][9], int);
void writesheet(int[9][9], int, int);
void setzero(int[9][9]);
void removefield(int[9][9]);
void boxmethod(int[9][9], int, int, int *);
void rowmethod(int[9][9], int, int *);
void columnmethod(int[9][9], int, int *);
void allmethods(int[9][9], int[9][9][87], int *);
void legalguess(int[9][9]);
void findsolution(int[9][9], int[9][9][87], int *, int *);
void findsudoku(char, int[9][9], int[9][9][87], int *, int , int *, int *);
void inputsheet(int[9][9]);
void fixline(int[9][9]);
void inputdata(int[9][9]);
void generateSudokus();
void solveSudoku(int[9][9]);
void easysheet(int[9][9]);
void hardsheet(int[9][9]);
void intro();

int main(){
    srand(time(NULL));
    int choice, returnval;
    intro();
    printf("[1] Solve sudoku\n");
    printf("[2] Generate sudokus\n");
    printf("Choice: ");
    returnval = scanf("%d", &choice);
    if(choice== 1){
        int sheet[9][9];
        inputsheet(sheet);
        //easysheet(sheet);
        //hardsheet(sheet);
        printsheet(sheet);
        solveSudoku(sheet);
    }
    else{
        generateSudokus();
        printf("Sudokus printed to sudoku.tex\n");
    }
    printf("Terminating\n");
    return 0;
}

void solveSudoku(int sheet[9][9]){
    int sheets[9][9][87], guess, rounds;
    findsolution(sheet, sheets, &guess, &rounds);
    printsheet(sheet);
    if(rounds > 0 || guess > 0){
      printf("We solved it in guess round number %d after %d guess(es).\n\n", rounds, guess);
    }
    else{
      printf("We solved it without guessing!\n\n");
    }
}

void generateSudokus(){
    int  sheet[9][9], sheets[9][9][87], attempt, guess, rounds, desiredFields, number, returnval;
    char allowGuess;
    printf("Input desired filled fields(22 <): ");
    returnval = scanf("%d", &desiredFields);
    printf("Is guessing allowed? (y/n) ");
    returnval = scanf("%s", &allowGuess);
    printf("How many sudokus?");
    returnval = scanf("%d", &number);
    for(int tt=0; tt<number; tt++){
        attempt = 0;
        findsudoku(allowGuess, sheet, sheets, &attempt, desiredFields, &guess, &rounds);
//        writesheet2(sheet,tt);
        writesheet(sheet, tt, number);
        showtask(sheet, sheets, attempt, &guess, &rounds);
    }
}

void findsudoku(char allowGuess, int sheet[9][9], int sheets[9][9][87], int *attempt, int desiredFields, int *guess, int *rounds){
    int jj, ii;
    while(*attempt < maxTrials && emptyfields(sheet) != 81-desiredFields){
        setzero(sheet);
        findsolution(sheet, sheets, guess, rounds);
        makecopy(sheet, sheets, 1); // this the sudoku to remove fields from
        ii = 0;
        while(emptyfields(sheet) != 81-desiredFields && ii < maxReverse){
            removefield(sheet);
            makecopy(sheet, sheets, 3); //this sudoku has multiple solutions
            findsolution(sheet, sheets, guess, rounds);
            makecopy(sheet, sheets, 4); // the solution is stored here
            jj = 0;
            while(jj < safety){
                getcopy(sheet, sheets, 3);
                findsolution(sheet, sheets, guess, rounds);
                if(compare(sheet, sheets, 4) == 1){ //is the solution identical to the first?
                    jj += 1;
                }
                else{//new solution found
                    jj = safety + 1;
                    getcopy(sheet, sheets, 2);//we have to take a step back
                    ii += 1; //we have to restart if this continues!
                }
            }
            if(jj == safety && (allowGuess != 'n' || *rounds == 0)){ //solution is still unique
                 getcopy(sheet, sheets, 3);
                 makecopy(sheet, sheets ,2);
            }
            else{
                getcopy(sheet, sheets, 2);//we have to take a step back
                ii += 1; //we have to restart if this continues!
            }
        }
        if(emptyfields(sheet) != 81-desiredFields){ //vi gav op
            *attempt += 1;
//            printf("GAH, it became to difficult to remove more, so we restart, but we got down to %d fields. - it was our attempt number %d.\n", 81 - emptyfields(sheet), *attempt);
        }
    }
}

void findsolution(int sheet[9][9], int sheets[9][9][87], int *guess, int *rounds){
    int error = 0;
    *guess = 0; *rounds = 0;
    allmethods(sheet, sheets, &error);
    makecopy(sheet, sheets, 0);
    while(done(sheet) == 0){
        error = 0; *guess= 0;
        while(error== 0 && done(sheet) == 0){
            legalguess(sheet);
            *guess+= 1;
            allmethods(sheet, sheets, &error);
        }
        if(error == 1){
            getcopy(sheet, sheets, 0);
        }
        *rounds += 1;
    }
}

void legalguess(int sheet[9][9]){
    while(1){
        int xx = rand()%9;
        int yy = rand()%9;
        int number = (rand()%9) + 1;
        if(column(sheet, xx, number) != 0 || row(sheet, yy, number) != 0 || box(sheet, xx / 3, yy / 3, number) != 0 || sheet[xx][yy] != 0 ){
            continue;
        }
        sheet[xx][yy] = number;
        return;
    }
}

void allmethods(int sheet[9][9], int sheets[9][9][87], int *error){
    makecopy(sheet, sheets, 86);
    do{
        makecopy(sheet, sheets, 86);
        for(int yy=0; yy<3; yy++){
            if(*error != 0){
               break;
            }
            for(int xx=0; xx<3; xx++){
                if(*error != 0){
                   break;
                }
                boxmethod(sheet, xx, yy, error);
            }
        }
        for(int xx=0; xx<3; xx++){
            if(*error != 0){
               break;
            }
             columnmethod(sheet, xx, error);
        }
         for(int yy=0; yy<3; yy++){
            if(*error != 0){
               break;
            }
            rowmethod(sheet, yy, error);
        }
    }while(compare(sheet, sheets, 86) == 0 && *error == 0);
}

void boxmethod(int sheet[9][9], int boxx, int boxy, int *error){
     int good, goodx, goody;
     for(int number = 1; number < 10; number++){
         if(*error != 0){
             break;
         }
         good = 0;
         if(box(sheet, boxx, boxy, number) == 0) //the number is missing from the box
         {
             for(int yy = boxy * 3; yy < boxy * 3 + 3; yy++){
                 for(int xx = boxx * 3; xx < boxx * 3 + 3; xx++){
                     if(sheet[xx][yy] == 0 && column(sheet, xx, number) == 0 && row(sheet, yy, number) == 0) //it is an empty field, and the number can be assigned
                     {
                         good += 1;
                         goodx = xx;
                         goody = yy;
                     }
                 }
             }
             if(good == 1){
                 sheet[goodx][goody] = number;
             }
             if(good == 0){
                 *error = 1;
             }
         }
     }
}

void rowmethod(int sheet[9][9], int rw, int *error){
     int good, goodx;
     for(int number = 1; number < 10; number++){
         if(*error != 0){
             break;
         }
         good = 0;
         if(row(sheet, rw, number) == 0){ //the number is missing from the row
             for(int xx = 0; xx<9; xx++){
                 if(sheet[xx][rw] == 0 && column(sheet, xx, number) == 0 && box(sheet, xx / 3, rw/ 3, number) == 0){ //it is an empty field, and the number can be assigned
                     good+= 1;
                     goodx = xx;
                 }
             }
             if(good == 1){
                 sheet[goodx][rw] = number;
             }
             if(good == 0){
                 *error = 1;
             }
         }
     }
}

void columnmethod(int sheet[9][9], int clmn, int *error){
     int good, goody;
     for(int number = 1; number < 10; number++){
         if(*error != 0){
             break;
         }
         good = 0;
         if(column(sheet, clmn, number) == 0){ //the number is missing from the column
             for(int yy=0; yy<9; yy++){
                 if(sheet[clmn][yy] == 0 && row(sheet, yy, number) == 0 && box(sheet, clmn/ 3, yy / 3, number) == 0){ //it is an empty field, and the number can be assigned
                     good += 1;
                     goody = yy;
                 }
             }
             if(good == 1){
                 sheet[clmn][goody] = number;
             }
             if(good == 0){
                 *error = 1;
             }
         }
     }
}

int box(int sheet[9][9], int xxx, int yyy, int number){
    int zz = 0;
    for(int yy = yyy*3; yy < yyy * 3 + 3; yy++){
        if(zz != 0){
           break;
        }
        for(int xx = xxx*3; xx < xxx * 3 + 3; xx++){
            if(zz != 0){
               break;
            }
            if(sheet[xx][yy] == number){
                zz = 1;
            }
        }
    }
    return zz;
}

int column(int sheet[9][9], int clmn, int number){
    int zz = 0;
    for(int yy=0; yy<9; yy++){
         if(zz != 0){
            break;
        }
        if(sheet[clmn][yy] == number){
            zz = 1;
        }
    }
    return zz;
}

int row(int sheet[9][9], int rw, int number){
    int zz = 0;
    for(int xx=0; xx<9; xx++){
       if(zz != 0){
            break;
        }
        if(sheet[xx][rw] == number){
            zz = 1;
        }
    }
    return zz;
}

int compare(int sheet[9][9], int sheets[9][9][87], int sht){
    int zz = 1;
    for(int yy=0; yy<9; yy++){
         if(zz != 1){
             break;
         }
         for(int xx=0; xx<9; xx++){
             if(zz != 1){
               break;
             }
             if(sheet[xx][yy] != sheets[xx][yy][sht]){
                 zz = 0;
             }
         }
    }
    return zz;
}

int emptyfields(int sheet[9][9]){
    int zz = 0;
    for(int yy=0; yy<9; yy++){
         for(int xx=0; xx<9; xx++){
              if(sheet[xx][yy] == 0){
                  zz += 1;
              }
         }
    }
    return zz;
}

int done(int sheet[9][9]){
    int zz = 1;
    for(int yy=0; yy<9; yy++){
         if(zz != 1){
             break;
         }
         for(int xx=0; xx<9; xx++){
              if(zz != 1){
                  break;
              }
              if(sheet[xx][yy] == 0){
                  zz = 0;
              }
         }
    }
    return zz;
}

void makecopy(int sheet[9][9], int sheets[9][9][87], int sht){
     for(int yy=0; yy<9; yy++){
         for(int xx=0; xx<9; xx++){
             sheets[xx][yy][sht] = sheet[xx][yy];
         }
    }
}

void getcopy(int sheet[9][9], int sheets[9][9][87], int sht){
     for(int yy=0; yy<9; yy++){
         for(int xx=0; xx<9; xx++){
             sheet[xx][yy] = sheets[xx][yy][sht];
         }
    }
}

void printsheet(int sheet[9][9]){
     for(int yy=0; yy<9; yy++){
         for(int xx=0; xx<9; xx++){
             if(sheet[xx][yy] != 0){
                 printf("%d", sheet[xx][yy]);
                 if(xx != 2 && xx != 5){
                     printf(" ");
                 }
             }
             else{
                 printf(" ");
                 if(xx != 2 && xx != 5){
                     printf(" ");
                 }
             }
             if(xx == 2 || xx == 5){
                 printf("|");
             }
         }
         printf("\n");
         if(yy == 2 || yy == 5){
             printf("-----+-----+-----\n");
         }
     }
}

void skrivark2(int sheet[9][9], int sht){
     FILE *fp;
     char sht2[10];
     snprintf(sht2, sizeof(sht2), "%d", sht+1);
     fp = fopen(sht2, "w+");
     for(int yy=0; yy<9; yy++){
         for(int xx=0; xx<9; xx++){
             if(sheet[xx][yy] != 0){
                 fprintf(fp, "%d", sheet[xx][yy]);
                 if(xx != 2 && xx != 5){
                     fprintf(fp, " ");
                 }
             }
             else{
                 fprintf(fp, " ");
                 if(xx != 2 && xx != 5){
                     fprintf(fp, " ");
                 }
             }
             if(xx == 2 || xx == 5){
                 fprintf(fp, "|");
             }
         }
         fprintf(fp, "\n");
         if(yy == 2 || yy == 5){
             fprintf(fp, "-----+-----+-----\n");
         }
     }
    fclose(fp);
}

void writesheet(int sheet[9][9], int sht, int maxsht){
     FILE *fp;
     if(sht==0){
       fp = fopen("sudoku.tex", "w+");
       fprintf(fp, "\\documentclass[twocolumn]{article}\n");
       fprintf(fp, "\\begin{document}\n");
     }
     else{
       fp = fopen("sudoku.tex", "a+");
     }
     fprintf(fp, "\\begin{table}\n");
     fprintf(fp, "\\begin{tabular}{||c | c | c || c | c | c || c | c | c ||} \\hline \\hline\n");
     for(int yy=0; yy<9; yy++){
         for(int xx=0; xx<9; xx++){
             if(sheet[xx][yy] != 0){
                 fprintf(fp, "%d", sheet[xx][yy]);
             }
             else{
                 fprintf(fp, " ");
             }
             if(xx != 8){
                   fprintf(fp, "&");
            }

         }
         fprintf(fp, "\\\\ \\hline");
         if(yy == 2 || yy == 5){
             fprintf(fp, "\\hline");
         }
         fprintf(fp, "\n");
     }
     fprintf(fp, "\\end{tabular}\n");
     fprintf(fp, "\\end{table}\n");
     if(sht == maxsht-1){
        fprintf(fp, "\\end{document}\n");
     }
     fclose(fp);
}

void setzero(int sheet[9][9]){
     for(int yy=0; yy<9; yy++){
         for(int xx=0; xx<9; xx++){
            sheet[xx][yy] = 0;
        }
    }
}

void removefield(int sheet[9][9]){
    int xx, yy;
    do{
        xx = rand()%9;
        yy = rand()%9;
    }while(sheet[xx][yy] == 0);
    sheet[xx][yy] = 0;
}

void showtask(int sheet[9][9], int sheets[9][9][87], int attempt, int *guess, int *rounds){
    if(attempt == maxTrials){
        printf("We failed to identify a sudoku according to the specifications\n");
    }
    else{
        printf("We found a sudoku according to the specification in attempt number %d:\n", attempt + 1);
        printsheet(sheet);
        findsolution(sheet, sheets, guess, rounds);
        if(*rounds > 0 || *guess > 0){
          printf("We solved it after %d guesses in round number %d.\n\n", *guess, *rounds);
        }
        else{
            printf("We solved it without guessing!\n\n");
        }
    }
}

void inputsheet(int sheet[9][9]){
    int input, returnval;
    char correct;
    inputdata(sheet);
    printf("Is this correct (y/n)?");
    returnval = scanf("%s", &correct);
    while(correct == 'n'){
      fixline(sheet);
      printf("Is this correct (y/n)?");
      returnval = scanf("%s", &correct);
    }
}

void inputdata(int sheet[9][9])
{
    int xx = 0, yy = 0, line, ii, returnval;
    printf("0 corresponds to empty field\n");
    while(yy < 9)
    {
        ii = 0;
        returnval = scanf("%d", &line);
        while(xx < 9)
        {
            sheet[xx][yy] = (line - ii)/ intpow(10, 8 - xx);
            ii += sheet[xx][yy] * intpow(10, 8 - xx);
            xx += 1;
        }
        xx = 0;
        yy += 1;
    }
    printf("Current input sudoku:\n");
    printsheet(sheet);
}

void fixline(int sheet[9][9])
{
    int line, yy, returnval, xx = 0, ii = 0;
    printf("Which line is wrong (counting from above)?");
    returnval = scanf("%d", &yy);
    printf("Input correct line:");
    returnval = scanf("%d", &line);
    while(xx < 9)
    {
        sheet[xx][yy - 1] = (line- ii)/ intpow(10, 8 - xx);
        ii += sheet[xx][yy - 1] * intpow(10, 8 - xx);
        xx += 1;
    }
    printf("Current input sudoku:\n");
    printsheet(sheet);
}

int intpow(int number, int exponent)
{
    int ii = 0, output = 1;
    while(ii < exponent)
    {
        output *= number;
        ii += 1;
    }
    return output;
}

void intro(){
    printf("Sudoku solver/generator\n");
    printf("Copyright (C) 2006  Kristian Ejlebjerg Jensen (kristian[7*191]jensen@gmail.com)\n");
    printf("\n");
    printf("This program is free software: you can redistribute it and/or modify\n");
    printf("it under the terms of the GNU General Public License as published by\n");
    printf("the Free Software Foundation, either version 3 of the License, or\n");
    printf("(at your option) any later version.\n");
    printf("\n");
    printf("This program is distributed in the hope that it will be useful,\n");
    printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    printf("GNU General Public License for more details.\n");
    printf("\n");
    printf("You should have received a copy of the GNU General Public License\n");
    printf("along with this program.  If not, see <https://www.gnu.org/licenses/>.\n\n");
}

void easysheet(int sheet[9][9]){
 setzero(sheet);
 sheet[0][0] = 7;
 sheet[3][0] = 4;
 sheet[4][0] = 6;
 sheet[5][0] = 9;
 sheet[7][0] = 2;
 sheet[0][1] = 3;
 sheet[3][1] = 2;
 sheet[6][1] = 1;
 sheet[0][2] = 2;
 sheet[5][2] = 8;
 sheet[7][2] = 5;
 sheet[1][3] = 3;
 sheet[3][3] = 1;
 sheet[5][3] = 2;
 sheet[6][3] = 4;
 sheet[1][4] = 1;
 sheet[3][4] = 9;
 sheet[6][4] = 2;
 sheet[8][4] = 6;
 sheet[2][5] = 7;
 sheet[5][5] = 5;
 sheet[2][6] = 8;
 sheet[3][6] = 7;
 sheet[5][6] = 4;
 sheet[3][7] = 5;
 sheet[4][7] = 2;
 sheet[5][7] = 6;
 sheet[6][7] = 7;
 sheet[3][8] = 8;
 sheet[6][8] = 6;
}

void hardsheet(int sheet[9][9]){
 setzero(sheet);
 sheet[0][0] = 6;
 sheet[0][1] = 3;
 sheet[4][1] = 8;
 sheet[6][1] = 7;
 sheet[8][1] = 9;
 sheet[0][2] = 8;
 sheet[1][2] = 9;
 sheet[3][2] = 6;
 sheet[6][2] = 2;
 sheet[8][2] = 4;
 sheet[3][3] = 8;
 sheet[8][3] = 2;
 sheet[2][4] = 7;
 sheet[3][4] = 9;
 sheet[5][4] = 4;
 sheet[6][4] = 3;
 sheet[2][5] = 2;
 sheet[4][5] = 7;
 sheet[4][6] = 9;
 sheet[2][7] = 6;
 sheet[4][7] = 4;
 sheet[0][8] = 4;
 sheet[1][8] = 5;
 sheet[8][8] = 6;
}
