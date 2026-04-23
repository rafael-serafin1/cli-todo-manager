#pragma once
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

void trim(char *str) {
    if (str == NULL) return;

    int start = 0;
    int end = strlen(str) - 1;
    
    while (str[start] != '\0' && isspace((unsigned char)str[start])) 
        start++;

    while (end >= start && isspace((unsigned char)str[end]))
        end--;

    register int i;
    for (i = 0; i <= (end - start); i++) 
        str[i] = str[start + i];
    str[i] = '\0';
}


void buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


char ask() {
    char c;
    scanf("%c", &c);
    buffer();
    printf("Pressione a tecla 'ENTER' para continuar.\n");
    return c;
}

void configurar_locale(void) {
    #if defined(_WIN32)
    system("chcp 65001 > nul");
    #endif

    const char *locais[] = {
        "pt_BR.UTF-8",
        "pt_BR.utf8",
        "Portuguese_Brazil.1252",
        "Portuguese",
        ""
    };
    register int i; 
    for (i = 0; i < 5; i++) {
        const char *r = setlocale(LC_ALL, locais[i]);
        if (r != NULL) {
            printf("Locale ativo: %s\n", r);
            return;
        }
        free(r);
    }
    printf("Aviso: Locale não pôde ser configurado.\n");
    free(locais);
}