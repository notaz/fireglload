/*
 * fireglload
 * (C) notaz, 2014
 *
 * This work is licensed under the terms of 3-clause BSD license.
 * See COPYING file in the top-level directory.
 */

#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include <pciaccess.h>

#include <xorg/xf86.h>
#include <xorg/xf86Module.h>
#include <xorg/xf86str.h>

#if 0
#define TRACE(f, ...) \
	fprintf(stderr, ". %s " f, __FUNCTION__, ##__VA_ARGS__)
#define NI() \
	fprintf(stderr, "NI: %s\n", __FUNCTION__)
#else
#define TRACE(...)
#define NI()
#endif

#ifndef _X_EXPORT
#define _X_EXPORT      __attribute__((visibility("default")))
#endif

static GDevRec drec;

#ifdef DIX_H
_X_EXPORT ClientPtr clients[MAXCLIENTS];
_X_EXPORT ScreenInfo screenInfo;
_X_EXPORT ClientPtr serverClient;
#else
#define MAXCLIENTS      256
_X_EXPORT void *clients[MAXCLIENTS];
_X_EXPORT long screenInfo[16]; // ScreenInfo
_X_EXPORT void *serverClient;
#endif
#ifdef _XF86_H
_X_EXPORT ScrnInfoPtr *xf86Screens;
_X_EXPORT confDRIRec xf86ConfigDRI;
#else
_X_EXPORT void *xf86Screens;
_X_EXPORT long xf86ConfigDRI[4]; // confDRIRec
#endif
_X_EXPORT int PanoramiXNumScreens;
_X_EXPORT long inputInfo[16]; // InputInfo
_X_EXPORT unsigned long globalSerialNumber = 0;
_X_EXPORT unsigned long serverGeneration = 0;
_X_EXPORT int noPanoramiXExtension;
_X_EXPORT int noRRExtension;
_X_EXPORT int noCompositeExtension;
_X_EXPORT int noRenderExtension;
_X_EXPORT char dispatchException;
_X_EXPORT void *xf86DriverList;
_X_EXPORT int xf86NumDrivers;
_X_EXPORT void *xf86configptr;
_X_EXPORT int xf86DefaultModes[1024]; // DisplayModeRec[]
_X_EXPORT int xf86Info[64]; // xf86InfoRec
_X_EXPORT int xf86Initialising;
_X_EXPORT int XRT_WINDOW;
_X_EXPORT int monitorResolution;
_X_EXPORT int DPMSEnabled;
#define SCREEN_SAVER_OFF  1
_X_EXPORT int screenIsSaved = SCREEN_SAVER_OFF;

#ifndef _XF86_H
_X_EXPORT void xf86DrvMsg(int scrnIndex, MessageType type, const char *format, ...);
_X_EXPORT Bool xf86LoaderCheckSymbol(const char *name);
_X_EXPORT void xf86AddDriver(DriverPtr driver, pointer module, int flags);
#endif
_X_EXPORT void GetSpritePosition(struct _DeviceIntRec*, int *, int *);
_X_EXPORT int xf86LoadKernelModule(const char *pathname);

// don't delete my globals..
extern void ddmg(void *g);

static void __attribute__((constructor)) ddmg_hack(void)
{
	ddmg(&clients);
	ddmg(&PanoramiXNumScreens);
	ddmg(&inputInfo);
	ddmg(&screenInfo);
	ddmg(&globalSerialNumber);
	ddmg(&serverGeneration);
	ddmg(&noPanoramiXExtension);
	ddmg(&noRRExtension);
	ddmg(&noCompositeExtension);
	ddmg(&noRenderExtension);
	ddmg(&dispatchException);
	ddmg(&xf86Screens);
	ddmg(&xf86DriverList);
	ddmg(&xf86NumDrivers);
	ddmg(&xf86configptr);
	ddmg(&xf86DefaultModes);
	ddmg(&xf86Info);
	ddmg(&xf86ConfigDRI);
	ddmg(&xf86Initialising);
	ddmg(&serverClient);
	ddmg(&XRT_WINDOW);
	ddmg(&monitorResolution);
	ddmg(&DPMSEnabled);
	ddmg(&screenIsSaved);

	ddmg(&LoadSubModule);
	ddmg(&LoaderSymbol);
	ddmg(&xf86LoaderCheckSymbol);
	ddmg(&xf86DrvMsg);
	ddmg(&xf86VDrvMsgVerb);
	ddmg(&xf86Msg);
	ddmg(&xf86MsgVerb);
	ddmg(&xf86AddDriver);
	ddmg(&GetSpritePosition);
	ddmg(&XNFalloc);
	ddmg(&XNFcalloc);
	ddmg(&XNFrealloc);
	ddmg(&xf86MatchDevice);
	ddmg(&xf86LoadKernelModule);
	ddmg(&xf86MatchPciInstances);
	ddmg(&xf86CheckPciSlot);
	ddmg(&xf86ConfigPciEntity);
	ddmg(&xf86ComparePciBusString);
	ddmg(&xf86ClaimPciSlot);
	ddmg(&xf86GetPciInfoForEntity);
	ddmg(&xf86GetEntityInfo);
	ddmg(&xf86AllocateEntityPrivateIndex);
	ddmg(&xf86GetEntityPrivate);
	ddmg(&xf86SetEntitySharable);
	ddmg(&xf86GetNumEntityInstances);
	ddmg(&xf86GetDevFromEntity);
	ddmg(&xf86AddBusDeviceToConfigure);
	ddmg(&GetTimeInMillis);
	ddmg(&xf86GetOptValBool);
}

#define MAKE_FUNC(name) \
int _X_EXPORT name() \
{ \
	NI(); \
	return 0; \
} \
static void __attribute__((constructor)) c_##name(void) \
{ \
	ddmg(&name); \
}

MAKE_FUNC(xf86SetDGAMode)
MAKE_FUNC(GetSpriteCursor)
MAKE_FUNC(StandardMinorOpcode)

void
GetSpritePosition(struct _DeviceIntRec *pDev, int *px, int *py)
{
	NI();
}

pointer
LoadSubModule(pointer _parent, const char *module,
              const char **subdirlist, const char **patternlist,
              pointer options, const XF86ModReqInfo * modreq,
              int *errmaj, int *errmin)
{
	void *ret;

	printf("LoadSubModule '%s'\n", module);

	if (module && strstr(module, "fglrxdrm"))
		module = "/usr/lib/xorg/modules/linux/libfglrxdrm.so";
	ret = dlopen(module, RTLD_LAZY | RTLD_GLOBAL);
	if (ret == NULL)
		fprintf(stderr, "LoadSubModule dlopen: %s\n", dlerror());

	return ret;
}

void *
LoaderSymbol(const char *name)
{
	void *ret = dlsym(NULL, name);
	TRACE("'%s' %p\n", name, ret);
	return ret;
}

Bool
xf86LoaderCheckSymbol(const char *name)
{
	Bool ret;

	ret = (dlsym(NULL, name) != NULL);
	TRACE("'%s' %d\n", name, ret);
	return ret;
}

int
xf86MatchDevice(const char *drivername, GDevPtr **sectlist)
{
	GDevRec **list;

	TRACE("'%s' %p\n", drivername, sectlist);

	if (sectlist == NULL)
		goto out;

	*sectlist = NULL;
	list = malloc(2 * sizeof(list[0]));
	if (list == NULL)
		return 0;
	list[0] = &drec;
	list[1] = NULL;
	*sectlist = list;

out:
	return 1;
}

int
xf86MatchPciInstances(const char *driverName, int vendorID,
                      SymTabPtr chipsets, PciChipsets *PCIchipsets,
                      GDevPtr *devList, int numDevs, DriverPtr drvp,
                      int **foundEntities)
{
	TRACE("'%s' %04x %p %p %p %d %p %p\n",
		driverName, vendorID, chipsets, PCIchipsets, devList, numDevs,
		drvp, foundEntities);

	*foundEntities = malloc(sizeof(int));
	if (*foundEntities == NULL)
		return 0;

	(*foundEntities)[0] = 0;
	return 1;
}

GDevPtr
xf86AddBusDeviceToConfigure(const char *driver, BusType bus, void *busData, int chipset)
{
	struct pci_device *pci = busData;

	TRACE("'%s' %d %p %d\n", driver, bus, busData, chipset);

	drec.driver = strdup(driver);
	drec.identifier = strdup("fake_dev");
	drec.busID = malloc(64);
	snprintf(drec.busID, 64, "PCI:%d:%d:%d", pci->bus, pci->dev, pci->func);
	drec.chipID = pci->device_id;
	drec.chipRev = pci->revision;
	drec.irq = -1;

	drec.myScreenSection = calloc(1, sizeof(*drec.myScreenSection));

	TRACE("ret busID '%s'\n", drec.busID);
	return &drec;
}

Bool
xf86CheckPciSlot(const struct pci_device *d)
{
	TRACE("%p\n", d);
	return TRUE;
}

ScrnInfoPtr
xf86ConfigPciEntity(ScrnInfoPtr pScrn, int scrnFlag, int entityIndex,
                          PciChipsets *p_chip, void *dummy, EntityProc init,
                          EntityProc enter, EntityProc leave, pointer private)
{
	ScrnInfoPtr screen;

	TRACE("%p %x %d %p %p %p %p %p %p\n",
		pScrn, scrnFlag, entityIndex, p_chip, dummy, init,
		enter, leave, private);

	screen = calloc(1, sizeof(*screen));
	screen->entityList = calloc(1, sizeof(screen->entityList[0]));
	screen->numEntities = 1;
	// used by atiddxIsCrossDisplay_3
	screen->confScreen = calloc(1, sizeof(*screen->confScreen));

	xf86Screens = calloc(2, sizeof(xf86Screens[0]));
	return (xf86Screens[0] = screen);
}

Bool
xf86ComparePciBusString(const char *busID, int bus, int device, int func)
{
	TRACE("'%s' %d:%d:%d\n", busID, bus, device, func);
	return TRUE;
}

static struct pci_device *cur_pci_dev;

int
xf86ClaimPciSlot(struct pci_device *d, DriverPtr drvp,
                 int chipset, GDevPtr dev, Bool active)
{
	TRACE("%p %p %d %p %d\n", d, drvp, chipset, dev, active);
	cur_pci_dev = d;

	return 0;
}

struct pci_device *
xf86GetPciInfoForEntity(int entityIndex)
{
	TRACE("%d\n", entityIndex);

	return cur_pci_dev;
}

EntityInfoPtr
xf86GetEntityInfo(int entityIndex)
{
	EntityInfoRec *ret;

	TRACE("%d\n", entityIndex);

	ret = calloc(1, sizeof(*ret));
	ret->device = &drec;
	ret->location.id.pci = cur_pci_dev;

	return ret;
}

static int numprivs;
static DevUnion *privs;

int
xf86AllocateEntityPrivateIndex(void)
{
	int ret;

	ret = numprivs++;
	TRACE("numprivs=%d\n", numprivs);

	privs = realloc(privs, numprivs * sizeof(DevUnion));
	return ret;
}

DevUnion *
xf86GetEntityPrivate(int entityIndex, int privIndex)
{
	DevUnion *ret = NULL;

	if (entityIndex != 0 || (unsigned int)privIndex > (unsigned int)numprivs)
		ret = NULL;
	else
		ret = &privs[privIndex];

	TRACE("%d %d; ret=%p\n", entityIndex, privIndex, ret);
	return ret;
}

void
xf86SetEntitySharable(int entityIndex)
{
	TRACE("%d\n", entityIndex);
}

int
xf86GetNumEntityInstances(int entityIndex)
{
	TRACE("%d\n", entityIndex);
	return 1;
}

GDevPtr
xf86GetDevFromEntity(int entityIndex, int instance)
{
	TRACE("%d %d\n", entityIndex, instance);
	return &drec;
}

int
xf86LoadKernelModule(const char *modName)
{
	TRACE("'%s'\n", modName);
	return 1;
}

pointer
XNFalloc(unsigned long n)
{
	return malloc(n);
}

pointer
XNFcalloc(unsigned long n)
{
	return calloc(1, n);
}

void *
XNFrealloc(void *ptr, unsigned long amount)
{
	return realloc(ptr, amount);
}

void
xf86DrvMsg(int scrnIndex, MessageType type, const char *format, ...)
{   
	va_list ap;

	printf("xf86DrvMsg: ");             
	va_start(ap, format);                                        
	vprintf(format, ap);             
	va_end(ap);
	fflush(stdout);
}   

void
xf86VDrvMsgVerb(int scrnIndex, MessageType type, int verb, const char *format,
                va_list args)
{
	printf("xf86VDrvMsgVerb: ");             
	vprintf(format, args);             
	fflush(stdout);
}

void xf86Msg(MessageType type, const char *format, ...)
{
	va_list ap;

	printf("xf86Msg: ");             
	va_start(ap, format);                                        
	vprintf(format, ap);             
	va_end(ap);
	fflush(stdout);
}

void xf86MsgVerb(MessageType type, int verb, const char *format, ...)
{
	va_list ap;

	printf("xf86MsgVerb: ");             
	va_start(ap, format);                                        
	vprintf(format, ap);             
	va_end(ap);
	fflush(stdout);
}

CARD32
GetTimeInMillis(void)
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
}

Bool
xf86GetOptValBool(const OptionInfoRec *table, int token, Bool *value)
{
	TRACE("%p/%s %d %p\n", table, table ? table->name : NULL, token, value);
	return FALSE;
}

static DriverPtr g_driver;

void
xf86AddDriver(DriverPtr driver, pointer module, int flags)
{
	TRACE("%p %p %x\n", driver, module, flags);             
	g_driver = driver;
}

static void scan_pci(void)
{
	static struct pci_slot_match match = {
		PCI_MATCH_ANY, PCI_MATCH_ANY, PCI_MATCH_ANY, PCI_MATCH_ANY, 0
	};
	struct pci_device_iterator *iter;
	struct pci_device *info;
	int ret;

	ret = pci_system_init();
	if (ret != 0) {
		fprintf(stderr, "pci_system_init failed\n");
		return;
	}

	iter = pci_slot_match_iterator_create(&match);
	while ((info = pci_device_next(iter)) != NULL) {
		if ((info->device_class & 0xff0000) != 0x30000)
			// not display
			continue;

		pci_device_probe(info);
		if (pci_device_is_boot_vga(info))
			break;
	}
	pci_iterator_destroy(iter);

	if (info == NULL) {
		fprintf(stderr, "no display devices?\n");
		return;
	}

	if (info->vendor_id != 0x1002) {
		fprintf(stderr, "not AMD display? (%04x)\n",
			info->vendor_id);
		return;
	}

	//exit(1);
}

static int module_desc[64];

int main(int argc, char *argv[])
{
	XF86ModuleData *mod_data;
	ScrnInfoPtr screen = NULL;
	pthread_mutex_t mutex;
	int errmaj = 0, errmin = 0;
	int retval = 1;
	void *setup_ret;
	void *mod;
	int fd, ret;

	// init pthreads
	pthread_mutex_init(&mutex, NULL);

#if 0
	// preload libpciaccess as it's not in fglrx's deps
	mod = dlopen("libpciaccess.so", RTLD_LAZY | RTLD_GLOBAL);
	if (mod == NULL)
		fprintf(stderr, "preload pciaccess: %s\n", dlerror());
#endif
	// fglrx relies on pci_system_init() being called too
	scan_pci();

	mod = dlopen("/usr/lib/xorg/modules/drivers/fglrx_drv.so",
		RTLD_LAZY | RTLD_GLOBAL);
	if (mod == NULL) {
		fprintf(stderr, "open module: %s\n", dlerror());
		return 1;
	}

	mod_data = dlsym(mod, "fglrxModuleData");
	if (mod_data == NULL) {
		fprintf(stderr, "dlsym fglrxModuleData: %s\n", dlerror());
		goto out;
	}

	if (mod_data->vers == NULL) {
		fprintf(stderr, "mod_data->vers is NULL\n");
		goto out;
	}

	printf("name: %s\n", mod_data->vers->modname);
	printf("vendor: %s\n", mod_data->vers->vendor);
	printf("abiclass: %s\n", mod_data->vers->abiclass);
	printf("moduleclass: %s\n", mod_data->vers->moduleclass);
	printf("version: %u.%u.%u\n", mod_data->vers->majorversion,
		mod_data->vers->minorversion, mod_data->vers->patchlevel);
	fflush(stdout);

	// fglrx assumes descriptor 0 to be Xorg.log
	fd = open("/var/log/Xorg.0.log", O_RDONLY);
	if (fd == -1) {
		perror("open Xorg.0.log");
		goto out;
	}
	ret = dup2(fd, 0);
	if (ret == -1) {
		perror("dup2");
		goto out;
	}

	if (mod_data->setup == NULL) {
		fprintf(stderr, "mod_data->setup is NULL\n");
		goto out;
	}

	setup_ret = mod_data->setup(module_desc, NULL, &errmaj, &errmin);
	printf("setup: %p, errmaj %u, errmin %u\n", setup_ret, errmaj, errmin);
	if (setup_ret == NULL) {
		fprintf(stderr, "setup() returned NULL\n");
		goto out;
	}

	if (g_driver == NULL) {
		fprintf(stderr, "setup() did not provide driver ptr\n");
		goto out;
	}

	printf("driverName: %s\n", g_driver->driverName);
	if (g_driver->Probe == NULL) {
		fprintf(stderr, "Probe is NULL\n");
		goto out;
	}

	// #0  in xdl_xs111_atiddxPxEarlyCheck ()
	// #1  in xdl_xs111_atiddxProbe ()
	// #2  in atiddxProbe ()
	ret = g_driver->Probe(g_driver, PROBE_DETECT);
	printf("Probe(PROBE_DETECT): %d\n", ret);
	if (!ret)
		goto out;

	ret = g_driver->Probe(g_driver, PROBE_DEFAULT);
	printf("Probe(PROBE_DEFAULT): %d\n", ret);
	if (!ret)
		goto out;

#if 0
	if (xf86Screens == NULL || xf86Screens[0] == NULL) {
		fprintf(stderr, "xf86Screens[0] did not get set\n");
		goto out;
	}
	screen = xf86Screens[0];
	if (screen->PreInit == NULL) {
		fprintf(stderr, "screen->PreInit did not get set\n");
		goto out;
	}
	ret = screen->PreInit(screen, 0);
	printf("PreInit: %d\n", ret);
#endif

	retval = !ret;

out:
	(void)screen;
	dlclose(mod);
	return retval;
}
