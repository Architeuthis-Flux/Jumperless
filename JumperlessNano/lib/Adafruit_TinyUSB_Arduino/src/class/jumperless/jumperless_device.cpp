#include "tusb_option.h"

#if CFG_TUD_JUMPERLESS

#if CFG_TUD_JUMPERLESS > 1
#error "Only one jumperless interface can be used"
#endif

#include "device/usbd.h"
#include "device/usbd_pvt.h"
#include "jumperless_device.h"

typedef struct {
  uint8_t itf_num;
  uint8_t ep_in;
  uint8_t ep_out;
  CFG_TUSB_MEM_ALIGN uint8_t epin_buf[CFG_TUD_JUMPERLESS_EP_BUFSIZE];
} jl_interface_t;

CFG_TUSB_MEM_SECTION tu_static jl_interface_t _jl_itf[CFG_TUD_JUMPERLESS];

bool tud_jumperless_ready() {
  uint8_t const rhport = 0;
  uint8_t const ep_in = _jl_itf[0].ep_in;
  return tud_ready() && (ep_in != 0) && !usbd_edpt_busy(rhport, ep_in);
}

bool tud_jumperless_send_measurements(void const *data, uint16_t len) {
  uint8_t const rhport = 0;
  jl_interface_t *iface = &_jl_itf[0];
  TU_VERIFY(usbd_edpt_claim(rhport, iface->ep_in));
  TU_VERIFY(0 == tu_memcpy_s(iface->epin_buf, CFG_TUD_JUMPERLESS_EP_BUFSIZE, data, len));
  return usbd_edpt_xfer(rhport, iface->ep_in, iface->epin_buf, len);
}

void tud_jumperless_init(void) {
  tud_jumperless_reset(0);
}

void tud_jumperless_reset(uint8_t rhport) {
  
}

uint16_t tud_jumperless_open(uint8_t rhport,
                             tusb_desc_interface_t const *desc_itf,
                             uint16_t max_len) {
  TU_VERIFY(0xFF == desc_itf->bInterfaceClass, 0);

  uint16_t const drv_len = (uint16_t) sizeof(tusb_desc_interface_t) + 2 * sizeof(tusb_desc_endpoint_t);

  TU_ASSERT(max_len >= drv_len, 0);

  jl_interface_t *iface = &_jl_itf[0];

  uint8_t const *p_desc = (uint8_t const*) desc_itf;

  p_desc = tu_desc_next(p_desc);

  TU_ASSERT(usbd_open_edpt_pair(rhport, p_desc, 2, TUSB_XFER_INTERRUPT, &iface->ep_out, &iface->ep_in), 0);

  iface->itf_num = desc_itf->bInterfaceNumber;

  return drv_len;
}

bool tud_jumperless_control_xfer_cb(uint8_t rhport, uint8_t stage,
                                    tusb_control_request_t const *request) {
  return false;
}

bool tud_jumperless_xfer_cb(uint8_t rhport, uint8_t ep_addr,
                            xfer_result_t event, uint32_t xferred_bytes) {
  return false;
}

#endif
