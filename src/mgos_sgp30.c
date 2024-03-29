#include "mgos.h"
#include <mgos_i2c.h>

#include "mgos_sgp30.h"

#include "sensirion_common.h"
#include "sgp30.h"

const int MGOS_SGP30_ERROR = -128;

/*
const struct mgos_config_i2c cfg = {
	.enable= true,
	.freq= 400,
	.debug= 0,
	.sda_gpio= 13,
	.scl_gpio= 14
};
*/

int8_t sensirion_i2c_init(void) {
/*		struct mgos_i2c *i2c = mgos_i2c_create(&cfg);
    if (NULL == i2c)
    {
				LOG(LL_INFO, ("Could not create i2c bus 1 instance"));
        return -1;
    }*/
	return 0;
}

int8_t sensirion_i2c_read(uint8_t address, uint8_t* data, uint16_t count)
{
    //struct mgos_i2c *i2c = mgos_i2c_get_global();
		struct mgos_i2c *i2c = mgos_i2c_get_bus(1);
    if (NULL == i2c)
    {
        //LOG(LL_INFO, ("Could not get i2c global instance"));
				LOG(LL_INFO, ("Could not get i2c bus 1 instance"));
        return -1;
    }
    bool ok = mgos_i2c_read(i2c, address, data, count, true /* stop */);
    return ok ? 0 : -2;
}

int8_t sensirion_i2c_write(uint8_t address, const uint8_t* data, uint16_t count)
{
    //struct mgos_i2c *i2c = mgos_i2c_get_global();
		struct mgos_i2c *i2c = mgos_i2c_get_bus(1);
    if (NULL == i2c)
    {
        //LOG(LL_INFO, ("Could not get i2c global instance"));
				LOG(LL_INFO, ("Could not get i2c bus 1 instance"));
        return -1;
    }

    bool ok = mgos_i2c_write(i2c, address, data, count, true /* stop */);
    return ok ? 0 : -2;
}

void sensirion_sleep_usec(uint32_t useconds)
{
    mgos_msleep((useconds / 1000) + 1);
}

bool mgos_mgos_sgp30_init(void) {
  return true;
}


bool mgos_sgp30_init(void) {
  return true;
}

bool mgos_sgp30_setup(int DoInit)
{
    s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;

    if (sgp_probe(DoInit) != STATUS_OK) {
        LOG(LL_INFO, ("SGP30 device not found"));
        //sensirion_sleep_usec(1000000);
		return(0);
    }
    LOG(LL_INFO, ("SGP30 sensor"));

    if ((err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal, &scaled_h2_signal)) != STATUS_OK) {
        LOG(LL_ERROR, ("SGP30 measure signals read failed"));
        return false;        
    }

    LOG(LL_ERROR, ("Ethanol signal: %f", scaled_ethanol_signal / 512.0f));

    if (DoInit) {	
	    if ((err = sgp_iaq_init()) != STATUS_OK) {
		LOG(LL_ERROR, ("SGP30 iaq init failed"));
		return false;
	    }
    }
	
    return true;
}

int8_t mgos_sgp30_read(struct mgos_sgp30_data* data)
{
//tph     u16 i = 0;
    s16 err;
    u16 tvoc_ppb, co2_eq_ppm;
    u32 iaq_baseline;

    if ((err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm)) != STATUS_OK) {
        LOG(LL_ERROR, ("SGP30 measure iaq read failed"));
        return -1;
    }

    data->tvoc = tvoc_ppb;
    data->co2 = co2_eq_ppm;

#if 0 //tph
    if (++i % 3600 == 3599) {
        err = sgp_get_iaq_baseline(&iaq_baseline);
        if (err == STATUS_OK) {
            /* IMPLEMENT: store baseline to presistent storage */
        }
    }

    sensirion_sleep_usec(1000000);
#endif
	
    return 0;
}

struct mgos_sgp30_data* mgos_sgp30_data_create()
{
    return calloc(1, sizeof (struct mgos_sgp30_data));
}

void mgos_sgp30_data_delete(struct mgos_sgp30_data* data)
{
    if (NULL != data) {
        free(data);
    }
}

int mgos_sgp30_data_get_tvoc(const struct mgos_sgp30_data* data)
{
    return (NULL != data) ? data->tvoc : MGOS_SGP30_ERROR;
}

int mgos_sgp30_data_get_co2(const struct mgos_sgp30_data* data)
{
    return (NULL != data) ? data->co2 : MGOS_SGP30_ERROR;
}

int mgos_sgp30_set_absolute_humidity(uint32_t absolute_humidity) 
{
	return((int)sgp_set_absolute_humidity(absolute_humidity));
}
