// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>
#include <utilstrencodings.h>
#include <crypto/common.h>
#include "util.h"

uint256 CBlockHeader::GetHash() const
{
    return Phi1612(BEGIN(nVersion), END(nNonce));
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}

// folmcoin: sign block
bool CBlock::SignBlock(const CKeyStore& keystore)
{
    std::vector<valtype> vSolutions;
    txnouttype whichType;

    if (IsProofOfStake()) {
        // if we are trying to sign
        //    a complete proof-of-stake block
        return gtx[0].vout[0].IsEmpty() && gtx.size() > 1 && gtx[1].IsCoinStake();
    } else {
        for (unsigned int i = 0; i < gtx[0].vout.size(); i++) {
            const CTxOut& txout = gtx[0].vout[i];

            if (!Solver(txout.scriptPubKey, whichType, vSolutions))
                continue;

            if (whichType == TX_PUBKEY) {
                // Sign
                CKeyID keyID;
                keyID = CKeyID(uint160(vSolutions[0]));

                CKey key;
                if (!keystore.GetKey(keyID, key))
                    return false;

                if (!key.Sign(GetHash(), vchBlockSig))
                    return false;

                return true;
            }
        }
    }

    LogPrintf("Sign failed\n");
    return false;
}

bool CBlock::CheckBlockSignature() const
{
    if (IsProofOfWork())
        return vchBlockSig.empty();

    if (IsProofOfStake()) {
        // if we are trying to sign
        //    a complete proof-of-stake block
        return gtx[0].vout[0].IsEmpty();
    }

    std::vector<valtype> vSolutions;
    txnouttype whichType;

    const CTxOut& txout = gtx[1].vout[1];

    if (!Solver(txout.scriptPubKey, whichType, vSolutions))
        return false;

    if (whichType == TX_PUBKEY)
    {
        valtype& vchPubKey = vSolutions[0];
        CPubKey pubkey(vchPubKey);
        if (!pubkey.IsValid())
            return false;

        if (vchBlockSig.empty())
            return false;

        return pubkey.Verify(GetHash(), vchBlockSig);
    }
    else if(whichType == TX_PUBKEYHASH)
    {
        valtype& vchPubKey = vSolutions[0];
        CKeyID keyID;
        keyID = CKeyID(uint160(vchPubKey));
        CPubKey pubkey(vchPubKey);

        if (!pubkey.IsValid())
            return false;

        if (vchBlockSig.empty())
            return false;

        return pubkey.Verify(GetHash(), vchBlockSig);

    }

    return false;
}