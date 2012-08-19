/*
  Copyright (C) 2006, 2009 by Werner Dittmann

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

/** Copyright (C) 2006, 2009
 *
 * @author  Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <bn.h>
#include <bnprint.h>
#include <ec/ec.h>
#include <ec/ecdh.h>
#include <zrtp/crypto/zrtpDH.h>
#include <zrtp/libzrtpcpp/ZrtpTextData.h>
#include <cryptcommon/aes.h>
#include <cryptcommon/ZrtpRandom.h>


static BigNum bnP2048 = {0};
static BigNum bnP3072 = {0};

static BigNum bnP2048MinusOne = {0};
static BigNum bnP3072MinusOne = {0};

static BigNum two = {0};

static uint8_t dhinit = 0;

typedef struct _dhCtx {
    BigNum privKey;
    BigNum pubKey;
    NistECpCurve curve;
    EcPoint pubPoint;
} dhCtx;

void randomZRTP(uint8_t *buf, int32_t length)
{
    ZrtpRandom::getRandomData(buf, length);
}

static const uint8_t P2048[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
    0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
    0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
    0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
    0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
    0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
    0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
    0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
    0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
    0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
    0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 0x98, 0xDA, 0x48, 0x36,
    0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
    0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56,
    0x20, 0x85, 0x52, 0xBB, 0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
    0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 0xF1, 0x74, 0x6C, 0x08,
    0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
    0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2,
    0xEC, 0x07, 0xA2, 0x8F, 0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
    0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18, 0x39, 0x95, 0x49, 0x7C,
    0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
    0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

static const uint8_t P3072[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
    0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
    0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
    0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
    0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
    0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
    0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
    0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
    0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
    0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
    0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 0x98, 0xDA, 0x48, 0x36,
    0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
    0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56,
    0x20, 0x85, 0x52, 0xBB, 0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
    0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 0xF1, 0x74, 0x6C, 0x08,
    0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
    0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2,
    0xEC, 0x07, 0xA2, 0x8F, 0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
    0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18, 0x39, 0x95, 0x49, 0x7C,
    0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
    0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAA, 0xC4, 0x2D, 0xAD, 0x33, 0x17, 0x0D,
    0x04, 0x50, 0x7A, 0x33, 0xA8, 0x55, 0x21, 0xAB, 0xDF, 0x1C, 0xBA, 0x64,
    0xEC, 0xFB, 0x85, 0x04, 0x58, 0xDB, 0xEF, 0x0A, 0x8A, 0xEA, 0x71, 0x57,
    0x5D, 0x06, 0x0C, 0x7D, 0xB3, 0x97, 0x0F, 0x85, 0xA6, 0xE1, 0xE4, 0xC7,
    0xAB, 0xF5, 0xAE, 0x8C, 0xDB, 0x09, 0x33, 0xD7, 0x1E, 0x8C, 0x94, 0xE0,
    0x4A, 0x25, 0x61, 0x9D, 0xCE, 0xE3, 0xD2, 0x26, 0x1A, 0xD2, 0xEE, 0x6B,
    0xF1, 0x2F, 0xFA, 0x06, 0xD9, 0x8A, 0x08, 0x64, 0xD8, 0x76, 0x02, 0x73,
    0x3E, 0xC8, 0x6A, 0x64, 0x52, 0x1F, 0x2B, 0x18, 0x17, 0x7B, 0x20, 0x0C,
    0xBB, 0xE1, 0x17, 0x57, 0x7A, 0x61, 0x5D, 0x6C, 0x77, 0x09, 0x88, 0xC0,
    0xBA, 0xD9, 0x46, 0xE2, 0x08, 0xE2, 0x4F, 0xA0, 0x74, 0xE5, 0xAB, 0x31,
    0x43, 0xDB, 0x5B, 0xFC, 0xE0, 0xFD, 0x10, 0x8E, 0x4B, 0x82, 0xD1, 0x20,
    0xA9, 0x3A, 0xD2, 0xCA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* **************
static const uint8_t P4096[] =
{
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 0x98, 0xDA, 0x48, 0x36,
0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56,
0x20, 0x85, 0x52, 0xBB, 0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 0xF1, 0x74, 0x6C, 0x08,
0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2,
0xEC, 0x07, 0xA2, 0x8F, 0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18, 0x39, 0x95, 0x49, 0x7C,
0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAA, 0xC4, 0x2D, 0xAD, 0x33, 0x17, 0x0D,
0x04, 0x50, 0x7A, 0x33, 0xA8, 0x55, 0x21, 0xAB, 0xDF, 0x1C, 0xBA, 0x64,
0xEC, 0xFB, 0x85, 0x04, 0x58, 0xDB, 0xEF, 0x0A, 0x8A, 0xEA, 0x71, 0x57,
0x5D, 0x06, 0x0C, 0x7D, 0xB3, 0x97, 0x0F, 0x85, 0xA6, 0xE1, 0xE4, 0xC7,
0xAB, 0xF5, 0xAE, 0x8C, 0xDB, 0x09, 0x33, 0xD7, 0x1E, 0x8C, 0x94, 0xE0,
0x4A, 0x25, 0x61, 0x9D, 0xCE, 0xE3, 0xD2, 0x26, 0x1A, 0xD2, 0xEE, 0x6B,
0xF1, 0x2F, 0xFA, 0x06, 0xD9, 0x8A, 0x08, 0x64, 0xD8, 0x76, 0x02, 0x73,
0x3E, 0xC8, 0x6A, 0x64, 0x52, 0x1F, 0x2B, 0x18, 0x17, 0x7B, 0x20, 0x0C,
0xBB, 0xE1, 0x17, 0x57, 0x7A, 0x61, 0x5D, 0x6C, 0x77, 0x09, 0x88, 0xC0,
0xBA, 0xD9, 0x46, 0xE2, 0x08, 0xE2, 0x4F, 0xA0, 0x74, 0xE5, 0xAB, 0x31,
0x43, 0xDB, 0x5B, 0xFC, 0xE0, 0xFD, 0x10, 0x8E, 0x4B, 0x82, 0xD1, 0x20,
0xA9, 0x21, 0x08, 0x01, 0x1A, 0x72, 0x3C, 0x12, 0xA7, 0x87, 0xE6, 0xD7,
0x88, 0x71, 0x9A, 0x10, 0xBD, 0xBA, 0x5B, 0x26, 0x99, 0xC3, 0x27, 0x18,
0x6A, 0xF4, 0xE2, 0x3C, 0x1A, 0x94, 0x68, 0x34, 0xB6, 0x15, 0x0B, 0xDA,
0x25, 0x83, 0xE9, 0xCA, 0x2A, 0xD4, 0x4C, 0xE8, 0xDB, 0xBB, 0xC2, 0xDB,
0x04, 0xDE, 0x8E, 0xF9, 0x2E, 0x8E, 0xFC, 0x14, 0x1F, 0xBE, 0xCA, 0xA6,
0x28, 0x7C, 0x59, 0x47, 0x4E, 0x6B, 0xC0, 0x5D, 0x99, 0xB2, 0x96, 0x4F,
0xA0, 0x90, 0xC3, 0xA2, 0x23, 0x3B, 0xA1, 0x86, 0x51, 0x5B, 0xE7, 0xED,
0x1F, 0x61, 0x29, 0x70, 0xCE, 0xE2, 0xD7, 0xAF, 0xB8, 0x1B, 0xDD, 0x76,
0x21, 0x70, 0x48, 0x1C, 0xD0, 0x06, 0x91, 0x27, 0xD5, 0xB0, 0x5A, 0xA9,
0x93, 0xB4, 0xEA, 0x98, 0x8D, 0x8F, 0xDD, 0xC1, 0x86, 0xFF, 0xB7, 0xDC,
0x90, 0xA6, 0xC0, 0x8F, 0x4D, 0xF4, 0x35, 0xC9, 0x34, 0x06, 0x31, 0x99,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
*************** */

ZrtpDH::ZrtpDH(const char* type) {

    uint8_t random[64];

    ctx = static_cast<void*>(new dhCtx);
    dhCtx* tmpCtx = static_cast<dhCtx*>(ctx);

    // Well - the algo type is only 4 char thus cast to int32 and compare
    if (*(int32_t*)type == *(int32_t*)dh2k) {
        pkType = DH2K;
    }
    else if (*(int32_t*)type == *(int32_t*)dh3k) {
        pkType = DH3K;
    }
    else if (*(int32_t*)type == *(int32_t*)ec25) {
        pkType = EC25;
    }
    else if (*(int32_t*)type == *(int32_t*)ec38) {
        pkType = EC38;
    }
    else {
        return;
    }

    randomZRTP(random, sizeof(random));

    if (!dhinit) {
        bnBegin(&two);
        bnSetQ(&two, 2);

        bnBegin(&bnP2048);
        bnInsertBigBytes(&bnP2048, P2048, 0, sizeof(P2048));
        bnBegin(&bnP3072);
        bnInsertBigBytes(&bnP3072, P3072, 0, sizeof(P3072));

        bnBegin(&bnP2048MinusOne);
        bnCopy(&bnP2048MinusOne, &bnP2048);
        bnSubQ(&bnP2048MinusOne, 1);

        bnBegin(&bnP3072MinusOne);
        bnCopy(&bnP3072MinusOne, &bnP3072);
        bnSubQ(&bnP3072MinusOne, 1);

        dhinit = 1;
    }

    bnBegin(&tmpCtx->privKey);
    INIT_EC_POINT(&tmpCtx->pubPoint);

    switch (pkType) {
    case DH2K:
    case DH3K:
        bnInsertBigBytes(&tmpCtx->privKey, random, 0, 256/8);
        break;

    case EC25:
        ecGetCurveNistECp(NIST256P, &tmpCtx->curve);
        ecGenerateRandomNumber(&tmpCtx->curve, &tmpCtx->privKey);
        break;

    case EC38:
        ecGetCurveNistECp(NIST384P, &tmpCtx->curve);
        ecGenerateRandomNumber(&tmpCtx->curve, &tmpCtx->privKey);
        break;
    }
}

ZrtpDH::~ZrtpDH() {
    if (ctx == NULL)
        return;

    dhCtx* tmpCtx = static_cast<dhCtx*>(ctx);
    FREE_EC_POINT(&tmpCtx->pubPoint);
    bnEnd(&tmpCtx->privKey);

    switch (pkType) {
    case DH2K:
    case DH3K:
        bnEnd(&tmpCtx->pubKey);
        break;

    case EC25:
    case EC38:
        ecFreeCurveNistECp(&tmpCtx->curve);
        break;
    }
}

int32_t ZrtpDH::computeSecretKey(uint8_t *pubKeyBytes, uint8_t *secret) {

    dhCtx* tmpCtx = static_cast<dhCtx*>(ctx);

    int32_t length = getDhSize();

    BigNum sec;
    if (pkType == DH2K || pkType == DH3K) {
        BigNum pubKeyOther;
        bnBegin(&pubKeyOther);
        bnBegin(&sec);

        bnInsertBigBytes(&pubKeyOther, pubKeyBytes, 0, length);

        if (pkType == DH2K) {
            bnExpMod(&sec, &pubKeyOther, &tmpCtx->privKey, &bnP2048);
        }
        else if (pkType == DH3K) {
            bnExpMod(&sec, &pubKeyOther, &tmpCtx->privKey, &bnP3072);
        }
        else {
            return 0;
        }
        bnEnd(&pubKeyOther);
        bnExtractBigBytes(&sec, secret, 0, length);
        bnEnd(&sec);

        return length;
    }

    if (pkType == EC25 || pkType == EC38) {
        int32_t len = getPubKeySize() / 2;
        EcPoint pub;

        bnBegin(&sec);
        INIT_EC_POINT(&pub);
        bnSetQ(pub.z, 1);               // initialze Z to one, these are affine coords

        bnInsertBigBytes(pub.x, pubKeyBytes, 0, len);
        bnInsertBigBytes(pub.y, pubKeyBytes+len, 0, len);

        /* Generate agreement for responder: sec = pub * privKey */
        ecdhComputeAgreement(&tmpCtx->curve, &sec, &pub, &tmpCtx->privKey);
        bnExtractBigBytes(&sec, secret, 0, length);
        bnEnd(&sec);
        FREE_EC_POINT(&pub);

        return length;
    }
    return -1;
}

int32_t ZrtpDH::generatePublicKey()
{
    dhCtx* tmpCtx = static_cast<dhCtx*>(ctx);

    bnBegin(&tmpCtx->pubKey);
    switch (pkType) {
    case DH2K:
        bnExpMod(&tmpCtx->pubKey, &two, &tmpCtx->privKey, &bnP2048);
        break;

    case DH3K:
        bnExpMod(&tmpCtx->pubKey, &two, &tmpCtx->privKey, &bnP3072);
        break;

    case EC25:
    case EC38:
        return ecdhGeneratePublic(&tmpCtx->curve, &tmpCtx->pubPoint, &tmpCtx->privKey);
    }
    return 0;
}

int32_t ZrtpDH::getDhSize() const
{
    switch (pkType) {
    case DH2K:
        return 2048/8;
        break;
    case DH3K:
        return 3072/8;
        break;

    case EC25:
        return 32;
        break;
    case EC38:
        return 48;
        break;
    }
    return 0;
}

int32_t ZrtpDH::getPubKeySize() const
{
    dhCtx* tmpCtx = static_cast<dhCtx*>(ctx);
    if (pkType == DH2K || pkType == DH3K)
        return bnBytes(&tmpCtx->pubKey);

    if (pkType == EC25 || pkType == EC38)
        return bnBytes(tmpCtx->curve.p) * 2;

    return 0;

}

int32_t ZrtpDH::getPubKeyBytes(uint8_t *buf) const
{
    dhCtx* tmpCtx = static_cast<dhCtx*>(ctx);

    if (pkType == DH2K || pkType == DH3K) {
        // get len of pub_key, prepend with zeros to DH size
        int size = getPubKeySize();
        int32_t prepend = getDhSize() - size;
        if (prepend > 0) {
            memset(buf, 0, prepend);
        }
        bnExtractBigBytes(&tmpCtx->pubKey, buf + prepend, 0, size);
        return size;
    }

    if (pkType == EC25 || pkType == EC38) {
        int32_t len = getPubKeySize() / 2;

        bnExtractBigBytes(tmpCtx->pubPoint.x, buf, 0, len);
        bnExtractBigBytes(tmpCtx->pubPoint.y, buf+len, 0, len);
        return len * 2;
    }
    return 0;
}

int32_t ZrtpDH::checkPubKey(uint8_t *pubKeyBytes) const
{

    /* ECC validation (partial), NIST SP800-56A, section 5.6.2.6 */
    if (pkType == EC25 || pkType == EC38) {

        struct BigNum t1, t2;
        dhCtx* tmpCtx = static_cast<dhCtx*>(ctx);
        EcPoint pub;
        int ret = 0;

        INIT_EC_POINT(&pub);
        int32_t len = getPubKeySize() / 2;

        bnBegin(&t1);
        bnBegin(&t2);

        bnInsertBigBytes(pub.x, pubKeyBytes, 0, len);
        bnInsertBigBytes(pub.y, pubKeyBytes+len, 0, len);

        /* Represent point at infinity by (0, 0), make sure it's not that */
        if (bnCmpQ(pub.x, 0) == 0 && bnCmpQ(pub.y, 0) == 0) {
            goto fail;
        }
        /* Check that coordinates are within range */
        if (bnCmpQ(pub.x, 0) < 0 || bnCmp(pub.x, tmpCtx->curve.p) >= 0) {
            goto fail;
        }
        if (bnCmpQ(pub.y, 0) < 0 || bnCmp(pub.y, tmpCtx->curve.p) >= 0) {
            goto fail;
        }
        /* Check that point satisfies EC equation y^2 = x^3 - 3x + b, mod P */
        bnSquareMod_(&t1, pub.y, tmpCtx->curve.p);
        bnSquareMod_(&t2, pub.x, tmpCtx->curve.p);
        bnSubQMod_(&t2, 3, tmpCtx->curve.p);
        bnMulMod_(&t2, &t2, pub.x, tmpCtx->curve.p);
        bnAddMod_(&t2, tmpCtx->curve.b, tmpCtx->curve.p);
        if (bnCmp (&t1, &t2) != 0) {
            goto fail;
        }
        ret = 1;

    fail:
        FREE_EC_POINT(&pub);
        bnEnd(&t1);
        bnEnd(&t2);
        return ret;
    }

    BigNum pubKeyOther;
    bnBegin(&pubKeyOther);
    bnInsertBigBytes(&pubKeyOther, pubKeyBytes, 0, getDhSize());

    if (pkType == DH2K) {
        if (bnCmp(&bnP2048MinusOne, &pubKeyOther) == 0) {
            return 0;
        }
    }
    else if (pkType == DH3K) {
        if (bnCmp(&bnP3072MinusOne, &pubKeyOther) == 0) {
            return 0;

        }
    }
    else {
        return 0;
    }
    if (bnCmpQ(&pubKeyOther, 1) == 0) {
        return 0;
    }

    bnEnd(&pubKeyOther);
    return 1;
}

const char* ZrtpDH::getDHtype()
{
    switch (pkType) {
    case DH2K:
        return dh2k;
        break;
    case DH3K:
        return dh3k;
        break;
    case EC25:
        return ec25;
        break;
    case EC38:
        return ec38;
        break;
    }
    return NULL;
}

/** EMACS **
 * Local variables:
 * mode: c++
 * c-default-style: ellemtel
 * c-basic-offset: 4
 * End:
 */
