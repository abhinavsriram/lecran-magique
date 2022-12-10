// #include <Stepper.h>

// // const String testLines[61] = 
// // {"SL 0 0",
// // "SL 40 70",
// // "SL 100 30",
// // "SL 61 66",
// // "SL 65 66",
// // "SL 67 66",
// // "SL 68 66",
// // "SL 69 68",
// // "SL 70 68",
// // "SL 71 70",
// // "SL 72 72",
// // "SL 73 75",
// // "SL 75 77",
// // "SL 76 80",
// // "SL 79 82",
// // "SL 84 85",
// // "SL 94 92",
// // "SL 102 96",
// // "SL 109 100",
// // "SL 114 102",
// // "SL 117 105",
// // "SL 121 108",
// // "SL 123 109",
// // "SL 124 112",
// // "SL 126 117",
// // "SL 128 122",
// // "SL 129 124",
// // "SL 129 130",
// // "SL 129 134",
// // "SL 129 140",
// // "SL 129 145",
// // "SL 129 150",
// // "SL 129 158",
// // "SL 131 167",
// // "SL 131 174",
// // "SL 133 182",
// // "SL 135 188",
// // "SL 137 196",
// // "SL 141 202",
// // "SL 143 206",
// // "SL 147 211",
// // "SL 149 214",
// // "SL 154 218",
// // "SL 157 221",
// // "SL 161 224",
// // "SL 167 228",
// // "SL 176 234",
// // "SL 189 242",
// // "SL 207 254",
// // "SL 221 264",
// // "SL 243 278",
// // "SL 285 297",
// // "SL 303 306",
// // "SL 323 324",
// // "SL 357 347",
// // "SL 373 356",
// // "SL 379 361",
// // "SL 384 364",
// // "SL 388 366",
// // "SL 389 367",
// // "SL 391 368"};

// const String testLines2[217] = 
// {
// "SL 0 0",
// "SL 1 2",
// "SL 1 7",
// "SL 1 12",
// "SL 1 19",
// "SL 1 24",
// "SL 1 28",
// "SL 1 32",
// "SL 1 34",
// "SL 1 38",
// "SL 1 40",
// "SL 1 42",
// "SL 1 46",
// "SL 1 48",
// "SL 1 50",
// "SL 3 57",
// "SL 3 60",
// "SL 4 64",
// "SL 5 68",
// "SL 5 72",
// "SL 6 76",
// "SL 7 82",
// "SL 8 91",
// "SL 9 96",
// "SL 11 102",
// "SL 11 106",
// "SL 11 111",
// "SL 12 113",
// "SL 12 115",
// "SL 13 116",
// "SL 14 118",
// "SL 14 119",
// "SL 15 120",
// "SL 15 121",
// "SL 16 121",
// "SL 17 122",
// "SL 19 123",
// "SL 20 125",
// "SL 23 126",
// "SL 25 128",
// "SL 26 130",
// "SL 29 132",
// "SL 31 132",
// "SL 34 134",
// "SL 37 134",
// "SL 39 135",
// "SL 42 135",
// "SL 44 135",
// "SL 46 135",
// "SL 47 135",
// "SL 50 135",
// "SL 52 134",
// "SL 53 133",
// "SL 56 132",
// "SL 57 131",
// "SL 59 130",
// "SL 60 128",
// "SL 63 126",
// "SL 65 124",
// "SL 67 123",
// "SL 69 120",
// "SL 73 119",
// "SL 76 117",
// "SL 79 116",
// "SL 81 114",
// "SL 83 113",
// "SL 86 112",
// "SL 87 111",
// "SL 88 111",
// "SL 89 110",
// "SL 91 110",
// "SL 91 109",
// "SL 93 108",
// "SL 94 108",
// "SL 95 106",
// "SL 97 106",
// "SL 98 104",
// "SL 99 104",
// "SL 101 103",
// "SL 103 103",
// "SL 105 102",
// "SL 107 102",
// "SL 109 102",
// "SL 111 102",
// "SL 114 102",
// "SL 116 102",
// "SL 118 102",
// "SL 120 102",
// "SL 121 102",
// "SL 123 102",
// "SL 124 102",
// "SL 125 103",
// "SL 126 104",
// "SL 127 105",
// "SL 127 106",
// "SL 128 107",
// "SL 129 107",
// "SL 129 108",
// "SL 129 109",
// "SL 129 110",
// "SL 129 111",
// "SL 130 112",
// "SL 131 112",
// "SL 131 113",
// "SL 131 114",
// "SL 132 117",
// "SL 134 119",
// "SL 134 122",
// "SL 135 125",
// "SL 135 128",
// "SL 136 132",
// "SL 137 136",
// "SL 137 138",
// "SL 137 141",
// "SL 138 142",
// "SL 138 144",
// "SL 140 146",
// "SL 140 148",
// "SL 141 150",
// "SL 141 152",
// "SL 142 154",
// "SL 143 156",
// "SL 144 159",
// "SL 144 162",
// "SL 146 164",
// "SL 147 166",
// "SL 147 168",
// "SL 147 169",
// "SL 149 170",
// "SL 149 173",
// "SL 150 174",
// "SL 151 176",
// "SL 152 178",
// "SL 153 179",
// "SL 154 180",
// "SL 155 180",
// "SL 156 181",
// "SL 157 182",
// "SL 158 182",
// "SL 159 182",
// "SL 160 182",
// "SL 161 182",
// "SL 162 182",
// "SL 163 182",
// "SL 165 182",
// "SL 166 182",
// "SL 169 182",
// "SL 172 182",
// "SL 177 181",
// "SL 181 180",
// "SL 185 180",
// "SL 188 179",
// "SL 191 178",
// "SL 194 178",
// "SL 196 178",
// "SL 199 177",
// "SL 201 176",
// "SL 203 176",
// "SL 205 174",
// "SL 207 174",
// "SL 211 172",
// "SL 215 170",
// "SL 219 168",
// "SL 224 167",
// "SL 229 166",
// "SL 233 164",
// "SL 236 164",
// "SL 239 162",
// "SL 241 162",
// "SL 245 160",
// "SL 247 160",
// "SL 249 160",
// "SL 254 159",
// "SL 257 159",
// "SL 260 159",
// "SL 263 159",
// "SL 266 159",
// "SL 268 160",
// "SL 270 160",
// "SL 271 160",
// "SL 273 161",
// "SL 275 162",
// "SL 276 164",
// "SL 277 166",
// "SL 279 167",
// "SL 280 168",
// "SL 283 170",
// "SL 285 173",
// "SL 289 176",
// "SL 293 180",
// "SL 298 185",
// "SL 303 189",
// "SL 307 194",
// "SL 311 198",
// "SL 313 202",
// "SL 316 207",
// "SL 319 212",
// "SL 319 216",
// "SL 321 220",
// "SL 323 226",
// "SL 324 229",
// "SL 325 232",
// "SL 327 236",
// "SL 327 238",
// "SL 328 240",
// "SL 331 244",
// "SL 333 247",
// "SL 335 249",
// "SL 336 252",
// "SL 337 254",
// "SL 337 256",
// "SL 337 257",
// "SL 337 259",
// "SL 337 260",
// "SL 337 262",
// "SL 337 263",
// "SL 337 264"
// };

// const int stepsPerRevolution = 30;
// int cursor_x = 0;
// int cursor_y = 0;
// int x_max = 320;
// int y_max = 220;
// bool resetting = false;
// Stepper myStepper1(stepsPerRevolution, 2, 3, 4, 5);
// Stepper myStepper2(stepsPerRevolution, 6, 7, 8, 9);
// int c = 0;

// void setup() {
//   myStepper1.setSpeed(500); // max speed = 500
//   myStepper2.setSpeed(500);
//   Serial.begin(9600);
// }

// void loop() {
//   // if (((cursor_x < x_max) && (cursor_y < y_max)) && !resetting) {
//     // Serial.println("Executing");
//     // while (Serial.available()) {}
//     if (c >= 1) {
//       return;
//     }
//     delay(1000);
//     // for (int i = 0; i < 61; i++) {
//     //   int spaceDelim1 = testLines[i].indexOf(" ");
//     //   int spaceDelim2 = testLines[i].lastIndexOf(" ");
//     //   int xCoord = testLines[i].substring(spaceDelim1 + 1, spaceDelim2).toInt() * 0.5;
//     //   int yCoord = testLines[i].substring(spaceDelim2 + 1).toInt() * 0.5;
//     //   plotLine(cursor_x, cursor_y, xCoord, yCoord);
//     // }

//     for (int i = 0; i < 217; i++) {
//       int spaceDelim1 = testLines2[i].indexOf(" ");
//       int spaceDelim2 = testLines2[i].lastIndexOf(" ");
//       int xCoord = testLines2[i].substring(spaceDelim1 + 1, spaceDelim2).toInt() * 0.5;
//       int yCoord = testLines2[i].substring(spaceDelim2 + 1).toInt() * 0.5;
//       // Serial.println(String(cursor_x) + ", " + String(cursor_y) + "; " + String(xCoord) + ", " + String(yCoord));
//       plotLine(cursor_x, cursor_y, xCoord, yCoord);
//       // delay(5000);
//     }

//     // for (int i = 0; i < 2276; i++) {
//     //   int spaceDelim1 = testLines3[i].indexOf(" ");
//     //   int spaceDelim2 = testLines3[i].lastIndexOf(" ");
//     //   int xCoord = testLines3[i].substring(spaceDelim1 + 1, spaceDelim2).toInt() * 0.5;
//     //   int yCoord = testLines3[i].substring(spaceDelim2 + 1).toInt() * 0.5;
//     //   plotLine(cursor_x, cursor_y, xCoord, yCoord);
//     //   // delay(5000);
//     // }

//     // plotLine(0, 0, 100, 0);
//     c++;
//   // Serial.println("cursor_x: ");
//   // Serial.println(cursor_x);
//   // Serial.println("cursor_y: ");
//   // Serial.println(cursor_y);
//   // delay(2);
// }

// void right(){
//     myStepper1.step(-stepsPerRevolution);
//     cursor_x += 1;
// }
// void left(){
//     myStepper1.step(stepsPerRevolution);
//     cursor_x -= 1;
// }
// void up(){
//     myStepper2.step(stepsPerRevolution);
//     cursor_y -= 1;
// }
// void down(){
//      myStepper2.step(-stepsPerRevolution);
//     cursor_y += 1;
// }
// void reset_cursor(){
//      while(cursor_x > 0){
//       left();
//         Serial.println("cursor_x: ");
//         Serial.println(cursor_x);
//      }
//      for (int i = 0; i<15; i++){
//       left();
//      }
//      while(cursor_y > 0){
//       up();
//         Serial.println("cursor_y: ");
//         Serial.println(cursor_y);
//      }
//      for (int i = 0; i<15; i++){
//       up();
//      }
//      cursor_x = 0;
//      cursor_y = 0;
// }

// void plotLineLow(int x0, int y0, int x1, int y1) {
//     int dx = x1 - x0;
//     int xi = 1;
//     if (dx < 0) {
//       xi = -1;
//       dx = -dx;
//     }
//     int dy = y1 - y0;
//     int yi = 1;
//     if (dy < 0) {
//         yi = -1;
//         dy = -dy;
//     }
//     int D = (2 * dy) - dx;
//     int y = y0;
//     bool hasYChanged = false;
//     for (int x = x0; ; x += xi) {
//         if (xi == 1) {
//           if (x > x1) {
//             break;
//           }
//         }
//         if (xi == -1) {
//           if (x < x1) {
//             break;
//           }
//         }
//         Serial.println(String(x) + ", " + String(y));
//         if (hasYChanged) {
//           myStepper2.step(-stepsPerRevolution * yi);
//           delay(3);
//         }
//         if (x != x0) {
//           myStepper1.step(stepsPerRevolution * xi);
//           delay(3);
//         }
//         if (D > 0) {
//             y = y + yi;
//             D = D + (2 * (dy - dx));
//             hasYChanged = true;
//         } else {
//             D = D + 2*dy;
//             hasYChanged = false;
//         }
//     }
// }

// void plotLineHigh(int x0, int y0, int x1, int y1) {
//     int dx = x1 - x0;
//     int dy = y1 - y0;
//     int xi = 1;
//     if (dx < 0) {
//         xi = -1;
//         dx = -dx;
//     }
//     int yi = 1;
//     if (dy < 0) {
//       yi = -1;
//       dy = -dy;
//     }
//     int D = (2 * dx) - dy;
//     int x = x0;
//     bool hasXChanged = false;

//     for (int y = y0; ; y += yi) {
//         if (yi == 1) {
//           if (y > y1) {
//             break;
//           }
//         }
//         if (yi == -1) {
//           if (y < y1) {
//             break;
//           }
//         }
//         Serial.println(String(x) + ", " + String(y));
//         if (hasXChanged) {
//           myStepper1.step(stepsPerRevolution * xi);
//           delay(3);
//         }
//         if (y != y0) {
//           myStepper2.step(-stepsPerRevolution * yi);
//           delay(3);
//         }
//         if (D > 0) {
//             x = x + xi;
//             D = D + (2 * (dx - dy));
//             hasXChanged = true;
//         } else {
//             D = D + 2*dx;
//             hasXChanged = false;
//         }
//     }
// }

// void plotLine(int x0, int y0, int x1, int y1) {
//     if (abs(y1 - y0) < abs(x1 - x0)) {
//         if (x0 > x1) {
//             // plotLineLow(x1, y1, x0, y0);
//             plotLineLow(x0, y0, x1, y1);
//         } else {
//             // plotLineLow(x0, y0, x1, y1);
//             plotLineLow(x0, y0, x1, y1);
//         }
//     } else {
//         if (y0 > y1) {
//             // plotLineHigh(x1, y1, x0, y0);
//             plotLineHigh(x0, y0, x1, y1);
//         } else {
//             plotLineHigh(x0, y0, x1, y1);
//         }
//     }
//     cursor_x = x1;
//     cursor_y = y1;
// }

// // void drawLine(int targetX, int targetY) {
// //   int dx = abs(targetX - cursor_x);
// //   int sx = cursor_x < targetX ? 1 : -1;
// //   int dy = -abs(targetY - cursor_y);
// //   int sy = cursor_y < targetY ? 1 : -1;
// //   int error = dx + dy;
// //   int e2;
// //   boolean thereYet = false;
// //   while(true) {
// //     if(cursor_x == targetX && cursor_y == targetY) break;
// //     /*
// //     Serial.print("Current x,y = ");
// //     Serial.print(currentX);
// //     Serial.print(", ");
// //     Serial.println(currentY);
// //     */
// //     e2 = 2 * error;
// //     if (e2 >= dy) { 
// //       if (cursor_x == targetX) break;
// //       error += dy;
// //       myStepper1.step(sx); 
// //       // delay(STEP_DELAY);
// //       cursor_x += sx;
// //     }
// //     if (e2 <= dx) { 
// //       if (cursor_y == targetY) break;
// //       error += dx; 
// //       myStepper2.step(sy); 
// //       // delay(STEP_DELAY);
// //       cursor_y += sy;
// //     }
// //   }
// // }

// // void drawLine(int x, int y) {
// //   int deltaX = x - cursor_x;
// //   int deltaY = y - cursor_y;
// //   int xDir = deltaX > 0 ? 1 : -1;
// //   int yDir = deltaY > 0 ? 1 : -1;
// //   int slope;
// //   if (deltaX >= deltaY) {
// //     slope = deltaX / deltaY;
// //     while (cursor_x != x && cursor_y != y) {
// //       for (int i = 0; i < slope; i++) {
// //         if (xDir == 1) {
// //           right();
// //         } else {
// //           left();
// //         }
// //       }
// //       if (yDir == 1) {
// //         down();
// //       } else {
// //         up();
// //       }
// //     }
// //   } else {
// //     slope = deltaY / deltaX;
// //     while (cursor_x != x && cursor_y != y) {
// //       for (int i = 0; i < slope; i++) {
// //         if (yDir == 1) {
// //           down();
// //         } else {
// //           up();
// //         }
// //       }
// //       if (xDir == 1) {
// //         right();
// //       } else {
// //         left();
// //       }
// //     }
// //   }
// // }
