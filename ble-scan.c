#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


int main(int argc, const char* argv[])
{
  int hciDeviceId = 1;
  int hciSocket;
  struct hci_dev_info hciDevInfo;

  struct hci_filter oldHciFilter;
  struct hci_filter newHciFilter;
  socklen_t oldHciFilterLen;



  unsigned char hciEventBuf[HCI_MAX_EVENT_SIZE];
  int hciEventLen;
  evt_le_meta_event *leMetaEvent;
  le_advertising_info *leAdvertisingInfo;
  char btAddress[18];
  int8_t rssi;

  memset(&hciDevInfo, 0x00, sizeof(hciDevInfo));

  setlinebuf(stdout);

  hciDeviceId = (argc > 1) ? atoi(argv[1]) : hci_get_route(NULL);

  if (hciDeviceId < 0) {
    hciDeviceId = 0; // use device 0, if device id is invalid
  }

  // setup HCI socket
  hciSocket = hci_open_dev(hciDeviceId);

  if (hciSocket == -1) {
    printf("adapterState unsupported\n");
    return -1;
  }
  hciDevInfo.dev_id = hciDeviceId;

  // get old HCI filter
  oldHciFilterLen = sizeof(oldHciFilter);
  getsockopt(hciSocket, SOL_HCI, HCI_FILTER, &oldHciFilter, &oldHciFilterLen);

  // setup new HCI filter
  hci_filter_clear(&newHciFilter);
  hci_filter_set_ptype(HCI_EVENT_PKT, &newHciFilter);
  hci_filter_set_event(EVT_LE_META_EVENT, &newHciFilter);
  setsockopt(hciSocket, SOL_HCI, HCI_FILTER, &newHciFilter, sizeof(newHciFilter));

  //  hci_le_set_scan_parameters(hciSocket, 0x01, htobs(0x0010), htobs(0x0010), 0x00, 0, 1000);
  hci_le_set_scan_parameters(hciSocket, 0x00, htobs(0x0010), htobs(0x0010), 0x00, 0, 1000);
  //  hci_le_set_scan_enable(hciSocket, 0x00, 0, 1000);
  hci_le_set_scan_enable(hciSocket, 0x01, 0, 1000);

  while(1) {
    // read event
    hciEventLen = read(hciSocket, hciEventBuf, sizeof(hciEventBuf));
    leMetaEvent = (evt_le_meta_event *)(hciEventBuf + (1 + HCI_EVENT_HDR_SIZE));
    hciEventLen -= (1 + HCI_EVENT_HDR_SIZE);
    
    if (leMetaEvent->subevent != EVT_LE_ADVERTISING_REPORT) continue;
    
    leAdvertisingInfo = (le_advertising_info *)(leMetaEvent->data + 1);
    ba2str(&leAdvertisingInfo->bdaddr, btAddress);
    rssi = *(leAdvertisingInfo->data + leAdvertisingInfo->length);
    
    printf("%s %d %ld\n",  btAddress, rssi, time(0)); fflush(stdout);
    
  }

  // restore original filter
  setsockopt(hciSocket, SOL_HCI, HCI_FILTER, &oldHciFilter, sizeof(oldHciFilter));

  // disable LE scan
  hci_le_set_scan_enable(hciSocket, 0x00, 0, 1000);

  close(hciSocket);

  return 0;
}
