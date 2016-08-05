#include <picotorrent/common/security/certificate_manager.hpp>

#include <iomanip>
#include <sstream>

#include <openssl/pem.h>
#include <openssl/x509.h>

using picotorrent::common::security::certificate_manager;

RSA* generate_rsa()
{
    BIGNUM *bn = BN_new();
    BN_set_word(bn, RSA_F4);

    RSA *rsa = RSA_new();
    RSA_generate_key_ex(rsa, 4096, bn, NULL);

    BN_free(bn);

    return rsa;
}

X509* generate_x509(EVP_PKEY *pkey)
{
    X509 *x509 = X509_new();

    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

    // Valid for three years from now
    X509_gmtime_adj(X509_get_notBefore(x509), 0L);
    X509_gmtime_adj(X509_get_notAfter(x509), 94670778L);

    // Set the public key for our certificate
    X509_set_pubkey(x509, pkey);

    X509_NAME *name = X509_get_subject_name(x509);

    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)"CA", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)"PicoTorrent", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)"PicoTorrent", -1, -1, 0);

    X509_set_issuer_name(x509, name);

    X509_sign(x509, pkey, EVP_sha1());

    return x509;
}

std::string certificate_manager::extract_public_key(const std::string &certificate_file)
{
    BIO *certbio = BIO_new(BIO_s_file());
    BIO *outbio = BIO_new(BIO_s_mem());

    BIO_read_filename(certbio, certificate_file.c_str());

    X509 *cert = PEM_read_bio_X509(certbio, NULL, 0, NULL);
    ASN1_BIT_STRING *pubKey = X509_get0_pubkey_bitstr(cert);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (int i = 0; i < pubKey->length; i++)
    {
        ss << std::setw(2) << std::hex << static_cast<unsigned>(pubKey->data[i]);
    }

    X509_free(cert);
    BIO_free_all(certbio);
    BIO_free_all(outbio);

    return ss.str();
}

std::vector<char> certificate_manager::generate()
{
    RSA *rsa = generate_rsa();
    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_set1_RSA(pkey, rsa);

    X509 *x509 = generate_x509(pkey);

    // Serialize the certificate
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_X509(bio, x509);
    BUF_MEM *mem = NULL;
    BIO_get_mem_ptr(bio, &mem);

    // Serialize the private key
    BIO *kBio = BIO_new(BIO_s_mem());
    int rc = PEM_write_bio_RSAPrivateKey(kBio, rsa, NULL, NULL, 0, 0, NULL);
    BUF_MEM *kMem = NULL;
    BIO_get_mem_ptr(kBio, &kMem);

    std::vector<char> res;
    res.insert(res.end(), kMem->data, kMem->data + kMem->length);
    res.insert(res.end(), mem->data, mem->data + mem->length);

    EVP_PKEY_free(pkey);
    X509_free(x509);
    BIO_free(bio);
    BIO_free(kBio);

    return res;
}
