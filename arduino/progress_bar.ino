//int pwm_val_1 = 0;
//int pwm_val_2 = 0;
//int pwm_val_3 = 0;
//int LED_PIN_1 = 0;
//int LED_PIN_2 = 1;
//int LED_PIN_3 = 10;
//int totalLinesToDraw = 100;
//int totalLinesProcessed = 0;
//int phase_size;
//int increment_size;
//int end_sequence = 0;
//
//void setup() {
//  phase_size = totalLinesToDraw/3; 
//  increment_size = 255/phase_size;
//}
//
//void loop() {
//   if (totalLinesProcessed <= phase_size){
//       analogWrite(LED_PIN_1, pwm_val_1);
//       pwm_val_1 += increment_size;
//        totalLinesProcessed++;
//   }else if (totalLinesProcessed <= phase_size*2){
//       analogWrite(LED_PIN_2, pwm_val_2);
//       pwm_val_2 += increment_size;
//        totalLinesProcessed++;
//   }else if (totalLinesProcessed < phase_size*3){
//       analogWrite(LED_PIN_3, pwm_val_3);
//       pwm_val_3 += increment_size;
//        totalLinesProcessed++;
//   } else if (totalLinesProcessed >= phase_size*3 && end_sequence <= 5){
//       analogWrite(LED_PIN_1, 0);
//       analogWrite(LED_PIN_2, 0);
//       analogWrite(LED_PIN_3, 0);
//       delay(500);
//       analogWrite(LED_PIN_1, 255);
//       analogWrite(LED_PIN_2, 255);
//       analogWrite(LED_PIN_3, 255);
//       delay(500);
//       end_sequence++;
//   }
//   delay(100);
//}
