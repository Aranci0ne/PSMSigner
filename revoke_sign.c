/*
 * revoke_sign.c
 * Copyright 2015 Yifan Lu
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

const unsigned char rsa_n[] = {0xB1, 0x97, 0xEF, 0x1F, 0x47, 0x30, 0xF3, 0xC9, 0x66, 0xF4, 0xF3, 0xD2, 0xFD, 0xB2, 0xC2, 0x96, 0x30, 0xEE, 0x08, 0x60, 0x90, 0x13, 0x55, 0x5F, 0xEA, 0xB0, 0x63, 0xEA, 0x2F, 0x95, 0xED, 0x82, 0x3A, 0xA5, 0x49, 0x8E, 0x9D, 0x57, 0x74, 0x07, 0x5A, 0xD7, 0x62, 0xAA, 0x6E, 0x01, 0xF6, 0x1F, 0x1C, 0xCB, 0x7D, 0xB9, 0xF9, 0x71, 0x0E, 0x44, 0xC0, 0xBC, 0xA0, 0x4A, 0xE5, 0x95, 0x29, 0x7B, 0xCA, 0x8B, 0xEE, 0xF7, 0x9E, 0xBF, 0x0F, 0x99, 0x5A, 0x2C, 0xB1, 0xAA, 0x46, 0x4C, 0x85, 0x3D, 0xC3, 0x5E, 0xAF, 0xE0, 0xCE, 0x8C, 0xEF, 0x7D, 0x6C, 0x37, 0xC9, 0x4D, 0x35, 0x75, 0x6D, 0x21, 0xEE, 0xC1, 0xCF, 0x68, 0xA3, 0x79, 0x89, 0x9F, 0xA0, 0x0E, 0x5A, 0xE8, 0x98, 0xDD, 0xA8, 0x2E, 0x08, 0x86, 0x31, 0x36, 0x04, 0xA5, 0x0E, 0xC7, 0xDA, 0xBE, 0x39, 0x82, 0x0D, 0x76, 0xB7, 0x43, 0xCE, 0x61, 0x14, 0xEC, 0x49, 0x9E, 0x7E, 0xC7, 0x94, 0xCE, 0x17, 0xEA, 0xB3, 0x71, 0x16, 0x2F, 0x72, 0xE3, 0xC2, 0xEA, 0x52, 0xB0, 0xF2, 0x5B, 0x0D, 0x0B, 0x6E, 0xCF, 0xB4, 0xED, 0xBA, 0x8D, 0x2E, 0x86, 0x07, 0x5E, 0xD3, 0x28, 0x65, 0xC3, 0x50, 0x1E, 0xA3, 0x66, 0x1A, 0xD2, 0x07, 0x1B, 0x1E, 0x11, 0xB8, 0x5C, 0xD9, 0xE2, 0x17, 0x6E, 0xA2, 0xB8, 0xCD, 0xB2, 0x64, 0x8A, 0x1E, 0x24, 0x12, 0xCF, 0xEC, 0x9D, 0x26, 0x02, 0x56, 0xB4, 0xB2, 0x4E, 0xB4, 0x2D, 0x84, 0x33, 0xCC, 0xE3, 0xF3, 0x34, 0xAD, 0x9A, 0x50, 0xE2, 0x31, 0x3C, 0x80, 0xA8, 0xA3, 0xC7, 0x8C, 0x43, 0x69, 0xE0, 0x50, 0x4B, 0xA0, 0x5F, 0x3B, 0x3C, 0x84, 0x8B, 0xB1, 0xBC, 0x9F, 0xE1, 0xD4, 0xDD, 0x5A, 0x4D, 0xF9, 0x4E, 0xF9, 0xEC, 0x53, 0xFF, 0xAD, 0x19, 0x5C, 0x87, 0x6D, 0x53, 0x5C, 0x10, 0x0C, 0xEF};
const unsigned char rsa_e[] = {0x00, 0x01, 0x00, 0x01};
const unsigned char rsa_p[] = {0xEB, 0xD1, 0x5F, 0x73, 0x6C, 0x39, 0x50, 0x22, 0x75, 0x70, 0x7C, 0x6E, 0xF1, 0xD4, 0x7D, 0x57, 0x8A, 0x2D, 0x63, 0x9E, 0xEF, 0x04, 0x91, 0x4D, 0xD9, 0x2B, 0xDB, 0xF2, 0x1D, 0xE4, 0x26, 0x73, 0xBD, 0x2D, 0xF5, 0x3F, 0x45, 0x83, 0x1E, 0x5D, 0x38, 0x09, 0x4A, 0x49, 0x61, 0xF5, 0x3C, 0x3F, 0xCD, 0x73, 0xCE, 0x30, 0x44, 0x27, 0xC6, 0x2E, 0xCB, 0xA1, 0xCA, 0xC3, 0x77, 0x5E, 0xF5, 0xDE, 0xF4, 0x13, 0x4E, 0xCA, 0x53, 0xDF, 0x05, 0x9D, 0xDC, 0x02, 0x76, 0xE6, 0x5D, 0x01, 0x3F, 0x73, 0x46, 0x9D, 0xB1, 0x2D, 0x1B, 0x0B, 0xFA, 0x22, 0xDF, 0x95, 0x9D, 0x45, 0x0B, 0xAF, 0xDE, 0xB8, 0xBC, 0xE5, 0x2F, 0xFD, 0xB9, 0x78, 0x50, 0xC4, 0x2C, 0xE7, 0x62, 0x6C, 0x9F, 0x15, 0x11, 0xB6, 0xCF, 0x86, 0x3D, 0x9D, 0xE0, 0x00, 0x5D, 0x52, 0x53, 0x60, 0x15, 0xC6, 0xF5, 0x5E, 0x8F, 0xDB};
const unsigned char rsa_q[] = {0xC0, 0xCA, 0xE6, 0x7F, 0x52, 0xDA, 0xB2, 0x25, 0x3F, 0xB2, 0x28, 0xE1, 0xD1, 0xF3, 0x93, 0x57, 0xA4, 0xD7, 0x79, 0xF4, 0xAB, 0x3B, 0x19, 0xE9, 0x8A, 0xAF, 0x50, 0xB3, 0xFA, 0x13, 0x49, 0x9E, 0x54, 0x49, 0x38, 0x79, 0x16, 0xBB, 0xA7, 0x17, 0x47, 0x57, 0xE4, 0x8B, 0x32, 0xE1, 0x78, 0xD8, 0x29, 0x67, 0xC8, 0x57, 0x05, 0x65, 0xF4, 0xB9, 0xC0, 0x5D, 0x1A, 0x91, 0x41, 0x38, 0x87, 0x91, 0xB1, 0x68, 0x9F, 0x62, 0x8C, 0x61, 0x8C, 0x90, 0x7C, 0x35, 0x1E, 0xE1, 0x3F, 0x64, 0xB9, 0x7E, 0x9C, 0x02, 0xAA, 0x7C, 0x5A, 0x30, 0x5A, 0x8A, 0xFC, 0x6C, 0xF1, 0xC8, 0x44, 0xAC, 0xCF, 0x2B, 0xCD, 0x99, 0xEC, 0xEB, 0x66, 0xD1, 0xD4, 0x1F, 0x7E, 0xFC, 0x85, 0xF8, 0x54, 0x88, 0x87, 0x03, 0x58, 0xDF, 0x3F, 0xD7, 0xA2, 0x0A, 0x5F, 0xB5, 0x94, 0xC8, 0x61, 0x9E, 0x73, 0x48, 0x1D, 0x7D};
const unsigned char rsa_d[] = {0xA4, 0xA2, 0x97, 0xFF, 0xE4, 0x0C, 0x82, 0x83, 0xFC, 0x19, 0x2B, 0x0C, 0x37, 0x3A, 0x87, 0xDF, 0x9A, 0x1E, 0xA0, 0xCF, 0x7B, 0xE1, 0x20, 0x6D, 0x59, 0xF1, 0x09, 0xBF, 0x29, 0x3C, 0x11, 0x41, 0x80, 0x42, 0x88, 0x6D, 0x58, 0x79, 0xA1, 0x4B, 0xCC, 0x30, 0x0D, 0xFC, 0xB7, 0x1C, 0xC4, 0x60, 0xD5, 0x5D, 0xE4, 0x01, 0x0B, 0x38, 0x7E, 0xA4, 0xC8, 0x14, 0x55, 0x78, 0xF9, 0x5A, 0x8D, 0x69, 0x80, 0x3C, 0xB7, 0x5D, 0x00, 0xE2, 0x58, 0x80, 0xAE, 0xB2, 0xEE, 0x45, 0x4F, 0x04, 0x53, 0x25, 0x0D, 0xBF, 0xF2, 0xC6, 0x32, 0x55, 0x3F, 0x13, 0x83, 0xC6, 0x37, 0xAC, 0x86, 0xD9, 0x84, 0x41, 0x47, 0xC8, 0x8E, 0x11, 0x90, 0x32, 0xF4, 0x9D, 0x71, 0x50, 0xBD, 0x80, 0x11, 0x6E, 0xB3, 0x21, 0x71, 0xAF, 0x8D, 0x61, 0x69, 0x6E, 0xE6, 0xC3, 0xE9, 0x8A, 0x1D, 0x5F, 0xEE, 0x41, 0x56, 0xE4, 0x4C, 0xCF, 0xD9, 0x9C, 0x46, 0x8D, 0xD9, 0x9A, 0xE5, 0xEC, 0x57, 0xC2, 0x53, 0x05, 0xDA, 0xDE, 0xAC, 0x8C, 0xAF, 0x08, 0xF4, 0xD5, 0xB6, 0xC0, 0x5D, 0x71, 0x2A, 0x74, 0xCF, 0x59, 0x17, 0x75, 0x08, 0x5C, 0x22, 0xF3, 0x8A, 0x1B, 0x4D, 0x0C, 0xA3, 0xC0, 0x6E, 0x97, 0x15, 0xA1, 0x05, 0xC7, 0x7C, 0x8B, 0x57, 0x54, 0x57, 0x87, 0x8D, 0x7A, 0x95, 0x08, 0xE5, 0x5C, 0x29, 0x32, 0x35, 0x3E, 0x01, 0x25, 0x33, 0xA4, 0x3B, 0x56, 0xC5, 0x6C, 0x53, 0x8A, 0x49, 0x18, 0xE9, 0x8A, 0xCB, 0x68, 0x05, 0x6C, 0xAA, 0x85, 0xE1, 0xE9, 0xE2, 0x0C, 0xAE, 0xA0, 0x74, 0x68, 0xAD, 0x6D, 0x5A, 0x69, 0xAE, 0x55, 0xC8, 0xAD, 0x2D, 0x8F, 0xF1, 0x93, 0x9F, 0x4F, 0xE7, 0xD2, 0x32, 0x32, 0xDB, 0xAE, 0xBF, 0xA6, 0x24, 0x47, 0x36, 0x9E, 0x9E, 0x0B, 0x60, 0x59, 0x44, 0x1D, 0xC2, 0x60, 0xB6, 0x49};

int
hash_with_date (const char *path, FILE *fout, unsigned char hash[SHA256_DIGEST_LENGTH])
{
    FILE *fp;
    SHA256_CTX sha;
    char buffer[1024];
    int len;
    time_t rawtime;
    struct tm *timeinfo;

    if (!SHA256_Init (&sha))
    {
        return -1;
    }
    if ((fp = fopen (path, "r")) == NULL)
    {
        return -1;
    }

    // read in the original data
    while ((len = fread (buffer, 1, 1024, fp)) > 0)
    {
        if (fwrite (buffer, 1, len, fout) < len)
        {
            return -1;
        }
        if (!SHA256_Update (&sha, buffer, len))
        {
            return -1;
        }
    }

    // add date stamp
    time (&rawtime);
    timeinfo = gmtime (&rawtime);
    len = strftime (buffer, 1024, "%Y%m%d", timeinfo);
    if (len < 1)
    {
        return -1;
    }
    if (fwrite (buffer, 1, len+1, fout) < len)
    {
        return -1;
    }
    if (!SHA256_Update (&sha, buffer, len+1))
    {
        return -1;
    }

    if (!SHA256_Final (hash, &sha))
    {
        return -1;
    }

    return 0;
}

int
generate_padding (unsigned char data[32], unsigned char output[256])
{
    int i;

    *(int *)&output[0] = 0xFFFF0100;
    memset (&output[4], 0xFF, 200);
    for (i = 204; i < 224; i += 4)
    {
        *(int *)&output[i] = rand ();
    }
    memcpy (&output[224], data, 32);

    return 0;
}

int
main (int argc, const char *argv[])
{
    RSA *rsa;
    FILE *fout;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char padding[256];
    unsigned char sig[256];

    srand(time(NULL));

    if (argc < 3)
    {
        fprintf (stderr, "usage: %s input output\n"
                         "  input   NSXVID-PSS.VT.WW-GLOBAL.xml file\n"
                         "  output  xml with current UTC date signed\n", argv[0]);
        return 1;
    }

    if ((fout = fopen (argv[2], "w")) == NULL)
    {
        perror ("output");
        return 1;
    }

    if ((rsa = RSA_new ()) == NULL)
    {
        ERR_print_errors_fp (stderr);
        return 1;
    }
    rsa->n = BN_bin2bn (rsa_n, sizeof (rsa_n), NULL);
    rsa->e = BN_bin2bn (rsa_e, sizeof (rsa_e), NULL);
    rsa->p = BN_bin2bn (rsa_p, sizeof (rsa_p), NULL);
    rsa->q = BN_bin2bn (rsa_q, sizeof (rsa_q), NULL);
    rsa->d = BN_bin2bn (rsa_d, sizeof (rsa_d), NULL);
    if (!rsa->n || !rsa->e || !rsa->p || !rsa->q || !rsa->d)
    {
        ERR_print_errors_fp (stderr);
        return 1;
    }

    if (hash_with_date (argv[1], fout, hash) < 0)
    {
        perror ("sha256");
        ERR_print_errors_fp (stderr);
        goto error;
    }

    if (generate_padding (hash, padding) < 0)
    {
        fprintf (stderr, "cannot generate RSA padding\n");
        goto error;
    }

    if (RSA_private_encrypt (sizeof (padding), padding, sig, rsa, RSA_NO_PADDING) < 0)
    {
        ERR_print_errors_fp (stderr);
        return 1;
    }

    if (fwrite (sig, 1, sizeof (sig), fout) < sizeof (sig))
    {
        perror ("fwrite");
        return 1;
    }

error:
    fclose (fout);
    RSA_free (rsa);
    return 0;
}