/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <omadm_service_api.h>
#include <pal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "DiagMon.WiFi"

#include "./pal_internal.h"

using namespace android;
using namespace omadm_service_api;

/**
  * Gets WiFi status of the device..
  *
  * @param[out] outdata WiFi status of the device (0 - WiFi is Disabled, not equals 0 - WiFi is Enabled, )
  *
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_state_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_STATE, outdata);
}

/**
  * Gets the speed in Mbps at which the WiFi connection is connected at.
  *
  * @param[out] outdata
  * @parblock
  *  WiFI current link speed in Mbps in integer format,
  *
  *  0 if device is not connected to a WiFi network
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_speed_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_SPEED, outdata);
}


/**
  * Gets the detailed state of the supplicant's negotiation with an access point.
  *
  * @param[out] outdata
  * @parblock
  *  state of WiFi adapter, possible values:
  *
  *           0 ASSOCIATED
  *           1 ASSOCIATING
  *           2 AUTHENTICATING
  *           3 COMPLETED
  *           4 DISCONNECTED
  *           5 DORMANT
  *           6 FOUR_WAY_HANDSHAKE
  *           7 GROUP_HANDSHAKE
  *           8 INACTIVE
  *           9 INTERFACE_DISABLED
  *           10 INVALID
  *           11 SCANNING
  *           12 UNINITIALIZED
  * @endparblock
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_status_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_STATUS, outdata);
}

/**
  * Returns the SSID of each of the Wi-Fi networks that are visible to the device
  * along with the RSSI values for those networks and the Network Security option.
  *
  * @param[out] outbuf - output data in format:
  * @parblock
  *
  *  size    size of received data in bytes
  *
  *  data    represents SSID,RSSI and Network Security option of networks that are visible
  *
  *  Format of outbuf->data:
  *  -Multiple networks are separated by "++" (two plus signs without quotes);
  *  -SSID, RSSI value, and Security Option for one Network are separated by "," (comma without quotes);
  *  -Multiple Security options are separated by "_";
  *  -NO extra blank spaces between or around the values;
  *  -SSID name(s) are exactly (case and format preserved) as they are sent by the Access Point with Blank spaces in SSID Name(s) if exsist;
  *  -For RSSI values, only the numbers are reported with maximum of 1 decimal place. The word dBm is not reported as part of the result;
  *  -For Network security option, possible values are:
  *    0 Unsecure
  *    1 WEP
  *    2 WPA
  *    3 WPA2 Personal
  *    4 WPA2 Enterprise
  *
  *  For example, if 3 networks are visible and:
  *  If the SSID names are as follows: Sample SSID1, Sample_SSID2, and SampleSSID3
  *  And their respective RSSI values are: -90dBm, -80.5dBm, -75dBm
  *  And their respective Network Security options are: Non-Secure, WEP Protected, and (WEP + WPA + WPA2 personal)
  *  Then, the response from the device is: Sample SSID1,-90,0++Sample_SSID2,-80.5,1++SampleSSID3,-75,1_2_3
  *
  *  If no networks are available, then the return value is N/A,N/A,N/A for SSID, RSSI, and Security Option.
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_networks_get(data_buffer_t *outbuf)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_NETWORKS, outbuf);
}

/**
  * Gets the received signal strength indicator of the current WiFi network (expressed in dBm).
  *
  * @param[out] outdata
  * @parblock
  *  Received Signal Strength in dBm in integer format,
  *
  *  0 if device is not connected to a WiFi network
  * @endparblock
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_signal_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_SIGNAL, outdata);
}

/**
  * Gets the service set identifier (SSID) of the current WiFi Network device is connected to.
  *
  * @param[out] outbuf
  * @parblock
  *
  *  size    size of received data in bytes
  *
  *  data    value of case sensitive broadcasted SSID of the current WiFi Network
  *
  *          "N/A" if device is not connected to a WiFi network
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_ssid_get(data_buffer_t *outbuf)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_SSID, outbuf);
}

/**
  * Returns the status of Device's Hotspot functionality.
  *
  * @param[out] outdata state of Device's Hotspot (1 - Hotspot is Enabled, 0 - Hotspot is Disabled)
  *
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_hotspot_state_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_HOTSPOT_STATE, outdata);
}

/**
  * Returns the MAC address of the WiFi adapter of the device.
  *
  * @param[out] outbuf
  * @parblock
  *
  *  size    size of received data in bytes
  *
  *  data    MAC address in format XX:XX:XX:XX:XX:XX, all letters are in upper case.
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_system_wifi_mac_get(data_buffer_t *outbuf)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_SYS_GET_WIFI_MAC, outbuf);
}

/**
  * Gets the Basic Service Set ID value of the Access point the device is connected to.
  * This is the MAC address of the Access Point that the device is connected to
  *
  * @param[out] outbuf
  * @parblock
  *
  *  size    size of received data in bytes
  *
  *  data    MAC address in format XX:XX:XX:XX:XX:XX, all letters are in upper case
  *
  *          "00:00:00:00:00:00" if the device is not connected to an Access Point
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_network_wifi_bssid_get(data_buffer_t *outbuf)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GET_WIFI_BSSID, outbuf);
}

/**
  * Sets WiFi mode to Enabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_wifi_enable()
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_SET_WIFI_ENABLE);
}
/**
  * Sets WiFi mode to Disabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_wifi_disable()
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_SET_WIFI_DISABLE);
}


/**
  * Sets HotSpot to Disabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_wifi_hotspot_disable()
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_SET_WIFI_HOTSPOT_DISABLE);
}
