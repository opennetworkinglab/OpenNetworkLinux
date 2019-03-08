#include <onlp/platformi/attributei.h>
#include <onlp/stdattrs.h>
#include <onlplib/file.h>
#include "platform_lib.h"

#define ONIE_FIELD_CPY(dst, src, field) \
  if (src.field != NULL) { \
    dst->field = malloc(strlen(src.field) + 1); \
    strcpy(dst->field, src.field); \
  } \


/**
 * @brief Initialize the attribute subsystem.
 */
int onlp_attributei_sw_init(void) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Initialize the attribute subsystem.
 */
int onlp_attributei_hw_init(uint32_t flags) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Determine whether the OID supports the given attributei.
 * @param oid The OID.
 * @param attribute The attribute name.
 */
int onlp_attributei_supported(onlp_oid_t oid, const char* attribute) {
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Set an attribute on the given OID.
 * @param oid The OID.
 * @param attribute The attribute name.
 * @param value A pointer to the value.
 */
int onlp_attributei_set(onlp_oid_t oid, const char* attribute, void* value) {
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Get an attribute from the given OID.
 * @param oid The OID.
 * @param attribute The attribute to retrieve.
 * @param[out] value Receives the attributei's value.
 */
int onlp_attributei_get(onlp_oid_t oid, const char* attribute,
                        void** value) {
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Free an attribute value returned from onlp_attributei_get().
 * @param oid The OID.
 * @param attribute The attribute.
 * @param value The value.
 */
int onlp_attributei_free(onlp_oid_t oid, const char* attribute, void* value) {
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * Access to standard attributes.
 */

/**
 * @brief Get an OID's ONIE attribute.
 * @param oid The target OID
 * @param rv [out] Receives the ONIE information if supported.
 * @note if rv is NULL you should only return whether the attribute is supported.
 */
int onlp_attributei_onie_info_get(onlp_oid_t oid, onlp_onie_info_t* rp)
{
    if(rp == NULL) {
        return ONLP_STATUS_OK;
    }

    return onlp_onie_decode_file(rp, IDPROM_PATH);
}

/**
 * @brief Get an OID's Asset attribute.
 * @param oid The target OID.
 * @param rv [out] Receives the Asset information if supported.
 * @note if rv is NULL you should only return whether the attribute is supported.
 */
int onlp_attributei_asset_info_get(onlp_oid_t oid, onlp_asset_info_t* rp)
{
    if(rp == NULL) {
        return ONLP_STATUS_OK;
    }

    onlp_onie_info_t onie_info;
    onlp_attributei_onie_info_get(oid, &onie_info);

    rp->oid = oid;
    ONIE_FIELD_CPY(rp, onie_info, manufacturer)
    ONIE_FIELD_CPY(rp, onie_info, part_number)
    ONIE_FIELD_CPY(rp, onie_info, serial_number)
    ONIE_FIELD_CPY(rp, onie_info, manufacture_date)
    rp->date = NULL;
    rp->hardware_revision = NULL;
    rp->firmware_revision = NULL;
    rp->cpld_revision = NULL;
    rp->description = NULL;
    rp->additional = NULL;

    return ONLP_STATUS_OK;
}