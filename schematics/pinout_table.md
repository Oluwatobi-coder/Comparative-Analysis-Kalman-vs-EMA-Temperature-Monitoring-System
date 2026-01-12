| Component | Pin Label | Arduino Mega Pin |
|-----------|-----------|------------------|
| DHT11     | DATA      | 2                |
| Buzzer    | (+)       | 8                |
| LED       | (+)       | 13               |
| LCD (VSS) | GND       | GND              |
| LCD (VDD) | Power     | 5V               |
| LCD (V0)* | Contrast  | -                |
| LCD (RS)  | RS        | 12               |
| LCD (RW)  | RW        | GND              |
| LCD (E)   | E         | 11               |
| LCD (D4)  | D4        | 7                |
| LCD (D5)  | D5        | 6                |
| LCD (D6)  | D6        | 5                |
| LCD (D7)  | D7        | 4                |
| LCD (A)   | (+)       | 5V               |
| LCD (K)   | GND       | GND              |

**Note:** V0 should be connected to the middle pin of a 10 to 50k Ohm potentiometer and the other two pins to 5V and GND. Use `circuit_diagram.png` as a guide.
