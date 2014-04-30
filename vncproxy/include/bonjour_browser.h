#ifndef BONJOUR_BROWSER_H
#define BONJOUR_BROWSER_H

#include <dns_sd.h>
#include <stdio.h>
#include <string.h>
#define SERVICE_OVERLAY 0x01
#define SERVICE_STANDARD 0x00

extern void connectService(const char *fullname, const char *ip, int port, uint16_t mode);
extern void disconnectService(const char *serviceName);
extern void HandleServiceDiscovered(const char *name);
extern void HandleServiceDisconnected(const char *name);

void DNSSD_API MyBrowseCallBack(DNSServiceRef service,
					  DNSServiceFlags flags,
					  uint32_t interfaceIndex,
					  DNSServiceErrorType errorCode,
					  const char *name,
					  const char *type,
					  const char *domain,
					  void *context);

DNSServiceRef MyDNSServiceBrowse();

void DNSSD_API MyResolveCallBack(DNSServiceRef serviceRef,
					   DNSServiceFlags flags,
					   uint32_t interfaceIndex,
					   DNSServiceErrorType errorCode,
					   const char *fullname,
					   const char *hosttarget,
					   uint16_t port,
					   uint16_t txtLen,
					   const unsigned char *txtRecord,
					   void *context);

DNSServiceErrorType MyDNSServiceResolve(const char *serviceName, uint32_t interfaceIndex);

void DNSSD_API RegisterCallBack(DNSServiceRef service,
							   DNSServiceFlags flags,
							   DNSServiceErrorType errorCode,
							   const char *name,
							   const char *type,
							   const char *domain,
							   void *context);

DNSServiceErrorType DNSServiceRegister(int port, int screenWidth, int screenHeight);

void DNSServiceDeregister();

#endif
