#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib,"Crypt32.lib")

#define EXPORT __declspec(dllexport)


BOOL LoadCertificateFile(const wchar_t* path_to_ca_cer,HCERTSTORE* cerstore) {
	if (!cerstore) { return FALSE; }
	*cerstore = CertOpenStore(CERT_STORE_PROV_FILENAME, X509_ASN_ENCODING, NULL, CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG, path_to_ca_cer);
	if (!*cerstore) { return FALSE; }
	return TRUE;
}

EXPORT BOOL IsCACertLoaded(const wchar_t* path_to_ca_cer) {

	HCERTSTORE mycerstore = NULL;
	if (!LoadCertificateFile(path_to_ca_cer, &mycerstore)) { return FALSE; }

	// Get the Certificate Context
	PCCERT_CONTEXT input_pcctx = NULL;
	input_pcctx = CertEnumCertificatesInStore(mycerstore, input_pcctx);
	if (!input_pcctx) { CertCloseStore(mycerstore, 0);  return FALSE; }


	// Open the Machine Store
	HCERTSTORE  hSystemStore;
	hSystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"ROOT");
	if (!hSystemStore) {
		CertCloseStore(mycerstore, 0);
		return FALSE;
	}

	PCCERT_CONTEXT pCert = CertFindCertificateInStore(hSystemStore,X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,0, CERT_FIND_EXISTING, input_pcctx,NULL);

	CertCloseStore(mycerstore, 0);
	CertCloseStore(hSystemStore, 0);
	
	if (!pCert) {return FALSE;}
	return TRUE;

}

// Reads a CER file, gets the first certificate in the store, then installs it in System Level Trusted Root CA
EXPORT BOOL InstallCACert(const wchar_t* path_to_ca_cer) {

	HCERTSTORE mycerstore = NULL;
	if (!LoadCertificateFile(path_to_ca_cer, &mycerstore)) { return FALSE; }

	// Get the Certificate Context
	PCCERT_CONTEXT input_pcctx = NULL;
	input_pcctx = CertEnumCertificatesInStore(mycerstore, input_pcctx);
	if (!input_pcctx) { CertCloseStore(mycerstore, 0);  return FALSE; }

	// Open the Machine Store
	HCERTSTORE  hSystemStore;
	hSystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,0, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"ROOT");
	if (!hSystemStore) { CertCloseStore(mycerstore, 0); return FALSE;}
	
	// Add The Certificate to The Store
	if (!CertAddEncodedCertificateToStore(hSystemStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, input_pcctx->pbCertEncoded, input_pcctx->cbCertEncoded, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) {
		CertCloseStore(mycerstore, 0);
		CertCloseStore(hSystemStore, 0); 
		return FALSE;
	}
	CertCloseStore(mycerstore, 0);
	CertCloseStore(hSystemStore, 0);

	return TRUE;
}

EXPORT BOOL RemoveCACert(const wchar_t* path_to_ca_cer) {

	HCERTSTORE mycerstore = NULL;
	if (!LoadCertificateFile(path_to_ca_cer, &mycerstore)) { return FALSE; }

	// Get the Certificate Context
	PCCERT_CONTEXT input_pcctx = NULL;
	input_pcctx = CertEnumCertificatesInStore(mycerstore, input_pcctx);
	if (!input_pcctx) { CertCloseStore(mycerstore, 0);  return FALSE; }


	// Open the Machine Store
	HCERTSTORE  hSystemStore;
	hSystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"ROOT");
	if (!hSystemStore) {
		CertCloseStore(mycerstore, 0);
		return FALSE;
	}

	PCCERT_CONTEXT pCert = CertFindCertificateInStore(hSystemStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_EXISTING, input_pcctx, NULL);

	if (!pCert) {
		CertCloseStore(mycerstore, 0);
		CertCloseStore(hSystemStore, 0);
		return FALSE;
	}

	BOOL res = CertDeleteCertificateFromStore(pCert);
	CertCloseStore(mycerstore, 0);
	CertCloseStore(hSystemStore, 0);

	return res;

}

#ifdef TEST_BIN
int main() {
	BOOL result = FALSE;
    printf("Testing...\n");
    if(!InstallCACert(L"mitmproxy-ca-cert.cer")){
       printf("Install CACert Failed!\n"); 
       return -1;
    }
	if(!IsCACertLoaded(L"mitmproxy-ca-cert.cer")){
        printf("IsCACertLoaded Failed!\n"); 
        return -1;
    }
        
	if(!RemoveCACert(L"mitmproxy-ca-cert.cer")){
        printf("RemoveCACert Failed!\n"); 
        return -1;        
    }
	printf("Result OK!\n");
	return 0;
}
#endif