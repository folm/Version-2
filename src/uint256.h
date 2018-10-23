// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef FOLM_UINT256_H
#define FOLM_UINT256_H

#include <assert.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>
#include <crypto/common.h>

/** Template base class for fixed-sized opaque blobs. */
template<unsigned int BITS>
class base_blob
{
protected:
    static constexpr int WIDTH = BITS / 8;
    uint8_t data[WIDTH];
public:
    base_blob()
    {
        memset(data, 0, sizeof(data));
    }

    explicit base_blob(const std::vector<unsigned char>& vch);

    explicit base_blob(const std::string& str);

    bool IsNull() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (data[i] != 0)
                return false;
        return true;
    }

    void SetNull()
    {
        memset(data, 0, sizeof(data));
    }

    inline int Compare(const base_blob& other) const { return memcmp(data, other.data, sizeof(data)); }

    int CompareTo(const base_blob& b) const;
    bool EqualTo(uint64_t b) const;

    friend inline bool operator==(const base_blob& a, const base_blob& b) { return a.Compare(b) == 0; }
    friend inline bool operator!=(const base_blob& a, const base_blob& b) { return a.Compare(b) != 0; }
    friend inline bool operator<(const base_blob& a, const base_blob& b) { return a.Compare(b) < 0; }
    friend inline bool operator==(const base_blob& a, uint64_t b) { return a.EqualTo(b); }

    std::string GetHex() const;
    void SetHex(const char* psz);
    void SetHex(const std::string& str);
    std::string ToString() const;

    unsigned char* begin()
    {
        return &data[0];
    }

    unsigned char* end()
    {
        return &data[WIDTH];
    }

    const unsigned char* begin() const
    {
        return &data[0];
    }

    const unsigned char* end() const
    {
        return &data[WIDTH];
    }

    unsigned int size() const
    {
        return sizeof(data);
    }

    const base_blob operator~() const
        {
            base_blob ret;
            for (int i = 0; i < WIDTH; i++)
                ret.data[i] = ~data[i];
            return ret;
        }


    uint64_t Get64(int n = 0) const
    {
        return data[2 * n] | (uint64_t)data[2 * n + 1] << 32;
    }

    uint64_t GetUint64(int pos) const
    {
        const uint8_t* ptr = data + pos * 8;
        return ((uint64_t)ptr[0]) | \
               ((uint64_t)ptr[1]) << 8 | \
               ((uint64_t)ptr[2]) << 16 | \
               ((uint64_t)ptr[3]) << 24 | \
               ((uint64_t)ptr[4]) << 32 | \
               ((uint64_t)ptr[5]) << 40 | \
               ((uint64_t)ptr[6]) << 48 | \
               ((uint64_t)ptr[7]) << 56;
    }

    base_blob& operator=(const base_blob& b)
        {
            for (int i = 0; i < WIDTH; i++)
                data[i] = b.data[i];
            return *this;
        }


        base_blob(uint64_t b)
        {
                data[0] = (unsigned int)b;
                data[1] = (unsigned int)(b >> 32);
                for (int i = 2; i < WIDTH; i++)
                    data[i] = 0;
            }

    template<typename Stream>
    void Serialize(Stream& s) const
    {
        s.write((char*)data, sizeof(data));
    }

    template<typename Stream>
    void Unserialize(Stream& s)
    {
        s.read((char*)data, sizeof(data));
    }
};

/** 160-bit opaque blob.
 * @note This type is called uint160 for historical reasons only. It is an opaque
 * blob of 160 bits and has no integer operations.
 */
class uint160 : public base_blob<160> {
public:
    uint160() {}
    explicit uint160(const std::vector<unsigned char>& vch) : base_blob<160>(vch) {}
};

/** 256-bit opaque blob.
 * @note This type is called uint256 for historical reasons only. It is an
 * opaque blob of 256 bits and has no integer operations. Use arith_uint256 if
 * those are required.
 */
/** 256-bit unsigned big integer. */
class uint256 : public base_uint<256>
{
public:
    uint256() {}
    uint256(const base_uint<256>& b) : base_uint<256>(b) {}
    uint256(uint64_t b) : base_uint<256>(b) {}
    explicit uint256(const std::string& str) : base_uint<256>(str) {}
    explicit uint256(const std::vector<unsigned char>& vch) : base_uint<256>(vch) {}

    /**
     * The "compact" format is a representation of a whole
     * number N using an unsigned 32bit number similar to a
     * floating point format.
     * The most significant 8 bits are the unsigned exponent of base 256.
     * This exponent can be thought of as "number of bytes of N".
     * The lower 23 bits are the mantissa.
     * Bit number 24 (0x800000) represents the sign of N.
     * N = (-1^sign) * mantissa * 256^(exponent-3)
     *
     * Satoshi's original implementation used BN_bn2mpi() and BN_mpi2bn().
     * MPI uses the most significant bit of the first byte as sign.
     * Thus 0x1234560000 is compact (0x05123456)
     * and  0xc0de000000 is compact (0x0600c0de)
     *
     * Bitcoin only uses this "compact" format for encoding difficulty
     * targets, which are unsigned 256bit quantities.  Thus, all the
     * complexities of the sign bit and using base 256 are probably an
     * implementation accident.
     */
    uint256& SetCompact(uint32_t nCompact, bool* pfNegative = NULL, bool* pfOverflow = NULL);
    uint32_t GetCompact(bool fNegative = false) const;
    uint64_t GetHash(const uint256& salt) const;


/* uint256 from const char *.
 * This is a separate function because the constructor uint256(const char*) can result
 * in dangerously catching uint256(0).
 */
inline uint256 uint256S(const char *str)
{
    uint256 rv;
    rv.SetHex(str);
    return rv;
}
/* uint256 from std::string.
 * This is a separate function because the constructor uint256(const std::string &str) can result
 * in dangerously catching uint256(0) via std::string(const char*).
 */
inline uint256 uint256S(const std::string& str)
{
    uint256 rv;
    rv.SetHex(str);
    return rv;
}

/** 512-bit unsigned big integer. */
class uint512 : public base_uint<512>
{
public:
    uint512() {}
    uint512(const base_uint<512>& b) : base_uint<512>(b) {}
    uint512(uint64_t b) : base_uint<512>(b) {}
    explicit uint512(const std::string& str) : base_uint<512>(str) {}
    explicit uint512(const std::vector<unsigned char>& vch) : base_uint<512>(vch) {}

    uint256 trim256() const
    {
        uint256 ret;
        for (unsigned int i = 0; i < uint256::WIDTH; i++) {
            ret.pn[i] = pn[i];
        }
        return ret;
    }
};


inline uint512 uint512S(const std::string& str)
{
    uint512 rv;
    rv.SetHex(str);
    return rv;
}


#endif // FOLM_UINT256_H
