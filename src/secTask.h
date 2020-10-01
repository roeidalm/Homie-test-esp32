#include <IRremote.h>

TaskHandle_t Task1;

#if defined(ESP32)
int IR_RECEIVE_PIN = 15;
#else
int IR_RECEIVE_PIN = 11;
#endif
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

void Task1code(void *parameter)
{
    for (;;)
    {
        if (irrecv.decode(&results))
        {
            Homie.getLogger() << "recieve IR: " << results.value << endl;

            irrecv.resume(); // Receive the next value
        }
        delay(100);
    }
}