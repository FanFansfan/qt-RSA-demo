#ifndef RSA_H
#define RSA_H

#include <QObject>
#include <QMetaType>
#include <gmpxx.h>

Q_DECLARE_METATYPE(mpz_class)

class Rsa : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString p READ p NOTIFY pChanged)
    Q_PROPERTY(QString q READ q NOTIFY qChanged)

public:
    explicit Rsa(int length = 1024, QObject *parent = nullptr);
    ~Rsa();
    QString p() const { return m_p.get_str(16).c_str(); }
    QString q() const { return m_q.get_str(16).c_str(); }

    Q_INVOKABLE void genKey();
    Q_INVOKABLE QString encrypt(const QString &base64Bytes);
    Q_INVOKABLE QString decrypt(const QString &base64Bytes);
    mpz_class encrypt(const mpz_class m);
    mpz_class decrypt(const mpz_class m);

    Q_INVOKABLE QString encode(const QString &pt);
    Q_INVOKABLE QString decode(const QString &ct);

signals:
    void pChanged(mpz_class value);
    void qChanged(mpz_class value);

public slots:

private:
    int length;
    mpz_class m_p;
    mpz_class m_q;
    mpz_class m_phi;
    mpz_class m_e;
    mpz_class m_n;
    mpz_class m_d;
    static gmp_randclass randeng;

public:
    void test();
};

#endif // RSA_H
