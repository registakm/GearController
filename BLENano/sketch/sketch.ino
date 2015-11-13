
/* 
  [BLENano] Gearコントローラ制御プログラム
  @Hikaru
*/
#define TXRX_BUF_LEN                      20

BLE                                       ble;
Timeout                                   timeout;                

static uint8_t rx_buf[TXRX_BUF_LEN];
static uint8_t rx_buf_num;
static uint8_t rx_state=0;

static uint8_t sensorValue;

// The Nordic UART Service
static const uint8_t service1_uuid[]                = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_tx_uuid[]             = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_rx_uuid[]             = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[]           = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};

uint8_t tx_value[TXRX_BUF_LEN] = {0,};
uint8_t rx_value[TXRX_BUF_LEN] = {0,};

GattCharacteristic  characteristic1(service1_tx_uuid, tx_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );

GattCharacteristic  characteristic2(service1_rx_uuid, rx_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

GattCharacteristic *uartChars[] = {&characteristic1, &characteristic2};

GattService         uartService(service1_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));

/* 変数定義 */
int loopPlus = 0;

/* 接続が切れた時の処理 */
static void disconnectionCallBack(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    Serial1.println("Disconnected!");
    Serial1.println("Restarting the advertising process");
    ble.startAdvertising();
}

/* Centralから書き込みがあったときの処理 */
void writtenHandle(const GattWriteCallbackParams *Handler)
{
    Serial1.println("onDataWritten : ");
}

/* BLE送信メソッド */
void updateChar(uint8_t prefix) {
//  String value = String(prefix);
  ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), (const uint8_t *)prefix, sizeof(prefix));
}


void setup() {
  
    Serial1.begin(9600);
    // Serial1.attach(uart_handle);

    ble.init();
    ble.onDisconnection(disconnectionCallBack);
    ble.onDataWritten(writtenHandle);
      
    // BLEPayloadデータセッティング
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                     (const uint8_t *)"HIKARU_BLENANO", sizeof("HIKARU_BLENANO"));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                     (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid_rev));
                
    // set adv_type
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);    
    // add service
    ble.addService(uartService);
    // set device name
    ble.setDeviceName((const uint8_t *)"HIKARU_BLENANO");
    // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
    ble.setTxPower(4);
    // set adv_interval, 100ms in multiples of 0.625ms.
    ble.setAdvertisingInterval(160);
    // set adv_timeout, in seconds
    ble.setAdvertisingTimeout(0);
    // start advertising
    ble.startAdvertising();

    Serial1.println("Advertising Start!");
    while(1) {
      sensorValue = analogRead(A4);
      updateChar(sensorValue);
    }
}

void loop() {
    ble.waitForEvent();
}
