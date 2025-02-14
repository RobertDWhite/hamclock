/* code to help orgranize reading and writing bytes to EEPROM using named locations.
 * To add another value, add to both the NV_Name enum in HamClock.h and nv_sizes[] below.
 * N.B. be sure they stay in sync.
 *
 * Storage starts at NV_BASE. Items are stored contiguously without gaps. Each item begins with NV_COOKIE
 * followed by the number of bytes listed in nv_sizes[] whose order must match the NV_ defines.
 */

#include "HamClock.h"


#define NV_BASE         55      // base address, move anywhere else to effectively start fresh
#define NV_COOKIE       0x5A    // magic cookie to decide whether a value is valid


/* number of bytes for each NV_Name.
 * N.B. must be in the same order.
 */
static const uint8_t nv_sizes[NV_N] = {
    4,                          // NV_TOUCH_CAL_A
    4,                          // NV_TOUCH_CAL_B
    4,                          // NV_TOUCH_CAL_C
    4,                          // NV_TOUCH_CAL_D
    4,                          // NV_TOUCH_CAL_E

    4,                          // NV_TOUCH_CAL_F
    4,                          // NV_TOUCH_CAL_DIV
    1,                          // NV_DXMAX_N
    1,                          // NV_DE_TIMEFMT
    4,                          // NV_DE_LAT

    4,                          // NV_DE_LNG
    4,                          // NV_DE_GRID_OLD
    1,                          // NV_DX_DST    not used
    4,                          // NV_DX_LAT
    4,                          // NV_DX_LNG

    4,                          // NV_DX_GRID_OLD
    2,                          // NV_CALL_FG_COLOR
    2,                          // NV_CALL_BG_COLOR
    1,                          // NV_CALL_BG_RAINBOW
    1,                          // NV_PSK_SHOWDIST

    4,                          // NV_UTC_OFFSET
    1,                          // NV_PLOT_1
    1,                          // NV_PLOT_2
    1,                          // NV_BRB_ROTSET_OLD
    1,                          // NV_PLOT_3

    1,                          // NV_RSS_ON
    2,                          // NV_BPWM_DIM
    2,                          // NV_PHOT_DIM
    2,                          // NV_BPWM_BRIGHT
    2,                          // NV_PHOT_BRIGHT

    1,                          // NV_LP
    1,                          // NV_METRIC_ON
    1,                          // NV_LKSCRN_ON
    1,                          // NV_MAPPROJ
    1,                          // NV_ROTATE_SCRN

    NV_WIFI_SSID_LEN,           // NV_WIFI_SSID
    NV_WIFI_PW_LEN_OLD,         // NV_WIFI_PASSWD_OLD
    NV_CALLSIGN_LEN,            // NV_CALLSIGN
    NV_SATNAME_LEN,             // NV_SATNAME
    1,                          // NV_DE_SRSS

    1,                          // NV_DX_SRSS
    1,                          // NV_GRIDSTYLE
    2,                          // NV_DPYON
    2,                          // NV_DPYOFF
    NV_DXHOST_LEN,              // NV_DXHOST

    2,                          // NV_DXPORT
    1,                          // NV_SWHUE
    4,                          // NV_TEMPCORR76
    NV_GPSDHOST_LEN,            // NV_GPSDHOST
    4,                          // NV_KX3BAUD

    2,                          // NV_BCPOWER
    4,                          // NV_CD_PERIOD
    4,                          // NV_PRESCORR76
    2,                          // NV_BR_IDLE
    1,                          // NV_BR_MIN

    1,                          // NV_BR_MAX
    4,                          // NV_DE_TZ
    4,                          // NV_DX_TZ
    NV_COREMAPSTYLE_LEN,        // NV_COREMAPSTYLE
    1,                          // NV_USEDXCLUSTER

    1,                          // NV_USEGPSD
    1,                          // NV_LOGUSAGE
    1,                          // NV_MAPSPOTS
    NV_WIFI_PW_LEN,             // NV_WIFI_PASSWD
    1,                          // NV_NTPSET

    NV_NTPHOST_LEN,             // NV_NTPHOST
    1,                          // NV_GPIOOK
    2,                          // NV_SATPATHCOLOR
    2,                          // NV_SATFOOTCOLOR
    2,                          // NV_X11FLAGS

    2,                          // NV_BCFLAGS
    NV_DAILYONOFF_LEN,          // NV_DAILYONOFF
    4,                          // NV_TEMPCORR77
    4,                          // NV_PRESCORR77
    2,                          // NV_SHORTPATHCOLOR

    2,                          // NV_LONGPATHCOLOR
    2,                          // NV_PLOTOPS
    1,                          // NV_NIGHT_ON
    NV_DE_GRID_LEN,             // NV_DE_GRID
    NV_DX_GRID_LEN,             // NV_DX_GRID

    2,                          // NV_GRIDCOLOR
    2,                          // NV_CENTERLNG
    1,                          // NV_NAMES_ON
    4,                          // NV_PANE1ROTSET
    4,                          // NV_PANE2ROTSET

    4,                          // NV_PANE3ROTSET
    1,                          // NV_AUX_TIME
    2,                          // NV_ALARMCLOCK
    1,                          // NV_BC_UTCTIMELINE
    1,                          // NV_RSS_INTERVAL

    1,                          // NV_DATEMDY
    1,                          // NV_DATEDMYYMD
    1,                          // NV_ROTUSE
    NV_ROTHOST_LEN,             // NV_ROTHOST
    2,                          // NV_ROTPORT

    1,                          // NV_RIGUSE
    NV_RIGHOST_LEN,             // NV_RIGHOST
    2,                          // NV_RIGPORT
    NV_DXLOGIN_LEN,             // NV_DXLOGIN
    1,                          // NV_FLRIGUSE

    NV_FLRIGHOST_LEN,           // NV_FLRIGHOST
    2,                          // NV_FLRIGPORT
    NV_DXCLCMD_LEN,             // NV_DXCMD0
    NV_DXCLCMD_LEN,             // NV_DXCMD1
    NV_DXCLCMD_LEN,             // NV_DXCMD2

    NV_DXCLCMD_LEN,             // NV_DXCMD3
    1,                          // NV_DXCMDUSED
    1,                          // NV_PSK_MODEBITS
    4,                          // NV_PSK_BANDS
    2,                          // NV_160M_COLOR

    2,                          // NV_80M_COLOR
    2,                          // NV_60M_COLOR
    2,                          // NV_40M_COLOR
    2,                          // NV_30M_COLOR
    2,                          // NV_20M_COLOR

    2,                          // NV_17M_COLOR
    2,                          // NV_15M_COLOR
    2,                          // NV_12M_COLOR
    2,                          // NV_10M_COLOR
    2,                          // NV_6M_COLOR

    2,                          // NV_2M_COLOR
    4,                          // NV_DASHED
    1,                          // NV_BEAR_MAG
    1,                          // NV_WSJT_SETSDX
    1,                          // NV_WSJT_DX

    2,                          // NV_PSK_MAXAGE
    1,                          // NV_WEEKMON
    1,                          // NV_BCMODE
    1,                          // NV_SDO
    1,                          // NV_SDOROT

    1,                          // NV_ONTASPOTA
    1,                          // NV_ONTASSOTA
    2,                          // NV_BRB_ROTSET
    2,                          // NV_ROTCOLOR
    1,                          // NV_CONTESTS

    4,                          // NV_BCTOA
    NV_ADIFFN_LEN,              // NV_ADIFFN
    NV_I2CFN_LEN,               // NV_I2CFN
    1,                          // NV_I2CON
    4,                          // NV_DXMAX_T

    NV_DXWLIST_LEN,             // NV_DXWLIST
    1,                          // NV_SCROLLDIR
    1,                          // NV_SCROLLLEN

};




/*******************************************************************
 *
 * internal implementation
 * 
 *******************************************************************/


/* called to init EEPROM. ignore after first call.
 */
static void initEEPROM()
{
    // ignore if called before
    static bool before;
    if (before)
        return;
    before = true;

    uint16_t ee_used, ee_size;
    reportEESize (ee_used, ee_size);
    if (ee_used > ee_size) {
        Serial.printf (_FX("EEPROM too large: %u > %u\n"), ee_used, ee_size);
        while(1);       // timeout
    }
    EEPROM.begin(ee_used);      
    Serial.printf (_FX("EEPROM size %u + %u = %u, max %u\n"), NV_BASE, ee_used-NV_BASE, ee_used, ee_size);

// #define _SHOW_EEPROM
#if defined(_SHOW_EEPROM)
    uint16_t len = 0;
    for (size_t i = 0; i < n; i++) {
        const uint8_t sz = nv_sizes[i];
        uint16_t start = NV_BASE+len;
        Serial.printf (_FX("%3d %3d %3d %02X: "), i, len, sz, EEPROM.read(start));
        start += 1;                     // skip cookie
        switch (sz) {
        case 1: {
            uint8_t i1 = EEPROM.read(start);
            Serial.printf (_FX("%11d = 0x%02X\n"), i1, i1);
            }
            break;
        case 2: {
            uint16_t i2 = EEPROM.read(start) + 256*EEPROM.read(start+1);
            Serial.printf (_FX("%11d = 0x%04X\n"), i2, i2);
            }
            break;
        case 4: {
            uint32_t i4 = EEPROM.read(start) + (1UL<<8)*EEPROM.read(start+1)*(1UL<<16)
                            + (1UL<<16)*EEPROM.read(start+2) + (1UL<<24)*EEPROM.read(start+3);
            float f4;
            memcpy (&f4, &i4, 4);
            Serial.printf (_FX("%11d = 0x%08X = %g\n"), i4, i4, f4);
            }
            break;
        default:
            for (int j = 0; j < sz; j++) {
                uint8_t c = EEPROM.read(start+j);
                if (c < ' ' || c >= 0x7f)
                    Serial.printf (" %02X ", c);
                else
                    Serial.printf ("%c", (char)c);
            }
            Serial.println();
            break;
        }
        len += sz + 1;         // size + cookie
    }
#endif // _SHOW_EEPROM
}


/* given NV_Name, return address of item's NV_COOKIE and its length
 */
static bool nvramStartAddr (NV_Name e, uint16_t *e_addr, uint8_t *e_len)
{
    if (e >= NV_N)
        return(false);
    *e_addr = NV_BASE;
    uint8_t i;
    for (i = 0; i < e; i++)
        *e_addr += 1 + nv_sizes[i];     // + room for cookie
    *e_len = nv_sizes[i];
    return (true);
}

/* write NV_COOKIE then the given array with the expected number of bytes in the given element location.
 * xbytes 0 denotes unknown for strings.
 */
static void nvramWriteBytes (NV_Name e, const uint8_t data[], uint8_t xbytes)
{
    resetWatchdog();

    initEEPROM();

    uint8_t e_len;
    uint16_t e_addr;
    if (!nvramStartAddr (e, &e_addr, &e_len)) {
        Serial.printf (_FX("NVBUG! Write: bad id %d\n"), e);
        return;
    }
    // Serial.printf ("Write %d at %d\n", e_len, e_addr-NV_BASE);
    if (xbytes && e_len != xbytes) {
        Serial.printf (_FX("NVBUG! Write: %d %d != %d bytes\n"), e, e_len, xbytes);
        return;
    }
    EEPROM.write (e_addr++, NV_COOKIE);
    for (uint8_t i = 0; i < e_len; i++)
        EEPROM.write (e_addr++, *data++);
    if (!EEPROM.commit())
        Serial.println(_FX("EEPROM.commit failed"));
    // Serial.printf ("Read back cookie: 0x%02X\n", EEPROM.read(e_addr - e_len -1));
}

/* read NV_COOKIE then the given array for the given element with the given number of expected bytes.
 * xbytes 0 denotes unknown for strings.
 */
static bool nvramReadBytes (NV_Name e, uint8_t *buf, uint8_t xbytes)
{
    resetWatchdog();

    initEEPROM();

    uint8_t e_len;
    uint16_t e_addr;
    if (!nvramStartAddr (e, &e_addr, &e_len)) {
        Serial.printf (_FX("NVBUG! Read: bad id %d\n"), e);
        return (false);
    }
    if (xbytes && e_len != xbytes) {
        Serial.printf (_FX("NVBUG! Read: %d %d != %d bytes\n"), e, e_len, xbytes);
        return (false);
    }
    if (EEPROM.read(e_addr++) != NV_COOKIE)
        return (false);
    for (uint8_t i = 0; i < e_len; i++)
        *buf++ = EEPROM.read(e_addr++);
    return (true);
}



/*******************************************************************
 *
 * external interface
 *
 *******************************************************************/

void reportEESize (uint16_t &ee_used, uint16_t &ee_size)
{
    ee_used = NV_BASE;
    const unsigned n = NARRAY(nv_sizes);
    for (unsigned i = 0; i < n; i++)
        ee_used += nv_sizes[i] + 1;      // +1 for cookie
    ee_size = FLASH_SECTOR_SIZE;
}

/* write the given float value to the given NV_name
 */
void NVWriteFloat (NV_Name e, float f)
{
    nvramWriteBytes (e, (uint8_t*)&f, 4);
}

/* write the given uint32_t value to the given NV_name
 */
void NVWriteUInt32 (NV_Name e, uint32_t u)
{
    nvramWriteBytes (e, (uint8_t*)&u, 4);
}

/* write the given int32_t value to the given NV_name
 */
void NVWriteInt32 (NV_Name e, int32_t i)
{
    nvramWriteBytes (e, (uint8_t*)&i, 4);
}

/* write the given uint16_t value to the given NV_name
 */
void NVWriteUInt16 (NV_Name e, uint16_t u)
{
    nvramWriteBytes (e, (uint8_t*)&u, 2);
}

/* write the given int16_t value to the given NV_name
 */
void NVWriteInt16 (NV_Name e, int16_t i)
{
    nvramWriteBytes (e, (uint8_t*)&i, 2);
}

/* write the given uint8_t value to the given NV_name
 */
void NVWriteUInt8 (NV_Name e, uint8_t u)
{
    nvramWriteBytes (e, (uint8_t*)&u, 1);
}

/* write the given string value to the given NV_name
 */
void NVWriteString (NV_Name e, const char *str)
{
    nvramWriteBytes (e, (uint8_t*)str, 0);
}

/* read the given NV_Name float value, return whether found in NVRAM.
 */
bool NVReadFloat (NV_Name e, float *fp)
{
    return (nvramReadBytes (e, (uint8_t*)fp, 4));
}

/* read the given NV_Name uint32_t value, return whether found in NVRAM.
 */
bool NVReadUInt32 (NV_Name e, uint32_t *up)
{
    return (nvramReadBytes (e, (uint8_t*)up, 4));
}

/* read the given NV_Name int32_t value, return whether found in NVRAM.
 */
bool NVReadInt32 (NV_Name e, int32_t *ip)
{
    return (nvramReadBytes (e, (uint8_t*)ip, 4));
}

/* read the given NV_Name uint16_t value, return whether found in NVRAM.
 */
bool NVReadUInt16 (NV_Name e, uint16_t *up)
{
    return (nvramReadBytes (e, (uint8_t*)up, 2));
}

/* read the given NV_Name int16_t value, return whether found in NVRAM.
 */
bool NVReadInt16 (NV_Name e, int16_t *ip)
{
    return (nvramReadBytes (e, (uint8_t*)ip, 2));
}

/* read the given NV_Name uint8_t value, return whether found in NVRAM.
 */
bool NVReadUInt8 (NV_Name e, uint8_t *up)
{
    return (nvramReadBytes (e, (uint8_t*)up, 1));
}

/* read the given NV_Name string value, return whether found in NVRAM.
 */
bool NVReadString (NV_Name e, char *buf)
{
    return (nvramReadBytes (e, (uint8_t*)buf, 0));
}
