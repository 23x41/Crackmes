/* ============================================================
   0x8A7 :: WAYNE SECURITY -- CHALLENGE 3: RIDDLER'S MAZE
   ------------------------------------------------------------
   Same family of bugs as Parts 1-2, but now compiled as a full
   modern hardened binary:
     - Stack canary   (-fstack-protector-all)
     - NX stack       (-z noexecstack, default anyway)
     - PIE + ASLR     (-pie -fPIE)  -> every address randomizes
                                       on every run

   NX means you cannot jump into shellcode you place on the
   stack. PIE means you don't know open_batcave()'s real address
   ahead of time -- it moves every run, along with everything
   else in the binary. ASLR randomizes the stack too.

   Two bugs remain, and one extra leak is needed:
     1) riddle_leak() - same raw write() overread as Part 2.
        Used to leak the stack canary (same trick, same reuse
        logic -- canary is still constant for the process).
     2) The SAME leak call also reads far enough to leak a
        saved return address off the stack -- a return address
        that points back into main(), i.e. INTO THE BINARY
        ITSELF. Since PIE only randomizes the *base* address
        (everything keeps the same fixed offset from that base),
        leaking any one code address lets you compute the base:
            base = leaked_address - known_static_offset
        and from there:
            open_batcave_runtime = base + open_batcave_static_offset
     3) check_password() has the classic overflow. Payload =
        padding + real_canary + filler + open_batcave_runtime.

   No shellcode is ever placed on the stack (satisfies NX) and
   no address is hardcoded (satisfies ASLR/PIE) -- you compute
   everything fresh from the leak, every single run.
   ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void open_batcave(void) {
    printf("\n[BATCOMPUTER]: Access Granted. The Batcave doors slide open...\n");
    printf("Riddle solved. Nygma has no comeback for this one.\n");
    printf("FLAG{0x8A7_P1E_L34K_4SLR_BYP4SS}\n");
    fflush(stdout);
    system("/bin/sh");
}

void riddle_leak(void) {
    char name[32];
    printf("Riddle me this: what's your name? ");
    fflush(stdout);
    ssize_t n = read(0, name, sizeof(name));
    if (n <= 0) return;
    printf("A pleasure, '");
    /* Leaks canary AND a saved return address (-> PIE base) in
       one shot: raw write(), no NUL-termination stop. */
    write(1, name, 64);
    printf("'.\n");
}

void check_password(void) {
    char buffer[64];

    printf("Enter the Maze access code: ");
    fflush(stdout);

    ssize_t n = read(0, buffer, 700);      /* <-- the vulnerability */
    if (n <= 0) return;
    if (n < (ssize_t)sizeof(buffer)) buffer[n] = '\0';

    if (strncmp(buffer, "Wh4t_Am_1", 9) == 0) {
        printf("Correct code -- but a riddle needs a cleverer answer than that.\n");
    } else {
        printf("Wrong! The maze walls close in...\n");
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=========================================\n");
    printf("   WAYNE ENTERPRISES SECURITY -- 0x8A7\n");
    printf("        RIDDLER'S MAZE TERMINAL\n");
    printf("=========================================\n");
    riddle_leak();
    check_password();
    printf("[LOG] Session ended.\n");
    return 0;
}
