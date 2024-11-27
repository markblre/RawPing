#include <stdio.h>

void display(char *buffer, int length) {
    for (int i = 0; i < length; i+=2) {
        // Affiche deux octets à la fois, en hexadécimal
        printf("%02x%02x ", (unsigned char)buffer[i], (unsigned char)buffer[i + 1]);
        
        // Ajoute un retour à la ligne tous les 16 octets
        if ((i + 1) % 16 == 15) {
            printf("\n");
        }
    }

    // Ajoute un retour à la ligne si la dernière ligne n'était pas complète
    if (length % 16 != 0) {
        printf("\n");
    }
}

int main() {
    // Exemple d'utilisation avec les 50 premiers entiers
    char buffer[50];
    for (int i = 0; i < 50; i++) {
        buffer[i] = i;
    }

    // Appel de la fonction pour afficher les octets
    display(buffer, 50);
    
    return 0;
}