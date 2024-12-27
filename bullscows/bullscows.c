#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    char s[5], g[5]; int b, c, i, j;
    srand(time(0));
    for (i = 0; i < 4; s[i++] = '0' + rand() % 10) for (j = 0; j < i; j++) if (s[j] == s[i]) i--;
    s[4] = '\0';
    while (1) {
        printf("> "); scanf("%4s", g);
        for (b = c = i = 0; i < 4; i++) {
            if (s[i] == g[i]) b++;
            for (j = 0; j < 4; j++) if (i != j && s[i] == g[j]) c++;
        }
        if (b == 4) { printf("You win!\n"); break; }
        printf("%d bulls, %d cows\n", b, c);
    }
    return 0;
}
