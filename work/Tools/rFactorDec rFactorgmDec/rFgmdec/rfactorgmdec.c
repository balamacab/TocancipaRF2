/*
    Copyright 2009 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl-2.0.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef WIN32
    #include <direct.h>
    #define PATHSLASH   '\\'
    #define MAKEDIR(x)  mkdir(x)   
#else
    #include <unistd.h>
    #define PATHSLASH   '/'
    #define MAKEDIR(x)  mkdir(name, 0755)
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;



#define VER         "0.2b"



u8 *create_dir(u8 *name);
u8 *get_ext(u8 *fname);
u8 *isi_gm_init(void);
void isi_gm_crypt(u8 *output, u8 *input, int len, u8 *key, int keynum, int encrypt);
void fd_dump(u8 *fname, u8 *buff, int len);
u8 *fd_load(u8 *fname, int *fsize);
int get_num(u8 *str);
void std_err(void);



int     overwrite   = 0;



int main(int argc, char *argv[]) {
    int     i,
            buffsz,
            datasz,
            keynum  = 0x000b2023,
            encrypt = 0;
    u8      *buff,
            *data,
            *fin,
            *fout,
            *key,
            *p;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    fputs("\n"
        "ISI rFactor //[[gM files decrypter/encrypter "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stderr);

    if(argc < 3) {
        fprintf(stderr, "\n"
            "Usage: %s [options] <input_file> <output_file>\n"
            "\n"
            "Options:\n"
            "-o      overwrite output file if already exists without prompting\n"
            "-k KEYN specify a keynum value different than the default one (0x%08x)\n"
            "        keynum is the NEEDED encryption number which is game dependent:\n"
            "        0            rFator\n"
            "        0x000b2023   Arca Sim Racing\n"
            "        0x000b2023   ACTC Simulador Turismo Carretera\n"
            "        0x000dfc2a   Top Race Simulador 2009\n"
            "        0x0001be0b   Superleague Formula\n"
            "-e      force the encryption of input_file (default is decryption)\n"
            "        at the moment only ARCA requires encrypted files to work, so if you\n"
            "        have modified the files of this game try to use -e on them\n"
            "\n"
            "input and output file can be also the same, so it will be overwritten\n"
            "use - as output_file for selecting stdout\n"
            "\n", argv[0], keynum);
        exit(1);
    }

    argc -= 2;
    for(i = 1; i < argc; i++) {
        if(((argv[i][0] != '-') && (argv[i][0] != '/')) || (strlen(argv[i]) != 2)) {
            printf("\nError: recheck your options (%s is not valid)\n", argv[i]);
            exit(1);
        }
        switch(argv[i][1]) {
            case 'o': overwrite = 1;                    break;
            case 'k': keynum    = get_num(argv[++i]);   break;
            case 'e': encrypt   = 1;                    break;
            default: {
                printf("\nError: wrong command-line argument (%s)\n\n", argv[i]);
                exit(1);
                break;
            }
        }
    }

    fin  = argv[argc];
    fout = argv[argc + 1];

    buff = fd_load(fin, &buffsz);
    fprintf(stderr, "- %d bytes loaded\n", buffsz);

    if(encrypt) {
        buff = realloc(buff, 45 + buffsz);
        if(!buff) std_err();
        data   = buff + 45;
        datasz = buffsz;
        buffsz += 45;
        memmove(data, buff, datasz);
        memcpy(buff, "//[[gMb1.002f (c)2007    ]] [[\xe7\x22  C@+     ]]\n", 45);

            // rFactor:
            //"\x2f\x2f\x5b\x5b\x67\x4d\x62\x31\x2e\x30\x30\x30\x66\x20\x28\x63"
            //"\x29\x32\x30\x30\x35\x20\x20\x20\x20\x5d\x5d\x20\x5b\x5b\xc7\x02"
            //"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5d\x5d\x0a"
    } else {
        if(buffsz < 45) {
            fprintf(stderr, "\nError: input file is too small\n");
            exit(1);
        }
        if(memcmp(buff, "//[[gM", 6)) {
            fprintf(stderr, "\nError: input file is not a //[[gM file\n");
            exit(1);
        }
        if(buff[6] == 'a') {
            fprintf(stderr, "\nError: input file is not an encrypted //[[gM file (%c)\n", buff[6]);
            exit(1);
        }
        if(memcmp(buff + 7, "1.000", 5)) {  // useless, do nothing
            *(u32 *)(buff + 30) += 0xdfdfdfe0;
            *(u32 *)(buff + 34) += 0xdfdfdfe0;
            *(u32 *)(buff + 38) += 0xdfdfdfe0;
        }

        data = buff + 45;
        p = strchr(buff, '\n');
        if(p) data = p + 1;
        datasz = buffsz - (data - buff);
    }

    fprintf(stderr, "- start decryption of a type '%c' file from offset %d with keynum 0x%08x\n", buff[6], data - buff, keynum);
    key = isi_gm_init();

        // Arca Sim Racing uses 0x000b2023
        // rFactor uses 0x00000000
        // ACTC/Simulador Turismo Carretera uses 0x000b2023
        // at the moment doesn't seem possible to autoguess this value from the input file without errors
    isi_gm_crypt(data, data, datasz, key, keynum, encrypt);

    if(encrypt) {   // ufff boring
        data   = buff;
        datasz = buffsz;
    }

    if(!strcmp(fout, "-")) {
        fwrite(data, 1, datasz, stdout);
    } else {
        fd_dump(fout, data, datasz);
    }
    free(buff);
    fprintf(stderr, "- done\n");
    return(0);
}



u8 *create_dir(u8 *name) {
    struct stat xstat;
    int     n,
            namelen;
    u8      *p;

    namelen = strlen(name);

    p = strchr(name, ':');
    if(p) {
        p++;
    } else {
        p = name;
    }
    if((*p == '/') || (*p == '\\')) p++;

    for(; (p - name) < namelen; p++) {
        n = strcspn(p, "/\\");
        if(!n) continue;

        p += n;
        if(!*p) continue;
        *p = 0;

        if(stat(name, &xstat) < 0) {
            fprintf(stderr, "- create folder %s\n", name);
            MAKEDIR(name);
        } else if(!S_ISDIR(xstat.st_mode)) {
            fprintf(stderr, "\nError: %s is not a folder\n", name);
            exit(1);
        }

        *p = PATHSLASH;
    }
    return(name);
}



u8 *get_ext(u8 *fname) {
    u8      *p;

    p = strrchr(fname, '.');
    if(p) return(p + 1);
    return(fname + strlen(fname));
}



u8 *isi_gm_init(void) {
    static u8   key[256];
    int     tmp[256],
            a,
            b,
            c,
            d;

    memset(tmp, 0, sizeof(tmp));
    d = 12;
    c = 5;
    for(b = 0; b < 256; b++) {
        a = (d * b) + c;
        while(tmp[a & 0xff]) {
            a++;
            c++;
        }
        a = (d * b) + c;
        key[b & 0xff] = a;
        tmp[a & 0xff] = a + 1;
        d = (d + 1) % 35;
        c = (c + 2) % 9;
    }
    return(key);
}



void isi_gm_crypt(u8 *output, u8 *input, int len, u8 *key, int keynum, int encrypt) {
    int     i,
            c;

    c = keynum;
    if(encrypt) {
        for(i = 0; i < len;) {
            output[i] = input[i] + key[c & 0xff];
            i++;
            c = (c << 1) + ((c < 0) ? 1 : 0);
            c = (c & 0xffffff00) | (((c | i) - (c & i)) & 0xff);
        }
    } else {
        for(i = 0; i < len;) {
            output[i] = input[i] - key[c & 0xff];
            i++;
            c = (c << 1) + ((c < 0) ? 1 : 0);
            c = (c & 0xffffff00) | (((c | i) - (c & i)) & 0xff);
        }
    }
}



void fd_dump(u8 *fname, u8 *buff, int len) {
    FILE    *fd;

    fname = create_dir(fname);
    fprintf(stderr, "- write file %s\n", fname);
    if(!overwrite) {
        fd = fopen(fname, "rb");
        if(fd) {
            fclose(fd);
            printf("- file already exists, do you want to overwrite it (y/N)?\n  ");
            fflush(stdin);
            if(tolower(fgetc(stdin)) != 'y') exit(1);
        }
    }
    fd = fopen(fname, "wb");
    if(!fd) std_err();
    fwrite(buff, 1, len, fd);
    fclose(fd);
}



u8 *fd_load(u8 *fname, int *fsize) {
    struct  stat xstat;
    FILE    *fd;
    u8      *buff;

    fprintf(stderr, "- open file %s\n", fname);
    fd = fopen(fname, "rb");
    if(!fd) std_err();
    fstat(fileno(fd), &xstat);
    buff = malloc(xstat.st_size);
    if(!buff) std_err();
    fread(buff, 1, xstat.st_size, fd);
    fclose(fd);
    *fsize = xstat.st_size;
    return(buff);
}



int get_num(u8 *str) {
    int     offset;

    if(!str[0]) return(0);
    if((strlen(str) > 2) && (str[0] == '0') && (tolower(str[1]) == 'x')) {
        sscanf(str + 2, "%x", &offset);
    } else {
        sscanf(str, "%d", &offset);
    }
    return(offset);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}


