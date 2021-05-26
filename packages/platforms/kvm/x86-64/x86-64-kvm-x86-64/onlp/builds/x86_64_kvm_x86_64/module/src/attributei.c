#include <onlp/platformi/attributei.h>
#include <onlp/stdattrs.h>
#include <onlplib/file.h>

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
  return ONLP_STATUS_OK;
}

/**
 * @brief Get an OID's Asset attribute.
 * @param oid The target OID.
 * @param rv [out] Receives the Asset information if supported.
 * @note if rv is NULL you should only return whether the attribute is supported.
 */
int onlp_attributei_asset_info_get(onlp_oid_t oid, onlp_asset_info_t* rp)
{
  return ONLP_STATUS_OK;
}