#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "nvs_flash.h"

static const char *TAG = "ESPNOW_SENDER";

typedef struct {
    char type[10];
}pair_msg_t;

typedef struct {
    uint16_t seq;
    float temperature;
    float voltage;
} data_packet_t;

static uint16_t sequence = 0;
static uint8_t peer_mac[6];
static bool paired = false;

/* ================= SEND CALLBACK ================= */
void send_cb(const esp_now_send_info_t *tx_info,esp_now_send_status_t status)
{
    ESP_LOGI(TAG, "Delivery: %s",
             status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void send_data_task( void *pv) 
{
    data_packet_t packet;

    while(1)
    {
	packet.seq = sequence++;
	packet.temperature = 25.5 + ( sequence * 0.1);
	packet.voltage = 12.0;
       
	esp_err_t result = esp_now_send(peer_mac,(uint8_t *)&packet,sizeof(packet));

        if (result == ESP_OK) {
            ESP_LOGI("SENDER", "Sent -> Seq:%d Temp:%.2f Volt:%.2f",
                     packet.seq, packet.temperature, packet.voltage);
        } else {
            ESP_LOGW("SENDER", "Send Error");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* ====== ESP-NOW RECEIVE CALLBACK (IDF v6) ====== */
void recv_cb(const esp_now_recv_info_t *info,
             const uint8_t *data,
             int len)
{
    /* ===== Pair Request ===== */
    if (len == sizeof(pair_msg_t)) {

        pair_msg_t msg;
        memcpy(&msg, data, sizeof(msg));

        if (strcmp(msg.type, "PAIR") == 0  && !paired) {

            ESP_LOGI(TAG, "Pair request received !!");

            esp_now_peer_info_t peer = {0};
            memcpy(peer.peer_addr, info->src_addr, 6);
            memcpy(peer_mac, info->src_addr, 6);
            peer.channel = 0;
            peer.encrypt = false;

            if (!esp_now_is_peer_exist(info->src_addr)) {
                if (esp_now_add_peer(&peer) == ESP_OK) {
                    ESP_LOGI(TAG, "Peer Added Automatically!!!");
                }
            }

            memcpy(peer_mac, info->src_addr, 6);

            strcpy(msg.type, "ACK");
            esp_now_send(info->src_addr,
                         (uint8_t *)&msg,
                         sizeof(msg));

            paired = true;
             /* Start sending data */
            xTaskCreate(send_data_task,
                        "send_data_task",
                        4096,
                        NULL,
                        4,
                        NULL);
        }
    }

    /* ===== Data Packet ===== */
    else if (len == sizeof(data_packet_t)) {

        data_packet_t packet;
        memcpy(&packet, data, sizeof(packet));

        ESP_LOGI(TAG,
                 "Received -> Seq:%d Temp:%.2f Volt:%.2f",
                 packet.seq,
                 packet.temperature,
                 packet.voltage);
    }
}
void app_main(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    esp_now_init();
    esp_now_register_recv_cb(recv_cb);
    esp_now_register_send_cb(send_cb);
    /* wait unit paired */
  //  while(!paired) {
    //    ESP_LOGI(TAG, "Waiting for pairing...");
//	vTaskDelay( pdMS_TO_TICKS(500));
  //  }

    //xTaskCreate(send_data_task, "send_data_task", 4096, NULL, 4, NULL);
    
    while (1)
    {
	vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
