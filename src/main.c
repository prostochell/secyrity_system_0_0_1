#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "ssd1306.h"
#include "nvs_flash.h"
#include "nvs.h"

// GPIO pins for the PIR sensor, LEDs, keypad, and I2C OLED
#define PIR_SENSOR_PIN GPIO_NUM_14
#define BLUE_LED_PIN   GPIO_NUM_20
#define RED_LED_PIN    GPIO_NUM_21  
#define BUZZER_PIN     GPIO_NUM_10
#define KEYPAD_ROW_1   GPIO_NUM_12
#define KEYPAD_ROW_2   GPIO_NUM_11
#define KEYPAD_ROW_3   GPIO_NUM_7
#define KEYPAD_COL_1   GPIO_NUM_6
#define KEYPAD_COL_2   GPIO_NUM_3
#define KEYPAD_COL_3   GPIO_NUM_4
#define KEYPAD_COL_4   GPIO_NUM_5

#define LED_BLINK_DELAY 500

#define I2C_SDA_PIN GPIO_NUM_6
#define I2C_SCL_PIN GPIO_NUM_7

#define EEPROM_NAMESPACE "storage"
#define EEPROM_KEY "password"

#define CONFIG_OFFSETX 0  // Set this value depending on your display's requirements


char stored_password[16]; // Stores the password retrieved from EEPROM

uint8_t relab_logo[] = {
// 'relab_wht_1_128_32', 128x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0x38, 0x78, 0x70, 0x80, 0x80, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 
0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0xf8, 0xc0, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xc0, 0xc0, 0x80, 0x70, 
0x78, 0x78, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xf0, 0xf0, 0x10, 0x10, 0xf0, 0xe0, 0x80, 0xc0, 0x40, 0x40, 0xc0, 0x00, 0xc0, 0x80, 0x00, 0xc0, 
0x80, 0x00, 0xc0, 0x40, 0x80, 0xc0, 0x40, 0x40, 0xc0, 0x80, 0x00, 0xc0, 0xc0, 0x40, 0x80, 0xc0, 
0x40, 0x40, 0xc0, 0x80, 0x80, 0xc0, 0x40, 0x40, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 
0x40, 0x40, 0xc0, 0x80, 0xc0, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xcf, 0xff, 0xff, 0x3f, 0x3f, 0x9f, 0x9f, 0x3f, 0x3f, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xbf, 0x9f, 0xbf, 0x3f, 0xff, 0xff, 0xff, 0x87, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x0f, 0x0f, 0x01, 0x01, 0x01, 0x00, 0x07, 0x0f, 0x08, 0x08, 0x0f, 0x03, 0x01, 0x0f, 0x0f, 0x01, 
0x07, 0x0e, 0x03, 0x00, 0x07, 0x0f, 0x09, 0x09, 0x0d, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x07, 0x0f, 
0x09, 0x09, 0x0d, 0x05, 0x07, 0x0f, 0x08, 0x0c, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 
0x08, 0x08, 0x0f, 0x03, 0x21, 0x3f, 0x1e, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3f, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0xfc, 0xf0, 0x73, 0x67, 0xe7, 0xf3, 0xf0, 0xfc, 
0x7f, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x7c, 0x70, 0xf3, 0x67, 0x73, 0x70, 0xfc, 0xff, 0xff, 0xff, 
0x7f, 0x7f, 0x7f, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x04, 0x0c, 0x3f, 0x22, 0x08, 0x3f, 0x25, 0x24, 0x24, 
0x00, 0x3f, 0x3f, 0x20, 0x20, 0x20, 0x38, 0x0f, 0x0b, 0x0f, 0x38, 0x20, 0x3f, 0x3d, 0x24, 0x27, 
0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};




// Modes and states
typedef enum {
    MODE_IDLE,
    MODE_SET_PASSWORD,
    MODE_RESET_PASSWORD,
    MODE_PROTECTION_ON,
    MODE_UNLOCK
} system_mode_t;

system_mode_t current_mode = MODE_IDLE;
char input_password[5];
int input_index = 0;
bool is_protected = false;
bool motion_detected = false;

SSD1306_t dev;

// Function to initialize the OLED
void oled_init() {
    i2c_master_init(&dev, I2C_SDA_PIN, I2C_SCL_PIN, -1);
    ssd1306_init(&dev, 128, 32);
    ssd1306_clear_screen(&dev, false); // Clear screen on initialization
}



// Function to update OLED display with mode information
void oled_update_display() {
    ssd1306_clear_screen(&dev, false);

    switch (current_mode) {
        case MODE_IDLE:
            ssd1306_display_text(&dev, 0, "Idle Mode", 9, false);
            ssd1306_display_text(&dev, 1, "Press 9 to Protect", 18, false);
            break;
        case MODE_SET_PASSWORD:
            ssd1306_display_text(&dev, 0, "Set Password", 12, false);
            // Show the password as asterisks
            char asterisk_password[5];
            memset(asterisk_password, '*', input_index);  // Fill with asterisks up to the number of input characters
            asterisk_password[input_index] = '\0';  // Null-terminate the string
            ssd1306_display_text(&dev, 1, asterisk_password, strlen(asterisk_password), false);
            break;
        case MODE_RESET_PASSWORD:
            ssd1306_display_text(&dev, 0, "Reset Password", 13, false);
            // Show the password as asterisks
            memset(asterisk_password, '*', input_index);
            asterisk_password[input_index] = '\0';
            ssd1306_display_text(&dev, 1, asterisk_password, strlen(asterisk_password), false);
            break;
        case MODE_PROTECTION_ON:
            ssd1306_display_text(&dev, 0, "Protection ON", 13, false);
            ssd1306_display_text(&dev, 1, "Motion: Waiting", 16, false);
            break;
        case MODE_UNLOCK:
            ssd1306_display_text(&dev, 0, "Unlocking...", 11, false);
            break;
    }
}


void display_logo() {
    ssd1306_clear_screen(&dev, false);
    // Display the logo starting from page 0, segment 0
    int k=128;
    for(int i=0; i<4; i++){
        
        ssd1306_display_image(&dev, i, 0, relab_logo, k);  // Assuming logo width is 128 pixels
    k+=128;
    }
    vTaskDelay(pdMS_TO_TICKS(15000));  // Show for 5 seconds
    ssd1306_clear_screen(&dev, false);
}

// Function to scan the keypad
char scan_keypad() {
    const int row_pins[3] = {KEYPAD_ROW_1, KEYPAD_ROW_2, KEYPAD_ROW_3};
    const int col_pins[4] = {KEYPAD_COL_1, KEYPAD_COL_2, KEYPAD_COL_3, KEYPAD_COL_4}; 
    const char keypad[4][3] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'*', '0', '#'}
    };

    for (int row = 0; row < 3; row++) {
        gpio_set_level(row_pins[row], 0);

        for (int col = 0; col < 4; col++) {
            if (gpio_get_level(col_pins[col]) == 0) {
                vTaskDelay(pdMS_TO_TICKS(10));  
                return keypad[col][row]; 
            }
        }
        gpio_set_level(row_pins[row], 1);
    }

    return '\0';  // No pressed key
}


// Function to handle password input
void handle_password_input(char key) {
    if (input_index < 4) {
        input_password[input_index++] = key;
    }

    if (input_index == 4) {
        input_password[4] = '\0';  // Null-terminate the password
        if (strcmp(input_password, stored_password) == 0) {
            is_protected = false;
            current_mode = MODE_IDLE;
        }
        input_index = 0;
    }
    oled_update_display();
}

// Function to handle setting or resetting the password
void handle_password_setting(char key) {
    if (input_index < 4) {
        input_password[input_index++] = key;
    }

    if (input_index == 4) {
        input_password[4] = '\0';  // Null-terminate the new password
        strcpy(stored_password, input_password);
        input_index = 0;
        current_mode = MODE_IDLE;
    }
    oled_update_display();
}

// Function to initialize NVS
void init_nvs() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

// Function to save the password to EEPROM
void save_password_to_eeprom(const char *password) {
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(EEPROM_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        // Write the password
        err = nvs_set_str(my_handle, EEPROM_KEY, password);
        if (err == ESP_OK) {
            nvs_commit(my_handle);
            printf("Password saved to EEPROM: %s\n", password);
        }
        nvs_close(my_handle);
    } else {
        printf("Error opening NVS handle!\n");
    }
}

// Function to read the password from EEPROM
void read_password_from_eeprom() {
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(EEPROM_NAMESPACE, NVS_READONLY, &my_handle);
    if (err == ESP_OK) {
        // Read the password
        size_t required_size;
        err = nvs_get_str(my_handle, EEPROM_KEY, NULL, &required_size);
        if (err == ESP_OK) {
            nvs_get_str(my_handle, EEPROM_KEY, stored_password, &required_size);
            printf("Password read from EEPROM: %s\n", stored_password);
        } else {
            printf("No password found in EEPROM, setting default password.\n");
            strcpy(stored_password, "1234"); // Default password
        }
        nvs_close(my_handle);
    } else {
        printf("Error opening NVS handle!\n");
    }
}

// Function to handle PIR sensor and LED&Buzz control
void pir_task(void *pvParameter) {
    gpio_set_direction(PIR_SENSOR_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(BLUE_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(RED_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);  

    while (1) {
        int pir_state = gpio_get_level(PIR_SENSOR_PIN);

        if (pir_state == 1 && is_protected) { 
            motion_detected = true;
            current_mode = MODE_UNLOCK;
            oled_update_display();

           
            int blink_count = 0;
            while (blink_count < 80) {  
                gpio_set_level(BLUE_LED_PIN, blink_count % 2);
                gpio_set_level(RED_LED_PIN, blink_count % 2);
                gpio_set_level(BUZZER_PIN, 1); 

                vTaskDelay(pdMS_TO_TICKS(LED_BLINK_DELAY / 2));
                gpio_set_level(BUZZER_PIN, 0); 

                vTaskDelay(pdMS_TO_TICKS(LED_BLINK_DELAY / 2)); 
                blink_count++;
            }

            if (is_protected) {
                current_mode = MODE_IDLE;
                oled_update_display();
                gpio_set_level(BLUE_LED_PIN, 0);
                gpio_set_level(RED_LED_PIN, 0);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Function to handle keypad input and mode switching
void keypad_task(void *pvParameter) {
    while (1) {
        char key = scan_keypad();

        if (key != '\0') {
            switch (key) {
                case '9':  // Switch ON protection
                    current_mode = MODE_PROTECTION_ON;
                    is_protected = true;
                    break;
                case '0':  // Switch OFF protection
                    current_mode = MODE_IDLE;
                    is_protected = false;
                    break;
                case '*':  // Set password mode
                    current_mode = MODE_SET_PASSWORD;
                    input_index = 0;
                    break;
                case '#':  // Reset password mode
                    current_mode = MODE_RESET_PASSWORD;
                    input_index = 0;
                    break;
                default:
                    if (current_mode == MODE_PROTECTION_ON && motion_detected) {
                        handle_password_input(key);
                    } else if (current_mode == MODE_SET_PASSWORD || current_mode == MODE_RESET_PASSWORD) {
                        handle_password_setting(key);
                    }
                    break;
            }
        }

        oled_update_display();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void app_main() {
    // Initialize NVS
    init_nvs();


    // Configure OLED
    oled_init();    
    // Display the logo on startup
    display_logo();

    oled_update_display();




    read_password_from_eeprom();



    // Configure keypad GPIOs
    gpio_set_direction(KEYPAD_ROW_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(KEYPAD_ROW_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(KEYPAD_ROW_3, GPIO_MODE_OUTPUT);
    gpio_set_direction(KEYPAD_COL_1, GPIO_MODE_INPUT);
    gpio_set_direction(KEYPAD_COL_2, GPIO_MODE_INPUT);
    gpio_set_direction(KEYPAD_COL_3, GPIO_MODE_INPUT);
    gpio_set_direction(KEYPAD_COL_4, GPIO_MODE_INPUT);

    gpio_set_level(KEYPAD_ROW_1, 1);
    gpio_set_level(KEYPAD_ROW_2, 1);
    gpio_set_level(KEYPAD_ROW_3, 1);

    // Create the PIR sensor and keypad tasks
    xTaskCreate(pir_task, "pir_task", 2048, NULL, 5, NULL);
    xTaskCreate(keypad_task, "keypad_task", 2048, NULL, 5, NULL);
}
