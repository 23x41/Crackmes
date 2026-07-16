/* ============================================================
   0x8A7 :: WAYNE SECURITY -- CHALLENGE 2: JOKER'S VAULT
   ------------------------------------------------------------
   Same overflow bug as Part 1, but now compiled with a stack
   canary (-fstack-protector-all). A naive overwrite corrupts
   the canary and the program aborts ("stack smashing detected").

   A second bug -- riddle_leak() -- reads a name into a stack
   buffer and echoes back RAW bytes with write(), which (unlike
   printf("%s", ...)) does not stop at a NUL byte. Overread that
   buffer and you leak the canary (and a bit more) straight off
   the stack. The canary is set once per process from TLS, so
   the same 8-byte value is valid in every stack frame of that
   process -- leak it once, reuse it anywhere.
   ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void open_batcave(void) {
    printf("\n[BATCOMPUTER]: Access Granted. The Batcave doors slide open...\n");
    printf("The Joker never saw this coming.\n");
    printf("FLAG{0x8A7_C4N4RY_L34K_4ND_R3US3}\n");
    fflush(stdout);
    system("/bin/sh");
}

void riddle_leak(void) {
    char name[32];
    printf("The Joker asks: what's your name, little bat? ");
    fflush(stdout);
    ssize_t n = read(0, name, sizeof(name));  /* fills buffer fully, no NUL */
    if (n <= 0) return;
    printf("Oh, '");
    write(1, name, 56);        /* <-- leak bug: reads past name[] on the stack */
    printf("'... how quaint.\n");
}

void check_password(void) {
    char buffer[64];

    printf("Enter the Vault access code: ");
    fflush(stdout);

    ssize_t n = read(0, buffer, 700);      /* <-- the vulnerability */
    if (n <= 0) return;
    if (n < (ssize_t)sizeof(buffer)) buffer[n] = '\0';

    if (strncmp(buffer, "H4H4H4_G0TCH4", 13) == 0) {
        printf("Correct code -- but that's not how you'll get in tonight.\n");
    } else {
        printf("Access Denied. Try again, Bat-Brain.\n");
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=========================================\n");
    printf("   WAYNE ENTERPRISES SECURITY -- 0x8A7\n");
    printf("         JOKER'S VAULT TERMINAL\n");
    printf("=========================================\n");
    riddle_leak();
    check_password();
    printf("[LOG] Session ended.\n");
    return 0;
}
