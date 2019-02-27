/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2014 Accton Technology Corporation.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/
#include <unistd.h>
#include <onlplib/i2c.h>
#include <onlplib/file.h>
#include <onlp/platformi/psui.h>
#include "platform_lib.h"

#define PSU1_ID 1
#define PSU2_ID 2

/*
 * Get all information about the given PSU oid.
 */
static onlp_psu_info_t pinfo[] =
    {
        { }, /* Not used */
        {
            { ONLP_PSU_ID_CREATE(PSU1_ID), "PSU-1", 0 },
        },
        {
            { ONLP_PSU_ID_CREATE(PSU2_ID), "PSU-2", 0 },
        }
    };

static int
twos_complement_to_int(uint16_t data, uint8_t valid_bit, int mask)
{
    uint16_t valid_data	 = data & mask;
    bool 	 is_negative = valid_data >> (valid_bit - 1);

    return is_negative ? (-(((~valid_data) & mask) + 1)) : valid_data;
}

static int
pmbus_parse_literal_format(uint16_t value)
{
    int exponent, mantissa, multiplier = 1000;

    exponent = twos_complement_to_int(value >> 11, 5, 0x1f);
    mantissa = twos_complement_to_int(value & 0x7ff, 11, 0x7ff);

    return (exponent >= 0) ? (mantissa << exponent) * multiplier :
           (mantissa * multiplier) / (1 << -exponent);
}

/**
 * @brief Software initialization of the PSU module.
 */
int onlp_psui_sw_init(void) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Hardware initialization of the PSU module.
 * @param flags The hardware initialization flags.
 */
int onlp_psui_hw_init(uint32_t flags) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Deinitialize the psu software module.
 * @note The primary purpose of this API is to properly
 * deallocate any resources used by the module in order
 * faciliate detection of real resouce leaks.
 */
int onlp_psui_sw_denit(void) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Get the PSU's oid header.
 * @param id The PSU OID.
 * @param[out] rv Receives the header.
 */
int onlp_psui_hdr_get(onlp_oid_id_t id, onlp_oid_hdr_t* rv) {
    onlp_psu_info_t info;
    onlp_psui_info_get(id, &info);
    *rv = info.hdr;
    return ONLP_STATUS_OK;
}

/**
 * @brief Get the information structure for the given PSU
 * @param id The PSU OID
 * @param[out] rv Receives the PSU information.
 */
int onlp_psui_info_get(onlp_oid_id_t id, onlp_psu_info_t* info) {
    int pid, value, addr;
    uint8_t mask = 0;
    pid  = ONLP_OID_ID_GET(id);
    *info = pinfo[pid]; /* Set the onlp_oid_hdr_t */

    /* Get the present status
     */
    mask = 1 << ((pid-1) * 4);
    value = onlp_i2c_readb(1, 0x32, 0x10, ONLP_I2C_F_FORCE);
    if (value < 0) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (value & mask) {
        info->hdr.status &= ~ONLP_OID_STATUS_FLAG_PRESENT;
        return ONLP_STATUS_OK;
    }
    info->hdr.status |= ONLP_OID_STATUS_FLAG_PRESENT;
    info->type = ONLP_PSU_TYPE_AC;

    /* Get power good status
     */
    mask = 1 << ((pid-1) * 4 + 1);
    if (!(value & mask)) {
        info->hdr.status |= ONLP_OID_STATUS_FLAG_FAILED;
        return ONLP_STATUS_OK;
    }

    /* Get input output power status
     */
    value = (pid == PSU1_ID) ? 0x2 : 0x1; /* mux channel for psu */
    if (bmc_i2c_writeb(7, 0x70, 0, value) < 0) {
        return ONLP_STATUS_E_INTERNAL;
    }

    /* Read vin */
    addr  = (pid == PSU1_ID) ? 0x59 : 0x5a;
    value = bmc_i2c_readw(7, addr, 0x88);
    if (value >= 0) {
        info->mvin = pmbus_parse_literal_format(value);
        info->caps |= ONLP_PSU_CAPS_GET_VIN;
    }

    /* Read iin */
    value = bmc_i2c_readw(7, addr, 0x89);
    if (value >= 0) {
        info->miin = pmbus_parse_literal_format(value);
        info->caps |= ONLP_PSU_CAPS_GET_IIN;
    }

    /* Get pin */
    if ((info->caps & ONLP_PSU_CAPS_GET_VIN) && (info->caps & ONLP_PSU_CAPS_GET_IIN)) {
        info->mpin = info->mvin * info->miin / 1000;
        info->caps |= ONLP_PSU_CAPS_GET_PIN;
    }

    /* Read iout */
    value = bmc_i2c_readw(7, addr, 0x8c);
    if (value >= 0) {
        info->miout = pmbus_parse_literal_format(value);
        info->caps |= ONLP_PSU_CAPS_GET_IOUT;
    }

    /* Read pout */
    value = bmc_i2c_readw(7, addr, 0x96);
    if (value >= 0) {
        info->mpout = pmbus_parse_literal_format(value);
        info->caps |= ONLP_PSU_CAPS_GET_POUT;
    }

    /* Get vout */
    if ((info->caps & ONLP_PSU_CAPS_GET_IOUT) && (info->caps & ONLP_PSU_CAPS_GET_POUT) && info->miout != 0) {
        info->mvout = info->mpout / info->miout * 1000;
        info->caps |= ONLP_PSU_CAPS_GET_VOUT;
    }

    /* Get model name */
    return bmc_i2c_readraw(7, addr, 0x9a, info->model, sizeof(info->model));
}

