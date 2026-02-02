#include "Utils.h"

String format_string(const char* format, va_list args) {
    if (format == NULL)
        return "";

    String result = "";
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 'd': // INT
                result += va_arg(args, int);
                break;
            case 'f': // FLOAT OR DOUBLE
                result += va_arg(args, double);
                break;
            case 'c': // CHAR
                result += (char)va_arg(args, int);
                break;
            case 's': // CHAR* (STRING)
                result += va_arg(args, char*);
                break;
            case 'u': // UNSIGNED INT
                result += va_arg(args, unsigned int);
                break;
            default:
                result += '%';
                if (format[i] != '\0')
                    result += format[i];
                break;
            }
        } else {
            result += format[i];
        }
    }
    return result;
}

void print_with_args(const char* format, ...) {
    va_list args;
    va_start(args, format);
    String result = format_string(format, args);
    va_end(args);
    Serial.print(result);
}

void println_with_args(const char* format, ...) {
    va_list args;
    va_start(args, format);
    String result = format_string(format, args);
    va_end(args);
    Serial.println(result);
}

// finds the max and min values in any static 2D array of floats, min and max must be initalized to sensible values beforehand
template<size_t rows, size_t cols> inline void min_max(const float arr_2D[rows][cols], float &min, float &max) {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (arr_2D[i][j] < min)
                min = arr_2D[i][j];

            if (arr_2D[i][j] > max)
                max = arr_2D[i][j];
        }
    }
}

// searches a sorted DECREASING list for the nearest element and returns its index, the "lower" flag if set will return the nearest element that is equal or lower
// online function testbench: https://www.programiz.com/online-compiler/5fkt3FMi4yJhY
template<size_t length> inline int search(const float arr[length], const float value, bool return_lower) {
if (value >= arr[0]) 
        return (0);

    if (value <= arr[length - 1])
        return (length - 1);

    int low = 0;
    int high = length - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == value)
            return mid;

        if (arr[mid] > value)
            low = mid + 1;
        else
            high = mid - 1;
    }

    if (return_lower || std::abs(arr[low] - value) < std::abs(arr[high] - value))
        return low; // arr[low] is closer
    else
        return high; // arr[high] is closer
}

// linearly interpolate an x-value from sorted DECREASING arrays: Y = Y1 + (Y2-Y1) / (X2-X1) * (X-X1)
template<size_t length> inline float interpolate(const float arr_x[length], const float arr_y[length], const float x_value) {
    int x1_index = 0; 
    int x2_index = 0;

    // out of range cases just return the y-bound of the array
    if (x_value >= arr_x[0]) 
        return arr_y[0];

    if (x_value <= arr_x[length - 1])
        return arr_y[length - 1];

    x1_index = search<length>(arr_x, x_value, true);
    if (x1_index == 0) // this condition should not be true if the search function works and the edge cases are handled properly
        return arr_y[0];

    x2_index = x1_index - 1; // decreasing arrays so x2_index is less than x1_index
    float y_value = arr_y[x1_index] + (arr_y[x2_index] - arr_y[x1_index]) / (arr_x[x2_index] - arr_x[x1_index]) * (x_value - arr_x[x1_index]);
    return (y_value);
}

uint8_t power_limit(const float max_cell_temp) {
    if (max_cell_temp <= FULL_POWER_TEMP_C)
        return FULL_POWER_LEVEL_KW;
    else
        return 4;
    
    // else {
    //   float slope = -1.0 / (ZERO_POWER_TEMP_C - FULL_POWER_TEMP_C);
    //   return float_2_uint8_t(FULL_POWER_LEVEL_KW * (slope * (max_cell_temp -
    //   FULL_POWER_TEMP_C) + 1.0), 0, 80);
    // }
}

uint8_t float_2_uint8_t(const float float_val, const float min, const float max) { // float to uint8_t, clips values under/over min or max
    if (max == min) return (0); // divide by zero
    if (float_val >= max) return max; // overflow
    if (float_val <= min) return min; // underflow

    uint8_t scaled = (uint8_t)(((float_val - min) / (max - min)) * 255.0); 
    // float decoded = ((float)scaled / 255.0) * (max - min) + min;
    return (scaled);
}

float map_voltage_to_temp(float V) { // voltage -> actual temp
    int const size = sizeof(NTC_LUT) / sizeof(NTC_LUT[0]);
    float R_bias = 10000;
    float V_ref = 3.00;
    
    if (V_ref == V) // divide by zero case
    return -55;
    
    float NTC_res = (V / V_ref * R_bias) / (1 - V / V_ref);
    int i = search<size>(NTC_LUT, NTC_res);
    float temperature = float(i) / float(size) * (150 + 55) - 55;
    return (temperature);
}

void map_text2var(Ev4_t *ctx, String name, String value) { // map text name and value to a variable
    if (name == "SOC:") {
        ctx->soc = value.toFloat();
        Serial.println(ctx->soc);
    }
}

void send_command(Ev4_t *ctx, uint16_t command) {
    uint8_t comm_arr[2];
    uint16_t pec;
    uint8_t pec0;
    uint8_t pec1;
    uint8_t cmd0;
    uint8_t cmd1;

    cmd0 = command >> 8; // top 8 bits
    cmd1 = command >> 0; // lower 8 bits

    if (millis() - ctx->sense_watchdog_timer >= 1800) {
        Wakeup_Sleep(num_boards + 1);
        ctx->sense_watchdog_timer = millis();
    } else {
        Wakeup_Idle(num_boards);
        ctx->sense_watchdog_timer = millis();
    }

    digitalWrite(CS, LOW);

    comm_arr[0] = cmd0;
    comm_arr[1] = cmd1;

    pec = pec15_calc(2, comm_arr);
    pec0 = pec >> 8;
    pec1 = pec >> 0;

    SPI.transfer(cmd0);
    SPI.transfer(cmd1);
    SPI.transfer(pec0);
    SPI.transfer(pec1);
}

void read_register_group(Ev4_t *ctx, uint16_t command, uint8_t response[num_boards][6]) { // register group is always 6 bytes
    uint8_t ccmd; // command counter
    uint16_t rx_pec10; // Recieved and parsed 10 bit data PEC
    uint16_t calc_pec10; // Calculated 10 bit data PEC
    uint8_t response_pec0;
    uint8_t response_pec1;

    send_command(ctx, command);

    for (int i = 0; i < num_boards; i++) {
        for (int j = 0; j < 6; j++) {
            response[i][j] = SPI.transfer(FULL_REG); // Send dummy byte to receive data
            // Serial.println(response[i][j], BIN);
        }

        response_pec0 = SPI.transfer(0xFF); // response PEC = command counter + PEC, needs to parsed
        response_pec1 = SPI.transfer(0xFF);
        
        // Extract command counter and received 10-bit PEC from the ADBMS6830B readback format:
        // PEC0 = [CCNT5..0 | PEC9..8], PEC1 = [PEC7..0]
        ccmd = (response_pec0 >> 2) & 0x3F;
        rx_pec10 = ((uint16_t)(response_pec0 & 0x03) << 8) | response_pec1;

        calc_pec10 = pec10_calc_data_ccnt(response[i], ccmd);

        if (rx_pec10 != (calc_pec10 & 0x3FF)) {
            Serial.println("PEC Error - Data PEC Mismatch");
            Wakeup_Sleep(num_boards + 1);
        }
    }

    // Debug code
    // Serial.print("response pec ");
    // Serial.println(rx_pec10, HEX);
    // Serial.print("calculated pec ");
    // Serial.println(calc_pec10, HEX);

    digitalWrite(CS, HIGH);
}

void write_register_group(Ev4_t *ctx, uint16_t command, uint8_t data[num_boards][6]) {
    uint8_t data_pec0;
    uint8_t data_pec1;
    uint16_t data_pec;

    send_command(ctx, command);

    for (int i = num_boards - 1; i >= 0; i--) {
        data_pec = pec15_calc(6, data[i]);
        data_pec0 = data_pec >> 8;
        data_pec1 = data_pec >> 0;

        for (int j = 0; j < 6; j++)
            SPI.transfer(data[i][j]);

        SPI.transfer(data_pec0);
        SPI.transfer(data_pec1);
    }
    digitalWrite(CS, HIGH);
}

void print_min_max(Ev4_t *ctx) { // This function prints the min and max parameters
    float min_cell_voltage = ctx->cell_voltage[0][0];
    float max_cell_voltage = ctx->cell_voltage[0][0];
    float min_cell_temp = ctx->cell_temp[0][0];
    float max_cell_temp = ctx->cell_temp[0][0];
    float min_die_temp = ctx->die_temps[0];
    float max_die_temp = ctx->die_temps[0];

    min_max<num_boards, num_cells>(ctx->cell_voltage, min_cell_voltage, max_cell_voltage);
    min_max<num_boards, 10>(ctx->cell_temp, min_cell_temp, max_cell_temp);
    min_max<1, num_boards>(&(ctx->die_temps), min_die_temp, max_die_temp); // This is how you pass a 1D array to the min_max function

    println_with_args("Max cell voltage: %f", max_cell_voltage);
    println_with_args("Min cell voltage: %f", min_cell_voltage);
    println_with_args("Max cell temp: %f", max_cell_temp);
    println_with_args("Min cell temp: %f", min_cell_temp);
    println_with_args("Max die temp: %f", max_die_temp);
    println_with_args("Min die temp: %f", min_die_temp);
}

// Determine starting mode (return true to break outer loop)
bool determineMode(Ev4_t *ctx, CAN_message_t msg, bool CAN_baud_alt) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (msg.id == INV_TX_ID && false) { // Always check msg id. Stdby has not yet been tested
        ctx->mode = Mode::Standby;
        ctx->can.setBaudRate(500000);
        // can.setMBFilter(MB1, 0);  // Disable Charger Mailbox
        return true;
    } else if (msg.id == CHG_TX_ID) {
        ctx->mode = Mode::Charge;
        ctx->can.setBaudRate(250000);
        // can.setMBFilter(MB0, 0); // Disable Inverter Mailbox
        return true;
    } else if (ctx->current >= 0.5) { // enter directly into drive mode if current is detected
        ctx->can.setBaudRate(500000);
        ctx->mode = Mode::Drive;
        return true;
    } else if (input == "balance") {
        ctx->mode = Mode::Balance;
        return true;
    } else if (input == "debug") {
        ctx->mode = Mode::Debug;
        return true;
    }

    return false;
}

void configure_sense(Ev4_t *ctx) {
    uint8_t data[6];
    uint8_t data_arr[num_boards][6]; // contains identical copies of data for each board
    uint16_t VUV = (UV - 1.5f) / (16 * 0.00015f); // Cell undervoltage threshold = VUV * 16 * 150μV + 1.5V
    uint16_t VOV = (OV - 1.5f) / (16 * 0.00015f); // Cell overvoltage threshold = VOV * 16 * 150μV + 1.5V
    Serial.println(VUV, BIN);
    Serial.println(VOV, BIN);

    data[0] = 0b11111100; // GPIO1-5 = 1 (pull-down off), REFON=1, DTEN=0, ADCOPT=0
    data[1] = (uint8_t)VUV;
    data[2] = (uint8_t)(VOV & 0b11110000) | (VUV >> 8 & 0b00001111);
    data[3] = (uint8_t)VOV >> 4;
    data[4] = CLEAR_REG;
    data[5] = CLEAR_REG;

    // data[0] = 0b11111110; // GPIO1-5 = 1 (pull-down off), REFON=1, DTEN=0, ADCOPT=0 
    // data[1] = (uint8_t) VUV; 
    // data[2] = (uint8_t) (VOV & 0b11110000) | (VUV>>8 & 0b00001111); 
    // data[3] = (uint8_t) VOV>>4; 
    // data[4] = 0b11111111;
    // data[5] = 0b11111111;

    for (int i = 0; i < num_boards; i++)
        std::copy(data, data + 6, data_arr[i]);

    write_register_group(ctx, WRCFGA, data_arr);
}