#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

#define EXPORT __declspec(dllexport)

static BOOL LoadCertificateFile(const wchar_t* path_to_ca_cer, HCERTSTORE* cerstore) {
	if (!cerstore || !path_to_ca_cer) {
		return FALSE;
	}
	
	*cerstore = CertOpenStore(
		CERT_STORE_PROV_FILENAME, 
		X509_ASN_ENCODING, 
		(HCRYPTPROV_LEGACY)NULL, 
		CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG, 
		path_to_ca_cer
	);
	
	return (*cerstore != NULL);
}

static HCERTSTORE OpenSystemRootStore() {
	return CertOpenStore(
		CERT_STORE_PROV_SYSTEM, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		(HCRYPTPROV_LEGACY)NULL, 
		CERT_SYSTEM_STORE_LOCAL_MACHINE, 
		L"ROOT"
	);
}

EXPORT BOOL IsCACertLoaded(const wchar_t* path_to_ca_cer) {
	BOOL result = FALSE;
	HCERTSTORE mycerstore = NULL;
	HCERTSTORE hSystemStore = NULL;
	PCCERT_CONTEXT input_pcctx = NULL;
	PCCERT_CONTEXT pCert = NULL;
	
	if (!LoadCertificateFile(path_to_ca_cer, &mycerstore)) {
		goto cleanup;
	}

	input_pcctx = CertEnumCertificatesInStore(mycerstore, NULL);
	if (!input_pcctx) {
		goto cleanup;
	}

	hSystemStore = OpenSystemRootStore();
	if (!hSystemStore) {
		goto cleanup;
	}

	pCert = CertFindCertificateInStore(
		hSystemStore,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		0, 
		CERT_FIND_EXISTING, 
		input_pcctx,
		NULL
	);
	
	result = (pCert != NULL);
	
cleanup:
	if (pCert) {
		CertFreeCertificateContext(pCert);
	}
	if (input_pcctx) {
		CertFreeCertificateContext(input_pcctx);
	}
	if (mycerstore) {
		CertCloseStore(mycerstore, 0);
	}
	if (hSystemStore) {
		CertCloseStore(hSystemStore, 0);
	}
	
	return result;
}

EXPORT BOOL InstallCACert(const wchar_t* path_to_ca_cer) {
	BOOL result = FALSE;
	HCERTSTORE mycerstore = NULL;
	HCERTSTORE hSystemStore = NULL;
	PCCERT_CONTEXT input_pcctx = NULL;
	
	if (!LoadCertificateFile(path_to_ca_cer, &mycerstore)) {
		goto cleanup;
	}

	input_pcctx = CertEnumCertificatesInStore(mycerstore, NULL);
	if (!input_pcctx) {
		goto cleanup;
	}

	hSystemStore = OpenSystemRootStore();
	if (!hSystemStore) {
		goto cleanup;
	}
	
	result = CertAddEncodedCertificateToStore(
		hSystemStore, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 
		input_pcctx->pbCertEncoded, 
		input_pcctx->cbCertEncoded, 
		CERT_STORE_ADD_REPLACE_EXISTING, 
		NULL
	);
	
cleanup:
	if (input_pcctx) {
		CertFreeCertificateContext(input_pcctx);
	}
	if (mycerstore) {
		CertCloseStore(mycerstore, 0);
	}
	if (hSystemStore) {
		CertCloseStore(hSystemStore, 0);
	}
	
	return result;
}

EXPORT BOOL RemoveCACert(const wchar_t* path_to_ca_cer) {
	BOOL result = FALSE;
	HCERTSTORE mycerstore = NULL;
	HCERTSTORE hSystemStore = NULL;
	PCCERT_CONTEXT input_pcctx = NULL;
	PCCERT_CONTEXT pCert = NULL;
	
	if (!LoadCertificateFile(path_to_ca_cer, &mycerstore)) {
		goto cleanup;
	}

	input_pcctx = CertEnumCertificatesInStore(mycerstore, NULL);
	if (!input_pcctx) {
		goto cleanup;
	}

	hSystemStore = OpenSystemRootStore();
	if (!hSystemStore) {
		goto cleanup;
	}

	pCert = CertFindCertificateInStore(
		hSystemStore, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 
		0, 
		CERT_FIND_EXISTING, 
		input_pcctx, 
		NULL
	);

	if (!pCert) {
		goto cleanup;
	}

	result = CertDeleteCertificateFromStore(pCert);
	// Note: Don't free pCert after CertDeleteCertificateFromStore as it's freed by the function
	pCert = NULL;
	
cleanup:
	if (pCert) {
		CertFreeCertificateContext(pCert);
	}
	if (input_pcctx) {
		CertFreeCertificateContext(input_pcctx);
	}
	if (mycerstore) {
		CertCloseStore(mycerstore, 0);
	}
	if (hSystemStore) {
		CertCloseStore(hSystemStore, 0);
	}
	
	return result;
}

#ifdef TEST_BIN
int main() {
	printf("Testing certificate operations...\n");
	
	if (!InstallCACert(L"mitmproxy-ca-cert.cer")) {
		printf("ERROR: Install CACert Failed!\n"); 
		return -1;
	}
	printf("Certificate installed successfully.\n");
	
	if (!IsCACertLoaded(L"mitmproxy-ca-cert.cer")) {
		printf("ERROR: IsCACertLoaded Failed!\n"); 
		return -1;
	}
	printf("Certificate verified in store.\n");
		
	if (!RemoveCACert(L"mitmproxy-ca-cert.cer")) {
		printf("ERROR: RemoveCACert Failed!\n"); 
		return -1;        
	}
	printf("Certificate removed successfully.\n");
	
	printf("All tests passed!\n");
	return 0;
}
#endif