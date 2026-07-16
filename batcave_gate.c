/* ============================================================
   0x8A7 :: WAYNE SECURITY -- CHALLENGE 1: BATCAVE GATE
   ------------------------------------------------------------
   A simple stack buffer overflow. No canary, no PIE.
   Goal: overwrite the saved return address so control jumps
   straight into open_batcave(), skipping the password check
   entirely.
   ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Never called anywhere in normal program flow.
   It only exists to be jumped to via the overflow. */
void open_batcave(void) {
    printf("\n[BATCOMPUTER]: Access Granted. The Batcave doors slide open...\n");
    printf("You find the Dark Knight's terminal waiting for you.\n");
    printf("FLAG{0x8A7_R3T2W1N_N0_PR0T3CT10N5}\n");
    fflush(stdout);
    system("/bin/sh");
}

void check_password(void) {
    char buffer[64];

    printf("Enter the Batcave access code: ");
    fflush(stdout);

    /* Unsafe: reads far more bytes than buffer[] can hold, straight
       off the wire and onto the stack. No bounds check. */
    ssize_t n = read(0, buffer, 512);      /* <-- the vulnerability */
    if (n <= 0) return;
    if (n < (ssize_t)sizeof(buffer)) buffer[n] = '\0';

    if (strncmp(buffer, "N1ghtw1ng_R3turns", 17) == 0) {
        printf("Correct code -- but Alfred says that's not how you get in tonight.\n");
    } else {
        printf("Access Denied. The Batcave remains sealed.\n");
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=========================================\n");
    printf("   WAYNE ENTERPRISES SECURITY -- 0x8A7\n");
    printf("        BATCAVE ACCESS TERMINAL\n");
    printf("=========================================\n");
    check_password();
    printf("[LOG] Session ended.\n");
    return 0;
}
