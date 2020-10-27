// FIXME: this is for testing only, not for production.....
// TODO: test

#include <onlplib/i2c.h>
#include <onlp/platformi/sfpi.h>
#include <onlplib/file.h>
#include "platform_lib.h"

#include "x86_64_accton_wedge100bf_65x_log.h"

#define BIT(i)          (1 << (i))
#define NUM_OF_SFP_PORT 64
static const int sfp_bus_index[] = {
 44, 43, 46, 45, 48, 47, 50, 49,
 52, 51, 54, 53, 56, 55, 58, 57,
 60, 59, 62, 61, 64, 63, 66, 65,
 68, 67, 70, 69, 72, 71, 74, 73, 
  4,  3,  6,  5,  8,  7,  10, 9,
 12,  11, 14, 13, 16, 15, 18, 17,
 20, 19, 22, 21, 24, 23, 26, 25,
 28, 27, 30, 29, 32, 31, 34, 33,
};

/************************************************************
 *
 * SFPI Entry Points
 *
 ***********************************************************/

/**
 * @brief Software initialization of the SFP module.
 */
int onlp_sfpi_sw_init(void) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Hardware initialization of the SFP module.
 * @param flags The hardware initialization flags.
 */
int onlp_sfpi_hw_init(uint32_t flags) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Deinitialize the chassis software module.
 * @note The primary purpose of this API is to properly
 * deallocate any resources used by the module in order
 * faciliate detection of real resouce leaks.
 */
int onlp_sfpi_sw_denit(void) {
    return ONLP_STATUS_OK;
}

int
onlp_sfpi_bitmap_get(onlp_sfp_bitmap_t* bmap)
{
    /*
     * Ports {0, 64}
     */
    int p;
    AIM_BITMAP_CLR_ALL(bmap);

    for(p = 0; p < NUM_OF_SFP_PORT; p++) {
        AIM_BITMAP_SET(bmap, p);
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Determine the SFP connector type.
 * @param id The SFP Port ID.
 * @param[out] rtype Receives the connector type.
 */
int onlp_sfpi_type_get(onlp_oid_id_t id, onlp_sfp_type_t* rtype) {
    *rtype = ONLP_SFP_TYPE_QSFP28;
    return ONLP_STATUS_OK;
}

static uint8_t
onlp_sfpi_reg_val_to_port_sequence(uint8_t value, int revert)
{
    int i;
    uint8_t ret = 0;

    for (i = 0; i < 8; i++) {
        if (i % 2) {
            ret |= (value & BIT(i)) >> 1;
        }
        else {
            ret |= (value & BIT(i)) << 1;
        }
    }

    return revert ? ~ret : ret;
}

int onlp_sfpi_is_present(onlp_oid_id_t port)
{
    /*
     * Return 1 if present.
     * Return 0 if not present.
     * Return < 0 if error.
     */
    int present;
    int bus;  
    int addr = ((port < 16) || (port >= 32 && port <= 47)) ? 0x22 : 0x23; /* pca9535 slave address */
    int offset;

    if(port < 16) {
        bus = 77;
    }else if(port >=16 && port < 32){
        bus = 78;
    }else if(port >=32 && port < 48){
        bus = 37;
    }else if(port >=48 && port <= 63){
        bus = 38;
    }
        
    if ((port < 8) || (port >= 16 && port <= 23) || (port >= 32 && port <= 39) \
            || (port >=48 && port <=55)) {
        offset = 0;
    }
    else {
        offset = 1;
    }

    present = onlp_i2c_readb(bus, addr, offset, ONLP_I2C_F_FORCE);
    if (present < 0) {
        return ONLP_STATUS_E_INTERNAL;
    }

    present = onlp_sfpi_reg_val_to_port_sequence(present, 0);
    return !(present & BIT(port % 8));
}

int
onlp_sfpi_presence_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    int i;
    uint8_t bytes[8] = {0};

    for (i = 0; i < AIM_ARRAYSIZE(bytes); i++) {
        int bus;  
        int addr = ((i < 2)|| (i > 3 && i < 6)) ? 0x22 : 0x23; /* pca9535 slave address */
        int offset = (i % 2);
        /* Adding bus number for upper board QSFP ports */
        switch(i)
        {
            case 0: 
            case 1: 
                    bus = 77;
                    break;
            case 2:
            case 3: 
                    bus = 78;
                    break;
            case 4:
            case 5: 
                    bus = 37;
                    break;
            case 6:
            case 7: 
                    bus = 38;
                    break;
            default:
                    break;
        }

        bytes[i] = onlp_i2c_readb(bus, addr, offset, ONLP_I2C_F_FORCE);
        if (bytes[i] < 0) {
            return ONLP_STATUS_E_INTERNAL;
        }

        bytes[i] = onlp_sfpi_reg_val_to_port_sequence(bytes[i], 1);
    }

    /* Convert to 64 bit integer in port order */
    i = 0;
    uint64_t presence_all = 0;
    for(i = AIM_ARRAYSIZE(bytes)-1; i >= 0; i--) {
        presence_all <<= 8;
        presence_all |= bytes[i];
    }

    /* Populate bitmap */
    for(i = 0; presence_all; i++) {
        AIM_BITMAP_MOD(dst, i, (presence_all & 1));
        presence_all >>= 1;
    }

    return ONLP_STATUS_OK;
}

int
onlp_sfpi_rx_los_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Read bytes from the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr Read offset.
 * @param[out] dst Receives the data.
 * @param len Read length.
 * @returns The number of bytes read or ONLP_STATUS_E_* no error.
 */
int onlp_sfpi_dev_read(onlp_oid_id_t id, int devaddr, int addr,
                       uint8_t* dst, int len) {
    int bus = sfp_bus_index[id];
    onlp_i2c_read(bus, devaddr, addr, len, dst, 0);
    return ONLP_STATUS_OK;
}

/**
 * @brief Write bytes to the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr Write offset.
 * @param src The bytes to write.
 * @param len Write length.
 */
int onlp_sfpi_dev_write(onlp_oid_id_t id, int devaddr, int addr,
                        uint8_t* src, int len) {
    int bus = sfp_bus_index[id];
    onlp_i2c_write(bus, devaddr, addr, len, src, 0);
    return ONLP_STATUS_OK;
}

/**
 * @brief Read a byte from the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The read address.
 * @returns The byte on success or ONLP_STATUS_E* on error.
 */
int onlp_sfpi_dev_readb(onlp_oid_id_t id, int devaddr, int addr) {
    int bus = sfp_bus_index[id];
    return onlp_i2c_readb(bus, devaddr, addr, 0);
}

/**
 * @brief Write a byte to the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The write address.
 * @param value The write value.
 */
int onlp_sfpi_dev_writeb(onlp_oid_id_t id, int devaddr, int addr,
                         uint8_t value) {
    int bus = sfp_bus_index[id];
    onlp_i2c_writeb(bus, devaddr, addr, value, 0);
    return ONLP_STATUS_OK;
}


/**
 * @brief Read a word from the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The read address.
 * @returns The word if successful, ONLP_STATUS_E* on error.
 */
int onlp_sfpi_dev_readw(onlp_oid_id_t id, int devaddr, int addr) {
    int bus = sfp_bus_index[id];
    return onlp_i2c_readw(bus, devaddr, addr, 0);
}

/**
 * @brief Write a word to the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The write address.
 * @param value The write value.
 */
int onlp_sfpi_dev_writew(onlp_oid_id_t id, int devaddr, int addr,
                         uint16_t value) {
    int bus = sfp_bus_index[id];
    onlp_i2c_writew(bus, devaddr, addr, value, 0);
    return ONLP_STATUS_OK;
}

/**
 * @brief Perform any actions required after an SFP is inserted.
 * @param id The SFP Port ID.
 * @param info The SFF Module information structure.
 * @note This function is optional. If your platform must
 * adjust equalizer or preemphasis settings internally then
 * this function should be implemented as the trigger.
 */
int onlp_sfpi_post_insert(onlp_oid_id_t id, sff_info_t* info) {
    return ONLP_STATUS_OK;
}

/**
 * @brief Get the SFP's OID header.
 * @param id The SFP oid.
 * @param [out] hdr Receives the header.
 */
int onlp_sfpi_hdr_get(onlp_oid_id_t id, onlp_oid_hdr_t* hdr) {
    int is_present = onlp_sfp_is_present(id);
    hdr->id = ONLP_SFP_ID_CREATE(id);
    hdr->poid = 0;
    hdr->status = is_present ? ONLP_OID_STATUS_FLAG_PRESENT : 0;
    return ONLP_STATUS_OK;
}

/**
 * @brief Returns whether or not the given control is supported on the given port.
 * @param id The SFP Port ID.
 * @param control The control.
 * @param[out] rv Receives 1 if supported, 0 if not supported.
 * @note This provided for convenience and is optional.
 * If you implement this function your control_set and control_get APIs
 * will not be called on unsupported ports.
 */
int onlp_sfpi_control_supported(onlp_oid_id_t id,
                                onlp_sfp_control_t control, int* rv) {
    *rv = 0;
    return ONLP_STATUS_OK;
}

static int
sfpi_eeprom_read(int port, uint8_t devaddr, uint8_t data[256])
{
    int i;

    /*
     * Read the SFP eeprom into data[]
     *
     * Return MISSING if SFP is missing.
     * Return OK if eeprom is read
     */
    memset(data, 0, 256);

    for (i = 0; i < 128; i++) {
        int bus = sfp_bus_index[port];
        int val = onlp_i2c_readw(bus, devaddr, i*2, ONLP_I2C_F_FORCE);

        if (val < 0) {
            return ONLP_STATUS_E_INTERNAL;
        }

        data[i*2]   = val & 0xff;
        data[(i*2)+1] = (val >> 8) & 0xff;
    }

    return ONLP_STATUS_OK;
}

int
onlp_sfpi_eeprom_read(int port, uint8_t data[256])
{
    return sfpi_eeprom_read(port, 0x50, data);
}

int
onlp_sfpi_dom_read(int port, uint8_t data[256])
{
    return sfpi_eeprom_read(port, 0x51, data);
}

int
onlp_sfpi_denit(void)
{
    return ONLP_STATUS_OK;
}

