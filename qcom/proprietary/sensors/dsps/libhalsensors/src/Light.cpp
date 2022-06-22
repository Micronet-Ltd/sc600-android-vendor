/*============================================================================
  @file Light.cpp

  @brief
  Light class implementation.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "Light.h"

/*============================================================================
  Light Constructor
============================================================================*/
Light::Light(int handle)
    :SMGRSensor(handle)
{
    trigger_mode = SENSOR_MODE_EVENT;
    (handle == HANDLE_LIGHT_WAKE_UP)?(bWakeUp = true):(bWakeUp = false);
}

/*============================================================================
  Light Destructor
============================================================================*/
Light::~Light()
{
    if (enabled) {
        enable(0);
    }
}
/*===========================================================================
  FUNCTION:  setSensorInfo
    Fill the sensor information from the sns_smgr_sensor_datatype_info_s_v01 type
    Parameters
    @datatype : sensor information got from the sensor1 callback
===========================================================================*/
void Light::setSensorInfo(sns_smgr_sensor_datatype_info_s_v01* sensor_datatype)
{
    HAL_LOG_DEBUG("%s: Light DTy: %d", __FUNCTION__, sensor_datatype->DataType);
    setType(SENSOR_TYPE_LIGHT);

    if (bWakeUp == false)  {
        setFlags(SENSOR_FLAG_ON_CHANGE_MODE);
    } else {
        setFlags(SENSOR_FLAG_ON_CHANGE_MODE|SENSOR_FLAG_WAKE_UP);
        strlcat(name," -Wakeup",SNS_MAX_SENSOR_NAME_SIZE);
    }
    setResolution((float)((float)sensor_datatype->Resolution *
                                                            UNIT_CONVERT_LIGHT));
    setMaxRange((float)((float)sensor_datatype->MaxRange *
                                                            UNIT_CONVERT_LIGHT));
    return;
}

/*===========================================================================
  FUNCTION:  processReportInd
    process the sensor data indicator from the sensor1 type to sensor event type
    Parameters
    @smgr_data : the sensor1 data message from the sensor1 callback
    @sensor_data : the sensor event message that will be send to framework
===========================================================================*/
#include <unistd.h>
void Light::processReportInd(sns_smgr_periodic_report_ind_msg_v01* smgr_ind,
            sns_smgr_data_item_s_v01* smgr_data, sensors_event_t &sensor_data)
{
    int fd = -1;
    int ir_led_bri = 0, ir_exaggeration_2_linearity = 0, dsp_raw;
    char bri[4];
    UNREFERENCED_PARAMETER(smgr_ind);
    sensor_data.type = SENSOR_TYPE_LIGHT;

    if(bWakeUp == false) {
        sensor_data.sensor = HANDLE_LIGHT;
        HAL_LOG_VERBOSE("%s:light sensor %s ",__FUNCTION__,
                    Utility::SensorTypeToSensorString(getType()));
    } else {
        sensor_data.sensor = HANDLE_LIGHT_WAKE_UP;
        HAL_LOG_VERBOSE("%s:light sensor %s (wake_up)",__FUNCTION__,
                        Utility::SensorTypeToSensorString(getType()));
    }

    dsp_raw = smgr_data->ItemData[0];
#if 0
    if ((dsp_raw & (1 << 31)) && !(dsp_raw & (1 << 30))) {
        dsp_raw |= 1 << 30;
    }
    if ((dsp_raw & (1 << 30)) && !(dsp_raw & (1 << 29))) {
        dsp_raw |= 1 << 29;
    }
    dsp_raw &= 0x3FFFFFFF; 
#endif
    if (fd < 0) {
        fd = open("/sys/class/leds/lcd-backlight/brightness", O_RDONLY);
    }
    if (fd > 0) {
        if (read(fd, &bri, 4) > 0) {
            int i = 0;
            ir_led_bri = 0;
        	while (i < 3 && (bri[i] != '\n')) {
        		ir_led_bri *= 10;
        		ir_led_bri += bri[i++] - 0x30;
        	}
            ir_exaggeration_2_linearity = 64*ir_led_bri/100;
            ir_led_bri *= 8;
            //dsp_raw -= 0x00460000*ir_led_bri;
            if (dsp_raw < 0) {
                //dsp_raw = 0;
            }
        } else {
            ir_led_bri = 0;
        }
        close(fd);
    }
//    sensor_data.light = (float)(smgr_data->ItemData[0]) * UNIT_CONVERT_LIGHT;
    sensor_data.light = (float)(dsp_raw) * UNIT_CONVERT_LIGHT;
    if (ir_led_bri > 0 && sensor_data.light > (float)ir_led_bri) {
        sensor_data.light -= ir_led_bri;
    }
    sensor_data.light /= 10.0;
    if (ir_exaggeration_2_linearity > 0 && sensor_data.light > (float)ir_exaggeration_2_linearity) {
        sensor_data.light -= ir_exaggeration_2_linearity;
    }
    HAL_LOG_INFO("Light::%s: %x %x %d %d %f[%f]", __FUNCTION__, 
                    smgr_data->ItemData[0], dsp_raw, ir_led_bri, ir_exaggeration_2_linearity,
                    sensor_data.light, (float)(smgr_data->ItemData[0]) * UNIT_CONVERT_LIGHT );
}

/*===========================================================================
  FUNCTION:  prepareAddMsg
    SMGRSensor::SMGRPrepareAddMsg will call this function and this func will
    fill the item that needed for this type of sensor.
    Parameters
    @buff_req : the sensor1 message buffer
===========================================================================*/
void Light::prepareAddMsg(sns_smgr_buffering_req_msg_v01 **buff_req)
{
    (*buff_req)->Item[0].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
    (*buff_req)->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
}
