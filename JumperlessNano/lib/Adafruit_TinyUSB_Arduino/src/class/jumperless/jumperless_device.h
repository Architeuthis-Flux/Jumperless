#ifndef _TUSB_JUMPERLESS_DEVICE_H_
#define _TUSB_JUMPERLESS_DEVICE_H_

#ifdef __cplusplus
 extern "C" {
#endif

// Application API
bool tud_jumperless_ready();
bool tud_jumperless_send_measurements(void const* data, uint16_t len);

//--------------------------------------------------------------------+
// Internal Class Driver API
//--------------------------------------------------------------------+
void     tud_jumperless_init            (void);
void     tud_jumperless_reset           (uint8_t rhport);
uint16_t tud_jumperless_open            (uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len);
bool     tud_jumperless_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
bool     tud_jumperless_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes);


#ifdef __cplusplus
 }
#endif
   
#endif
