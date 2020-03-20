#ifndef __ATMO_FILESYSTEM_H_
#define __ATMO_FILESYSTEM_H_

/** \addtogroup Filesystem
 * The file system driver is used for the reading and writing of files.
 *  @{
 *
 */


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * Filesystem function return enumerations
 */
typedef enum
{
	ATMO_FILESYSTEM_Status_Success              = 0x00u,  /**< Common - Operation was successful */
	ATMO_FILESYSTEM_Status_Fail                 = 0x01u,  /**< Common - Operation failed */
	ATMO_FILESYSTEM_Status_Initialized          = 0x02u,  /**< Common - Peripheral already initialized */
	ATMO_FILESYSTEM_Status_Invalid              = 0x03u,  /**< Common - Invalid operation or result */
	ATMO_FILESYSTEM_Status_NotSupported         = 0x04u,  /**< Common - Feature not supported by platform */
	ATMO_FILESYSTEM_Status_Unknown              = 0x05u,  /**< Common - Some other status not defined */
} ATMO_FILESYSTEM_Status_t;

/**
 * File open flags
 */
typedef enum
{
	ATMO_RDONLY = 1,        /**< Open a file as read only */
	ATMO_WRONLY = 2,        /**< Open a file as write only */
	ATMO_RDWR   = 3,        /**< Open a file as read and write */
	ATMO_CREAT  = 0x0100,   /**< Create a file if it does not exist */
	ATMO_EXCL   = 0x0200,   /**< Fail if a file already exists */
	ATMO_TRUNC  = 0x0400,   /**< Truncate the existing file to zero size */
	ATMO_APPEND = 0x0800,   /**< Move to end of file on every write */
	ATMO_NORETRY = 0x1000, /**< Don't retry if open fails */

} ATMO_FILESYSTEM_Open_Flags_t;

/**
 * File type
 */
typedef enum
{
	ATMO_FILESYSTEM_Type_File,
	ATMO_FILESYSTEM_Type_Dir,
	ATMO_FILESYSTEM_Type_Unknown
} ATMO_FILESYSTEM_Type_t;

/* Exported Structures -------------------------------------------------------*/

/// \cond DO_NOT_DOCUMENT
// These are intended to be implementation specific data structures
// Opaque to user, just used to pass context to underlying drivers
typedef struct
{
	void *data;
} ATMO_FILESYSTEM_File_t;

typedef struct
{
	void *data;
} ATMO_FILESYSTEM_Directory_t;

typedef struct
{
	ATMO_FILESYSTEM_Type_t type;
	uint32_t size;
	char name[256];
} ATMO_FILESYSTEM_Info_t;
/// \endcond

/**
 * Filesystem driver configuration
 */
typedef struct
{
	unsigned int numRetries; /**< Number of retries on a filesystem operation failure */
	unsigned int retryDelayMs; /**< How long to wait (in milliseconds) before retrying */
} ATMO_FILESYSTEM_Config_t;

typedef struct ATMO_FILESYSTEM_DriverInstance_t ATMO_FILESYSTEM_DriverInstance_t;

struct ATMO_FILESYSTEM_DriverInstance_t
{
	ATMO_FILESYSTEM_Status_t ( *Init )( ATMO_DriverInstanceData_t *instance, ATMO_DriverInstanceHandle_t blockDriverHandle );
	ATMO_FILESYSTEM_Status_t ( *SetConfiguration )( ATMO_DriverInstanceData_t *instance, const ATMO_FILESYSTEM_Config_t *config );
	ATMO_FILESYSTEM_Status_t ( *Wipe )( ATMO_DriverInstanceData_t *instance );
	ATMO_FILESYSTEM_Status_t ( *Mount )( ATMO_DriverInstanceData_t *instance );
	ATMO_FILESYSTEM_Status_t ( *Unmount )( ATMO_DriverInstanceData_t *instance );
	ATMO_FILESYSTEM_Status_t ( *Remove )( ATMO_DriverInstanceData_t *instance, const char *path );
	ATMO_FILESYSTEM_Status_t ( *Rename )( ATMO_DriverInstanceData_t *instance, const char *oldPath, const char *newPath );
	ATMO_FILESYSTEM_Status_t ( *Stat )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_Info_t *info, const char *path );
	ATMO_FILESYSTEM_Status_t ( *FileOpen )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, const char *path, int flags );
	ATMO_FILESYSTEM_Status_t ( *FileClose )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
	ATMO_FILESYSTEM_Status_t ( *FileSync )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
	ATMO_FILESYSTEM_Status_t ( *FileRead )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );
	ATMO_FILESYSTEM_Status_t ( *FileWrite )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );
	ATMO_FILESYSTEM_Status_t ( *FileSeek )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t offset );
	ATMO_FILESYSTEM_Status_t ( *FileTell )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *currPos );
	ATMO_FILESYSTEM_Status_t ( *FileSize )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *size );
	ATMO_FILESYSTEM_Status_t ( *FileRewind )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
	ATMO_FILESYSTEM_Status_t ( *FileTruncate )( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t size );
	ATMO_FILESYSTEM_Status_t ( *DirMk )( ATMO_DriverInstanceData_t *instance, const char *path );
};

/* Exported Function Prototypes -----------------------------------------------*/

ATMO_Status_t ATMO_FILESYSTEM_AddDriverInstance( const ATMO_FILESYSTEM_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize filesystem
 *
 * @param[in] instanceNumber
 * @param[in] blockDriverHandle
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Init( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_DriverInstanceHandle_t blockDriverHandle );

/**
 * Set filesystem configuration
 *
 * @param[in] instanceNumber
 * @param[in] config
 * @return ATMO_FILESYSTEM_Status_t
 *
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_SetConfiguration( ATMO_DriverInstanceHandle_t instanceNumber, const ATMO_FILESYSTEM_Config_t *config );

/**
 * Wipe the whole filesystem
 *
 * @param[in] instanceNumber
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Wipe( ATMO_DriverInstanceHandle_t instanceNumber );

/**
 * Mount filesystem
 *
 * @param[in] instanceNumber
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Mount( ATMO_DriverInstanceHandle_t instanceNumber );

/**
 * Unmount filesystem
 *
 * @param[in] instanceNumber
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Unmount( ATMO_DriverInstanceHandle_t instanceNumber );

/**
 * Remove file/directory
 *
 * @param[in] instanceNumber
 * @param[in] path - path to file/directory
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Remove( ATMO_DriverInstanceHandle_t instanceNumber, const char *path );

/**
 * Rename file/directory
 *
 * @param[in] instanceNumber
 * @param[in] oldPath
 * @param[in] newPath
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Rename( ATMO_DriverInstanceHandle_t instanceNumber, const char *oldPath, const char *newPath );

/**
 * Get info about data at given path (type, size, etc...)
 *
 * @param[in] instanceNumber
 * @param[out] info
 * @param[in] path
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Stat( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_Info_t *info, const char *path );

/**
 * Open file
 * @param[in] file - file structure to be passed in
 * @param[in] path - path to file
 * @param[in] flags - OR of type ATMO_FILESYSTEM_Open_Flags_t
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileOpen( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, const char *path, int flags );

/**
 * Close file
 *
 * @param[in] instanceNumber
 * @param[in] file - file structure to be passed in
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileClose( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file );

/**
 * Sync file with underlying block driver
 *
 * See platform specific documentation for usage details.
 *
 * @param[in] instanceNumber
 * @param[in] filePath
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileSync( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file );

/**
 * Read  from file
 *
 * @param[in] instanceNumber
 * @param[in] file
 * @param[in/out] buffer - buffer to be filled with data
 * @param[in] size - number of bytes to read from file
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileRead( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );

/**
 * Write to file
 *
 * @param[in] instanceNumber
 * @param[in] file
 * @param[in] buffer - buffer to write
 * @param[in] size - number of bytes to write
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileWrite( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );

/**
 * Seek within a file
 *
 * @param[in] instanceNumber
 * @param[in] file
 * @param[in] offset - offset to seek to in bytes
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileSeek( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t offset );

/**
 * Get current position in file
 *
 * @param[in] instanceNumber
 * @param[in] file
 * @param[out] currPos - current position in bytes
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileTell( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t *currPos );

/**
 * Get file size
 *
 * @param[in] instanceNumber
 * @param[in] file
 * @param[out] size - size in bytes
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileSize( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t *size );

/**
 * Seek to beginning of file
 *
 * @param[in] instanceNumber
 * @param[in] file
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileRewind( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file );

/**
 * Truncate file
 *
 * @param[in] instanceNumber
 * @param[in] file
 * @param[in] size - new size in bytes
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileTruncate( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t size );

/**
 * Make a new directory
 *
 * @param[in] instanceNumber
 * @param[in] path
 * @return ATMO_FILESYSTEM_Status_t
 */
ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_DirMk( ATMO_DriverInstanceHandle_t instanceNumber, const char *path );

#ifdef __cplusplus
}
#endif

#endif
/** @}*/