#include <stdio.h>
#include <string.h>
#include "esp_now.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_flash.h"
#include "esp_netif.h"
#include "nvs_flash.h"

static const char *TAG = "RECEIVER";

/* Broadcast address */
uint8_t broadcast_mac[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

typedef struct {
    char type[10];
} pair_msg_t;

typedef struct {
    uint16_t seq;
    float temperature;
    float voltage;
} data_packet_t;

static bool paired = false; 
static uint8_t peer_mac[6];
static uint16_t sequence = 0;

void send_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
{
    if (status == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGI(TAG, "Delivery Success");
    } else {
        ESP_LOGW(TAG, "Delivery Failed");
    }
}

void send_task(void *pv)
{
    data_packet_t packet;

    while (1) {

        packet.seq = sequence++;
        packet.temperature = 30.0 + sequence;
        packet.voltage = 11.5;

        esp_now_send(peer_mac,
                     (uint8_t *)&packet,
                     sizeof(packet));

        ESP_LOGI(TAG,
                 "Sent -> Seq:%d Temp:%.2f Volt:%.2f",
                 packet.seq,
                 packet.temperature,
                 packet.voltage);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* ===== RECEIVE CALLBACK (IDF v6 style) ===== */
void recv_cb(const esp_now_recv_info_t *info,
             const uint8_t *data,
             int len)
{
    /* ===== Pair Message ===== */
    if (len == sizeof(pair_msg_t)) {

        pair_msg_t msg;
        memcpy(&msg, data, sizeof(msg));

        if (strcmp(msg.type, "ACK") == 0 && !paired ) {

            ESP_LOGI(TAG, "ACK received! Paired successfully.");

            esp_now_peer_info_t peer = {0};
            memcpy(peer.peer_addr, info->src_addr, 6);
            memcpy(peer_mac, info->src_addr, 6);
	    peer.channel = 0;
            peer.encrypt = false;

            if (!esp_now_is_peer_exist(info->src_addr)) {
                if (esp_now_add_peer(&peer) == ESP_OK) {
                    ESP_LOGI(TAG, "Sender added as peer.");
                }
            }

            paired = true;
	    
	    xTaskCreate(send_task, "send_task", 4096, NULL, 4, NULL);
        }
    }

    /* ===== Data Packet Recived   ===== */
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
/* ===== MAIN ===== */
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

    /* Add broadcast peer */
    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, broadcast_mac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    pair_msg_t msg;
    strcpy(msg.type, "PAIR");

    while (!paired) {
        ESP_LOGI(TAG, "Sending Pair Request...");
        esp_now_send(broadcast_mac, (uint8_t *)&msg, sizeof(msg));
        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    /* Keep system alive after pairing */
 //   while (1) {
 //   vTaskDelay(pdMS_TO_TICKS(1000));
//}
}
