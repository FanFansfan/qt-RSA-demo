#include "Rsa.h"
#include<iostream>
#include <QDebug>
#include <QByteArray>

Rsa::Rsa(int length, QObject *parent)
    :QObject(parent)
{
    this->length = length;
//    qDebug() << "Rsa::Rsa() :";
}

Rsa::~Rsa()
{

}

gmp_randclass Rsa::randeng{gmp_randinit_mt};

void Rsa::genKey()
{
    // 生成一半密钥长度的随机数，然后使用gmp 的nextprime生成质数
    const int prime_length = length / 2;
    char* p_str = new char[prime_length + 1];
    char* q_str = new char[prime_length + 1];

    p_str[0] = '1';
    q_str[0] = '1';

    for(int i = 1; i < prime_length; i++)
        p_str[i] = (int)(2.0*rand()/(RAND_MAX+1.0)) + 48;

    for(int i = 1; i < prime_length; i++)
        q_str[i] = (int)(2.0*rand()/(RAND_MAX+1.0)) + 48;

    p_str[prime_length] = '\0';
    q_str[prime_length] = '\0';

    mpz_set_str(m_p.get_mpz_t(), p_str, 2);
    mpz_set_str(m_q.get_mpz_t(), q_str, 2);

    qDebug() << "genKey, p: " << m_p.get_str(16).c_str();
    qDebug() << "genKey, q: " << m_q.get_str(16).c_str();

    mpz_nextprime(m_p.get_mpz_t(), m_p.get_mpz_t());
    mpz_nextprime(m_q.get_mpz_t(), m_q.get_mpz_t());

    m_n = m_p * m_q;
    m_phi = (m_p - 1) * (m_q - 1);
    //assert(mpz_sizeinbase(m_n.get_mpz_t(), 2) >= length);
    qDebug() << "genKey, n: " << m_n.get_str(2).c_str();

    // 确保e与欧拉函数互质
    m_e = "65537";
    mpz_class tmp_gcd;
    while(true)
    {
        mpz_gcd(tmp_gcd.get_mpz_t(), m_phi.get_mpz_t(), m_e.get_mpz_t());

        if(mpz_cmp_ui(tmp_gcd.get_mpz_t(), (unsigned int)1) == 0)
            break;

        m_e += 2;
    }
    qDebug() << "genKey, e: " << m_e.get_si();

    assert(mpz_invert(m_d.get_mpz_t(), m_e.get_mpz_t(), m_phi.get_mpz_t()) != 0);

    qDebug() << "genKey, d: " << m_d.get_str(10).c_str();

    emit pChanged(m_p);
    emit qChanged(m_q);
    delete [] p_str;
    delete [] q_str;
}

// base64编码的字节流按块加密，要求字节流已经经过padding。每一块import到mpz大整数类。加密后的大整数补前导0到密钥长度。
QString Rsa::encrypt(const QString &base64Bytes)
{
    qDebug() << "Rsa::encrypt(const QString &): " << base64Bytes;
    QByteArray src = QByteArray::fromBase64(base64Bytes.toUtf8());
    qDebug() << "Rsa::encrypt(const QString &): " << src.length();

    int blk_siz = length / 8;
    int remain_bytes = src.length();
    assert(remain_bytes % blk_siz == 0);
    int blk_num = remain_bytes / blk_siz;

    mpz_class m, c;
    QByteArray res;

    for(int i = 0; i < blk_num; ++i)
    {
        QByteArray blk(blk_siz, 0x00);
        mpz_import(m.get_mpz_t(), blk_siz, 1, sizeof(char), 0, 0, src.data() + i * blk_siz);
        assert(mpz_sizeinbase(m.get_mpz_t(), 2) <= length);
        c = encrypt(m);
        assert(decrypt(c) == m);
        int bits_count = mpz_sizeinbase(c.get_mpz_t(), 2);
        qDebug() << "Rsa::encrypt(const QString &): bit count: " << bits_count;
        int offset = blk_siz - (bits_count + 8 - 1) / 8;
        qDebug() << "Rsa::encrypt(const QString &): offset: " << offset;

        size_t cnt;
        mpz_export(blk.data() + offset, &cnt, 1, sizeof(char), 0, 0, c.get_mpz_t());
        qDebug() << cnt;
        res.append(blk);
    }
    qDebug() << "Rsa::encrypt(const QString &): res: " << res;
    return res.toBase64();
}


mpz_class Rsa::encrypt(const mpz_class m)
{
    mpz_class ret;
    mpz_powm(ret.get_mpz_t(), m.get_mpz_t(), m_e.get_mpz_t(), m_n.get_mpz_t());
    return ret;
}

// 把base64编码的密文分块解密
QString Rsa::decrypt(const QString &base64Bytes)
{
    QByteArray src = QByteArray::fromBase64(base64Bytes.toUtf8());

    qDebug() << "Rsa::decrypt() : " << src << " length " << src.length();

    int blk_siz = length / 8;
    int remain_bytes = src.length();
    assert(remain_bytes % blk_siz == 0);
    mpz_class c, m;
    QByteArray res;

    while(remain_bytes > 0)
    {
        QByteArray blk(blk_siz, 0x00);
        mpz_import(c.get_mpz_t(), blk_siz, 1, sizeof(char), 0, 0, src.data() + src.length() - remain_bytes);
        m = decrypt(c);
        int offset = (blk_siz - (mpz_sizeinbase(m.get_mpz_t(), 2) + 8 - 1) / 8);
        qDebug() << "Rsa::decrypt(): " << offset;
        mpz_export((blk.data()) + offset, NULL, 1, sizeof(char), 0, 0, m.get_mpz_t());
        res.append(blk);
        remain_bytes -= blk_siz;
    }
    return res.toBase64();
}

mpz_class Rsa::decrypt(const mpz_class m)
{
    mpz_class ret;
    mpz_powm(ret.get_mpz_t(), m.get_mpz_t(), m_d.get_mpz_t(), m_n.get_mpz_t());
    return ret;
}

// PKCS1 V1_5 padding，然后base64编码
QString Rsa::encode(const QString &pt)
{
    //       0     1           blk_size - D.len - 1
    // EMB = 00 || 02 || PS || 00 || D
    QByteArray src = pt.toUtf8();
    qDebug() << "Rsa::encode(), length: " << src.length();
    int blk_siz = this->length / 8;
    int remain_bytes = src.length();

    QByteArray res;
    QByteArray blk(blk_siz, 0x00);
    blk[0] = 0x00;
    blk[1] = 0x02;

    while(remain_bytes > 0)
    {
        int D_len = (remain_bytes >= (blk_siz - 11)) ? blk_siz - 11 : remain_bytes;

        for(int i = 0; i < blk_siz - D_len - 3; ++i)
            blk[i+2] = (rand() % (0xFF - 1)) + 1;
        blk[blk_siz - D_len - 1] = 0x00;
        for(int i = 0; i < D_len; ++i)
            blk[blk_siz - D_len + i] = src[src.length() - remain_bytes + i];
        remain_bytes -= D_len;
        res.append(blk);
    }
    assert(res.length() % blk_siz == 0);
    qDebug() << "Rsa::endoce() : " << res.length();
    qDebug() << "Rsa::encode() : " << res;
    return res.toBase64();
}

// 去掉padding拼接块
QString Rsa::decode(const QString &ct)
{
    QByteArray src = QByteArray::fromBase64(ct.toUtf8());
    qDebug() << "Rsa::decode(): " << src << "\nlenth: " << src.length();
    QByteArray res;
    int blk_siz = length / 8;
    assert(src.length() % blk_siz == 0);
    int blk_num = src.length() / blk_siz;
    for(int i = 0; i < blk_num; ++i)
    {
        int j = 2;
        while(src[i * blk_siz + j] != 0x00 && j < blk_siz)
            j++;
        j++;
        for(; j < blk_siz; ++j)
            res.append(src[i * blk_siz + j]);
    }
    qDebug() << "Rsa::decode()" << res.length();
    qDebug() << "Rsa::decode()" << res;
    return res;
}

void Rsa::test()
{

}
