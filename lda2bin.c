/*
   DEC PDP-11 LDA to absolute memory image conversion.

   Author: Paul Hardy

   This code is in the public domain.

   Synopsis: lda2bin [-v] < INFILE.LDA > outfile.bin

   This program creates a memory image of a file from a DEC absolute
   loader image and writes out a binary memory map, starting at the
   execution address for the absolute loader file and ending with
   the last byte of the absolute loader image.

   The optional "-v" flag produces verbose output.  This will print
   information about every LDA frame to stderr.
*/

#include <stdio.h>
#include <stdlib.h>


int main (int argc, char *argv[]) {
   int verbose=0;  /* set to 1 for verbose output */
   /*
      These are the states that we go through reading an absolute
      loader formatted block; the FINAL state is reached when the
      Byte Count (BC) is 6.
   */
   enum STATES {LEADL, LEADH,  /* leading 1, leading 0          */
                BCL, BCH,      /* byte count low, high          */
                ADL, ADH,      /* address count low, high       */
                DATA,          /* data bytes                    */
                CHKSUM,        /* block checksum byte           */
                FINAL};        /* final block; byte count is 6  */
   enum STATES block_state = LEADL;

   uint16_t leader, byte_count; /* LDA header */
   int32_t  address;            /* LDA header */
   uint8_t  chksum; /* checksum for a block */
   uint8_t  byte_sum; /* sum of all bytes in block */
   uint8_t  data_byte;
   int      ch;  /* input character */
   uint16_t low_byte, high_byte; /* 2 bytes in a word  */
   int32_t  low_mem  = 0xDFFF;  /* lowest address used */
   int32_t  high_mem = 0x0000; /* highest address used */
   uint32_t exec_address=0; /* address where execution begins */

   uint16_t mem_map [64 * 1024]; /* 64kbyte memory map */
   int      i; /* loop variable */

   if (argc > 1) {
      if (argv[1][0] == '-' && argv[1][1] == 'v') verbose = 1;
   }

   for (i = 0; i < 64 * 1024; i++) mem_map [i] = 0;

   while (!feof (stdin)) {
      if ((ch = fgetc (stdin)) >= 0) {
         ch &= 0xFF; /* just keep the low byte */
         switch (block_state) {
            case LEADL:
               if (ch == 1) {
                  byte_sum = ch;
                  block_state = LEADH; /* got leader byte with value 1 */
               }
               break;
            case LEADH:
               if (ch == 0) {
                  block_state = BCL; /* got leader byte with value 0 */
               }
               else {
                  block_state = LEADL; /* try all over again */
               }
               break;
            case BCL:
               byte_count = ch;   /* got low byte count      */
               byte_sum  += ch;
               block_state = BCH; /* now get high byte count */
               break;
            case BCH:
               byte_count |= ch << 8; /* got high byte count      */
               byte_sum  += ch;
               block_state = ADL;     /* now get low address byte */
               break;
            case ADL:
               address = ch;       /* got low address byte      */
               byte_sum  += ch;
               block_state = ADH;  /* now get high address byte */
               break;
            case ADH:
               address |= ch << 8; /* got high address byte */
               if (address < low_mem) low_mem = address;
               byte_sum  += ch;
               byte_count -= 6; /* already read the 6 header bytes */
               if (byte_count > 0) {
                  block_state = DATA; /* now get data bytes    */
               }
               else {
                  exec_address = address;
                  block_state  = CHKSUM;
               }
               if (verbose == 1) {
                  fprintf (stderr,
                           "   Start: 0%05o, Bytes: %02o octal.\n",
                           address, byte_count);
               }
               break;
            case DATA:
               mem_map [address] = ch;
               byte_sum += ch;
               /* If new high memory location is < 56k (device space) */
               if ((address > high_mem) && (address < (int)0xE000)) {
                  high_mem = address;
                  if (high_mem >= 020000 && verbose == 1) {
                     fprintf (stderr, "    Mem @ 0%05o = %03o\n",
                        high_mem, mem_map [high_mem]);
                  }
               }
               address++;
               byte_count--;
               if (byte_count == 0) block_state = CHKSUM;
               break;
            case CHKSUM:
               byte_sum += ch;
               if (byte_sum != 0) {
                  fprintf (stderr,
                           "\n   Checksum not zero when 0x%02X added.\n",
                           ch);
               }
               block_state = LEADL; /* look for new block */
               break;
            case FINAL:
               break;
         }  /* switch */
      }  /* if fgetc */
   }  /* while */

   if (exec_address == 0) exec_address = low_mem;

   fprintf (stderr, "   Memory range is [0%05o, 0%05o]\n", low_mem, high_mem);
   fprintf (stderr, "   Execution begins at 0%05o\n", exec_address);

   /*
      The lines below that are commented out could be used to create
      an output LDA file with one contiguous output frame, followed
      by one execution address frame of 6 bytes plus checksum.
   */
// putchar (1); putchar (0);  /* leader */
   byte_sum = 1;
   byte_count = high_mem - low_mem + 1;
// putchar (byte_count & 0xFF);  byte_sum += byte_count & 0xFF;
// putchar (byte_count >> 8);    byte_sum += byte_count >> 8;
// putchar (low_mem & 0xFF);     byte_sum += low_mem    & 0xFF;
// putchar (low_mem >> 8);       byte_sum += low_mem    >> 8;

   for (i = low_mem; i <= high_mem; i++) {
      putchar (mem_map [i]);
      byte_sum += mem_map [i];
   }
   byte_sum = -byte_sum;
// putchar (byte_sum & 0xFF);

   /* Print exec block */
// putchar (1); putchar (0);  /* leader */
// byte_sum = 1;
// putchar (6); /* byte count low  */
// byte_sum += 6;
// putchar (0); /* byte count high */
// putchar (low_mem & 0xFF);  byte_sum += low_mem & 0xFF;
// putchar (low_mem >> 8);    byte_sum += low_mem >> 8;
// byte_sum = -byte_sum;
// putchar (byte_sum & 0xFF);

   exit (EXIT_SUCCESS);
}
