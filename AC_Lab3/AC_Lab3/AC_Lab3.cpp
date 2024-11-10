#include <iostream>
#include <vector>
#include <random>
#include <chrono>

uint8_t crc_slow(const std::vector<int>& message, uint8_t polynomial)
{
    uint8_t remainder = 0;
    for (uint8_t byte : message)
    {
        remainder ^= byte;
        for (int i = 0; i < 8; ++i)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1) ^ polynomial;
            }
            else {
                remainder <<= 1;
            }
        }
    }
    return remainder;
}

std::vector<uint8_t> crc_table_init(uint8_t polynomial)
{
    std::vector<uint8_t> crc_table(256);
    for (int i = 0; i < 256; ++i)
    {
        uint8_t remainder = i;
        for (int j = 0; j < 8; ++j)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1) ^ polynomial;
            }
            else
            {
                remainder <<= 1;
            }
        }
        crc_table[i] = remainder;
    }
    return crc_table;
}

uint8_t crc_table(const std::vector<int>& message, const std::vector<uint8_t>& table)
{
    uint8_t crc = 0;
    for (uint8_t byte : message)
    {
        crc = table[crc ^ byte];
    }
    return crc;
}

uint8_t crc_reflect_slow(const std::vector<int>& message, uint8_t polynomial)
{
    uint8_t remainder = 0;
    for (auto it = message.rbegin(); it != message.rend(); ++it)
    {
        remainder ^= *it;
        for (int i = 0; i < 8; ++i)
        {
            if (remainder & 1)
            {
                remainder = (remainder >> 1) ^ polynomial;
            }
            else
            {
                remainder >>= 1;
            }
        }
    }
    return remainder;
}

std::vector<uint8_t> crc_reflect_table_init(uint8_t polynomial)
{
    std::vector<uint8_t> crc_table(256);
    for (int i = 0; i < 256; ++i)
    {
        uint8_t remainder = i;
        for (int j = 0; j < 8; ++j)
        {
            if (remainder & 1)
            {
                remainder = (remainder >> 1) ^ polynomial;
            }
            else
            {
                remainder >>= 1;
            }
        }
        crc_table[i] = remainder;
    }
    return crc_table;
}

uint8_t crc_reflect_table(const std::vector<int>& message, const std::vector<uint8_t>& table)
{
    uint8_t crc = 0;
    for (auto it = message.rbegin(); it != message.rend(); ++it)
    {
        crc ^= *it;
        crc = table[crc];
    }
    return crc;
}

std::vector<int> generate_random_message(int length)
{
    std::vector<int> message(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int& byte : message)
    {
        byte = dis(gen);
    }
    return message;
}

int main()
{
    uint8_t polynomial = 0x91;
    auto table = crc_table_init(polynomial);
    auto reflect_table = crc_reflect_table_init(polynomial);

    const int iterations = 1000000;
    const int message_length = 32; // length of each random message

    std::chrono::duration<double> crc_slow_time = std::chrono::duration<double>::zero();
    for (int i = 0; i < iterations; ++i)
    {
        auto message = generate_random_message(message_length);
        auto start = std::chrono::high_resolution_clock::now();
        crc_slow(message, polynomial);
        auto end = std::chrono::high_resolution_clock::now();
        crc_slow_time += end - start;
    }
    std::cout << "crc_slow time: " << crc_slow_time.count() << " seconds\n";

    std::chrono::duration<double> crc_table_time = std::chrono::duration<double>::zero();
    for (int i = 0; i < iterations; ++i)
    {
        auto message = generate_random_message(message_length);
        auto start = std::chrono::high_resolution_clock::now();
        crc_table(message, table);
        auto end = std::chrono::high_resolution_clock::now();
        crc_table_time += end - start;
    }
    std::cout << "crc_table time: " << crc_table_time.count() << " seconds\n";

    std::chrono::duration<double> crc_reflect_slow_time = std::chrono::duration<double>::zero();
    for (int i = 0; i < iterations; ++i)
    {
        auto message = generate_random_message(message_length);
        auto start = std::chrono::high_resolution_clock::now();
        crc_reflect_slow(message, polynomial);
        auto end = std::chrono::high_resolution_clock::now();
        crc_reflect_slow_time += end - start;
    }
    std::cout << "crc_reflect_slow time: " << crc_reflect_slow_time.count() << " seconds\n";

    std::chrono::duration<double> crc_reflect_table_time = std::chrono::duration<double>::zero();
    for (int i = 0; i < iterations; ++i)
    {
        auto message = generate_random_message(message_length);
        auto start = std::chrono::high_resolution_clock::now();
        crc_reflect_table(message, reflect_table);
        auto end = std::chrono::high_resolution_clock::now();
        crc_reflect_table_time += end - start;
    }
    std::cout << "crc_reflect_table time: " << crc_reflect_table_time.count() << " seconds\n";

    return 0;
}
