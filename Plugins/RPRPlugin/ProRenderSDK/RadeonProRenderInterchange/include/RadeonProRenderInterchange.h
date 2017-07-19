#pragma once
#ifndef RPRI_INTERCHANGE_API_H
#define RPRI_INTERCHANGE_API_H 1

#include <stdint.h>

#define RPRI_FALSE (rpriBoolean) 0
#define RPRI_TRUE (rpriBoolean) 1
#define RPRI_MAX_INFO_VERBOSITY 5

#ifdef __cplusplus
extern "C"
{
#endif // end C++ only

typedef struct rpriContext_t {} *rpriContext;

typedef unsigned int rpriBoolean;

enum RPRI_IMPORT_OPTIONS
{
};

enum RPRI_EXPORT_OPTIONS
{
};

struct rpriExportProperty
{
    char const * name;
    uintptr_t value;
};

typedef rpriExportProperty rpriImportProperty;

// WIP
// For RPR Export module only
struct rpriExportRprResult
{
   size_t maxScenes;
   size_t numScenes;
   void** rprScenes; // should be maxScenes in size
};

struct rpriExportRprMaterialResult
{
	uint32_t type; // 0 = rprMaterial, 1 = rprMaterialX
	void* data;
};

typedef void(*rpriLogCallback)(char const * _log);


/**
* \brief Allocates a rpif::Context and store it in _out for the caller
* to pass into the API.
* \param _out pointer to store the rpriContext that is allocate
* \return RPRI_TRUE if succesful or RPRI_FALSE if not
*/
extern rpriBoolean rpriAllocateContext(rpriContext* _out);

/**
* \brief Frees a context from rpriAllocateContext
* \param _ctx null or a context returned from rpriAllocateContext
*/
extern void rpriFreeContext(rpriContext _ctx);

/**
* \brief Sets the error options for this context
* \param _ctx rpif context
* \param _infoVerbosity how verbose info logs are,
*                               must be < RPRI_MAX_INFO_VERBOSITY
* \param _breakOnError should the debugger fire on error logs?
*                               (maybe not be supported on all platforms)
* \param _abortOnError should an abort call occurs on error logs?
* \return RPRI_FALSE if something went wrong, RPRI_TRUE if set okay
*/
extern rpriBoolean rpriErrorOptions(rpriContext _ctx,
                unsigned int _infoVerbosity,
                rpriBoolean _breakOnError,
                rpriBoolean _abortOnError);
/**
* \brief Sets the rpri loggers to supplied callbacks or reset to defaults
* \param _ctx previously allocated rpifContext
* \param _info callback used \?0#'/,for info, nullptr turns off
* \param _warn callback used for warns, nullptr turns off
* \param _error callback used for errors, nullptr uses os error stream
* \return RPRI_FALSE if something went wrong, RPRI_TRUE if set okay
*/
extern rpriBoolean rpriSetLoggers(rpriContext _ctx,
                rpriLogCallback _info,
                rpriLogCallback _warn,
                rpriLogCallback _error);




/**
 *
 */
extern rpriBoolean rpriGetImporters(rpriContext _ctx,
                uint32_t * _num, char const * * * _importers);
/**
*
*/
extern rpriBoolean rpriGetExporters(rpriContext _ctx,
                uint32_t * _num, char const * * * _importers);

extern rpriBoolean rpriGetExporters(rpriContext _ctx,
                uint32_t * _num, char const * * * _importers);

// Generic API is only available in C++ but is always available
#if defined(__cplusplus)
#include "rprigenericapi.h"
#endif

extern rpriBoolean rpriImport(rpriContext _ctx,
                char const * _importer, char const * _path);

extern rpriBoolean rpriImportFromMemory(rpriContext _ctx,
    char const* _importer,
    uint32_t const _numProperties,
    rpriImportProperty const* _properties);

extern rpriBoolean rpriExport(rpriContext _ctx,
                                char const * _exporter,
                                uint32_t const _numProperties,
                                rpriExportProperty const * _properties);


    #ifdef __cplusplus
}
#endif // end C++ only

#endif // end RPIF_LOADSTORE_API_H
