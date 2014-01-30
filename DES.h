#ifndef _DES_H
#define _DES_H

typedef char bool;
void EncryptDES(bool key[56], bool outBlk[64], bool inBlk[64], int verbose);
void DecryptDES(bool key[56], bool outBlk[64], bool inBlk[64], int verbose);

#endif
