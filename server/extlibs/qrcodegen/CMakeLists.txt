include(ExternalProject)

ExternalProject_Add(
    ext_qrcodegen
    PREFIX
        ${EXTLIB_INSTALL_DIR}
    URL
	https://github.com/nayuki/QR-Code-generator/archive/v1.5.0.zip
    URL_HASH
    	SHA256=54bce5e2d4871af9e98e3b96b015fc76f493bb385d9004cc3b6f5d1a08ef9510
    DOWNLOAD_DIR
        ${EXTLIB_CACHE_DIR}/qrcodegen
    PATCH_COMMAND
        ${EXTLIB_PATCHER} qrcodegen
    CMAKE_ARGS
        ${EXTLIB_CMAKE_ARGS}
    LOG_DOWNLOAD 1 LOG_UPDATE 0 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
)

add_dependencies(ext_all ext_qrcodegen)
